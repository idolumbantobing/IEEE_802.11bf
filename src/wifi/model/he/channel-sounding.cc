/*
 * Copyright (c) 2023
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jingyuan Zhang <jingyuan_z@gatech.edu>
 */

#include "channel-sounding.h"

#include "he-configuration.h"

#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/wifi-protection.h"
#include <ns3/nstime.h>

#include <algorithm>
#include <functional>

namespace ns3
{
/***********************************************************
 *          Channel sounding
 ***********************************************************/
NS_LOG_COMPONENT_DEFINE("ChannelSounding");

NS_OBJECT_ENSURE_REGISTERED(ChannelSounding);

ChannelSounding::ChannelSounding()
{
}

ChannelSounding::~ChannelSounding()
{
}

TypeId
ChannelSounding::GetTypeId()
{
    static TypeId tid = TypeId("ns3::ChannelSounding")
                            .SetParent<Object>()
                            .SetGroupName("Wifi")
                            .AddConstructor<ChannelSounding>();

    return tid;
}

void
ChannelSounding::DoDispose(void)
{
    NS_LOG_FUNCTION(this);

    Object::DoDispose();
}

void
ChannelSounding::DoInitialize(void)
{
    NS_LOG_FUNCTION(this);
}

uint16_t
ChannelSounding::GetBfReportLength(uint16_t bandwidth,
                                   uint8_t ng,
                                   uint8_t nc,
                                   uint8_t nr,
                                   uint8_t codeBookSize,
                                   HeMimoControlHeader::CsType type)
{
    NS_ASSERT((ng == 4) || (ng == 16));
    NS_ASSERT(codeBookSize <= 1);

    uint16_t numBytes = nc;

    uint8_t ruStart = 0;
    uint8_t ruEnd = 0;
    if (bandwidth == 20)
    {
        ruEnd = 8;
    }
    else if (bandwidth == 40)
    {
        ruEnd = 17;
    }
    else if (bandwidth == 80)
    {
        ruEnd = 36;
    }
    else if (bandwidth == 160)
    {
        ruEnd = 73;
    }
    else
    {
        NS_FATAL_ERROR("Improper bandwidth:" << bandwidth);
    }

    uint8_t angleBits = 0;

    uint16_t ns = HeCompressedBfReport::GetNSubcarriers(ruStart, ruEnd, ng);
    if (type == HeMimoControlHeader::CQI)
    {
        angleBits = 0;
        numBytes = 0;
        NS_FATAL_ERROR("Unsupported type of channel sounding: CQI.");
    }
    else if (type == HeMimoControlHeader::SU)
    {
        if (codeBookSize == 0)
        {
            angleBits = 6;
        }
        else if (codeBookSize == 1)
        {
            angleBits = 10;
        }
        numBytes += ceil((HeCompressedBfReport::CalculateNa(nc, nr) / 2 * angleBits * ns) / 8.0);
    }
    else if (type == HeMimoControlHeader::MU)
    {
        if (codeBookSize == 0)
        {
            angleBits = 12;
        }
        else if (codeBookSize == 1)
        {
            angleBits = 16;
        }
        numBytes += ceil(
            (4 * nc * ns + HeCompressedBfReport::CalculateNa(nc, nr) / 2 * angleBits * ns) / 8.0);
    }
    else
    {
        NS_FATAL_ERROR("Improper channel sounding type");
    }
    return numBytes;
}

/***********************************************************
 *          Channel sounding for Beamformer
 ***********************************************************/
NS_OBJECT_ENSURE_REGISTERED(CsBeamformer);

TypeId
CsBeamformer::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::CsBeamformer")
                            .SetParent<ChannelSounding>()
                            .AddConstructor<CsBeamformer>()
                            .SetGroupName("Wifi");

    return tid;
}

CsBeamformer::CsBeamformer()
{
    m_sendNdpa = false;
    m_sendNdp = false;
    m_lastCs = MilliSeconds(0);
}

CsBeamformer::~CsBeamformer()
{
}

void
CsBeamformer::DoDispose()
{
    NS_LOG_FUNCTION(this);
    ClearAllInfo();
    ChannelSounding::DoDispose();
}

void
CsBeamformer::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    ChannelSounding::DoInitialize();
}

void
CsBeamformer::ClearChannelInfo()
{
    auto staChannelInfo = m_channelInfoList.begin();
    while (staChannelInfo != m_channelInfoList.end())
    {
        staChannelInfo->second.m_stStreamSnr.clear();

        for (uint16_t i = 0; i < staChannelInfo->second.m_deltaSnr.size(); i++)
        {
            staChannelInfo->second.m_deltaSnr[i].clear();
        }
        staChannelInfo->second.m_deltaSnr.clear();

        for (uint16_t i = 0; i < staChannelInfo->second.m_phi.size(); i++)
        {
            staChannelInfo->second.m_phi[i].clear();
        }
        staChannelInfo->second.m_phi.clear();

        for (uint16_t i = 0; i < staChannelInfo->second.m_psi.size(); i++)
        {
            staChannelInfo->second.m_psi[i].clear();
        }
        staChannelInfo->second.m_psi.clear();

        staChannelInfo++;
    }
    m_channelInfoList.clear();
}

void
CsBeamformer::ClearAllInfo()
{
    ClearChannelInfo();
    m_csStaIdList.clear();
}

void
CsBeamformer::PrintChannelInfo(void)
{
    auto channelInfo = m_channelInfoList.begin();
    while (channelInfo != m_channelInfoList.end())
    {
        NS_LOG_INFO("STA ID:" << channelInfo->first);
        for (uint16_t i = 0; i < channelInfo->second.m_stStreamSnr.size(); i++)
        {
            NS_LOG_INFO("Average SNR of stream "
                        << std::to_string(i) << ":"
                        << std::to_string(channelInfo->second.m_stStreamSnr[i]));
        }

        for (uint16_t i = 0; i < channelInfo->second.m_phi.size(); i++)
        {
            NS_LOG_INFO("Subcarrier " << std::to_string(i));
            for (uint16_t j = 0; j < channelInfo->second.m_phi[0].size(); j++)
            {
                NS_LOG_INFO("Angle Phi " << std::to_string(j) << ":"
                                         << channelInfo->second.m_phi[i][j]);
                NS_LOG_INFO("Angle Psi " << std::to_string(j) << ":"
                                         << channelInfo->second.m_psi[i][j]);
            }
        }

        for (uint16_t i = 0; i < channelInfo->second.m_deltaSnr.size(); i++)
        {
            NS_LOG_INFO("Subcarrier " << std::to_string(i));
            for (uint16_t j = 0; j < channelInfo->second.m_deltaSnr[0].size(); j++)
            {
                NS_LOG_INFO("DeltaSnr " << std::to_string(j) << ":"
                                        << std::to_string(channelInfo->second.m_deltaSnr[i][j]));
            }
        }
        channelInfo++;
    }
}

bool
CsBeamformer::CheckChannelSounding(Time interval)
{
    Time currentTime = Simulator::Now();
    if (m_lastCs.IsZero() ||
        (currentTime.GetMilliSeconds() - m_lastCs.GetMilliSeconds() > interval.GetMilliSeconds()))
    {
        //   m_lastCs = currentTime;
        return true;
    }
    else
    {
        return false;
    }
}

void
CsBeamformer::SetLastCsTime(Time time)
{
    m_lastCs = time;
}

void
CsBeamformer::GenerateNdpaFrame(Mac48Address apAddress,
                                std::list<Mac48Address> staMacAddrList,
                                uint16_t bandwidth,
                                Ptr<WifiRemoteStationManager> remoteStaManager)
{
    if (staMacAddrList.empty())
    {
        NS_LOG_ERROR("Cannot generate NDPA frame due to empty list of beamformees.");
    }
    m_csStaIdList.clear();

    Mac48Address receiver =
        staMacAddrList.size() > 1 ? Mac48Address::GetBroadcast() : *staMacAddrList.begin();
    WifiMacHeader hdrNdpa(WIFI_MAC_CTL_NDPA);
    hdrNdpa.SetAddr1(receiver);
    hdrNdpa.SetAddr2(apAddress);
    hdrNdpa.SetDsNotTo();
    hdrNdpa.SetDsNotFrom();

    uint8_t ruStart = 0;
    uint8_t ruEnd = 0;
    if (bandwidth == 20)
    {
        ruEnd = 8;
    }
    else if (bandwidth == 40)
    {
        ruEnd = 17;
    }
    else if (bandwidth == 80)
    {
        ruEnd = 36;
    }
    else if (bandwidth == 160)
    {
        ruEnd = 73;
    }
    else
    {
        NS_FATAL_ERROR("Improper bandwidth:" << bandwidth);
    }

    CtrlNdpaHeader ndpaHeader;
    ndpaHeader.SetSoundingDialogToken(1);
    auto staIt = staMacAddrList.begin();
    while (staIt != staMacAddrList.end())
    {
        CtrlNdpaHeader::StaInfo sta;
        uint32_t staId = remoteStaManager->GetAssociationId(*staIt);
        sta.m_aid11 = staId & 0x000007ff;
        sta.m_ruStart = ruStart;
        sta.m_ruEnd = ruEnd;
        sta.m_disambiguation = 1;
        uint8_t codebook;
        if (staMacAddrList.size() > 1)
        {
            codebook = remoteStaManager->GetStationHeCapabilities(*staIt)->GetCodebookSizeforSu() ==
                       "(6,4)";
        }
        else
        {
            codebook = remoteStaManager->GetStationHeCapabilities(*staIt)->GetCodebookSizeforMu() ==
                       "(9,7)";
        }
        sta.m_codebookSize = codebook;
        sta.m_nc = remoteStaManager->GetStationHeCapabilities(*staIt)->GetMaxNc();

        if (staMacAddrList.size() > 1)
        {
            uint8_t ng = remoteStaManager->GetStationHeCapabilities(*staIt)->GetNgforMuFeedback();
            switch (ng)
            {
            case 4:
                sta.m_feedbackTypeNg = 2;
                break;
            case 16:
                sta.m_feedbackTypeNg = 3;
                sta.m_codebookSize = 1;
                break;
            default:
                NS_FATAL_ERROR("Unsupported subcarrier grouping Ng = " << ng);
                break;
            }
        }
        else
        {
            uint8_t ng = remoteStaManager->GetStationHeCapabilities(*staIt)->GetNgforSuFeedback();
            switch (ng)
            {
            case 4:
                sta.m_feedbackTypeNg = 0;
                break;
            case 16:
                sta.m_feedbackTypeNg = 1;
                break;
            default:
                NS_FATAL_ERROR("Unsupported subcarrier grouping Ng = " << ng);
                break;
            }
        }

        ndpaHeader.AddStaInfoField(sta);
        m_csStaIdList.push_back(staId);
        staIt++;

        NS_LOG_DEBUG("NDPA frame generation:");
        NS_LOG_DEBUG("STA Info Ru Start=" << std::to_string(sta.m_ruStart));
        NS_LOG_DEBUG("STA Info Ru End=" << std::to_string(sta.m_ruEnd));
        NS_LOG_DEBUG("STA Info Feedback Type and Ng=" << std::to_string(sta.m_feedbackTypeNg));
        NS_LOG_DEBUG("STA Info Disambiguation=" << std::to_string(sta.m_disambiguation));
        NS_LOG_DEBUG("STA Info Codebook Size=" << std::to_string(sta.m_codebookSize));
        NS_LOG_DEBUG("STA Info Nc=" << std::to_string(sta.m_nc));
    }

    Ptr<Packet> packetNdpa = Create<Packet>();
    packetNdpa->AddHeader(ndpaHeader);
    m_beamformerFrameInfo.m_ndpa = Create<WifiMpdu>(packetNdpa, hdrNdpa);
}

void
CsBeamformer::GetBfReportInfo(Ptr<const WifiMpdu> bfReport, uint16_t staId)
{
    ChannelInfo staChannelInfo;

    Ptr<Packet> bfPacket = bfReport->GetPacket()->Copy();

    // Get HE action field
    WifiActionHeader actionHdr;
    bfPacket->RemoveHeader(actionHdr);

    // Get HE MIMO Control Info field
    HeMimoControlHeader heMimoControlHeader;
    bfPacket->RemoveHeader(heMimoControlHeader);

    // Get Compressed Beamforming Report field
    HeCompressedBfReport heCompressedBfReport(heMimoControlHeader);
    bfPacket->RemoveHeader(heCompressedBfReport);

    staChannelInfo.m_stStreamSnr = heCompressedBfReport.GetChannelInfo().m_stStreamSnr;
    staChannelInfo.m_phi = heCompressedBfReport.GetChannelInfo().m_phi;
    staChannelInfo.m_psi = heCompressedBfReport.GetChannelInfo().m_psi;

    // Get MU Exclusive Beamforming Report field
    if (heMimoControlHeader.GetFeedbackType() == HeMimoControlHeader::MU)
    {
        HeMuExclusiveBfReport heMuExclusiveBfReport(heMimoControlHeader);
        bfPacket->RemoveHeader(heMuExclusiveBfReport);
        staChannelInfo.m_deltaSnr = heMuExclusiveBfReport.GetDeltaSnr();
    }

    m_channelInfoList.erase(staId);
    m_channelInfoList.insert(std::pair<uint16_t, ChannelInfo>(staId, staChannelInfo));
}

std::list<uint16_t>
CsBeamformer::CheckAllChannelInfoReceived(void)
{
    std::list<uint16_t> staList;
    auto staIt = m_csStaIdList.begin();
    while (staIt != m_csStaIdList.end())
    {
        if (m_channelInfoList.find(*staIt) == m_channelInfoList.end())
        {
            staList.push_back(*staIt);
        }
        staIt++;
    }
    return staList;
}

CsBeamformer::BeamformerFrameInfo&
CsBeamformer::GetBeamformerFrameInfo()
{
    return m_beamformerFrameInfo;
}

std::map<uint16_t, CsBeamformer::ChannelInfo>
CsBeamformer::GetChannelInfoList() const
{
    return m_channelInfoList;
}

uint8_t
CsBeamformer::GetNumCsStations() const
{
    return m_csStaIdList.size();
}

std::list<uint16_t>
CsBeamformer::GetCsStaIdList() const
{
    return m_csStaIdList;
}

void
CsBeamformer::SetTxParameters(WifiTxParameters txParams, std::string frameType)
{
    if (frameType == "NDPA")
    {
        m_beamformerFrameInfo.m_txParamsNdpa = txParams;
    }
    else if (frameType == "NDP")
    {
        m_beamformerFrameInfo.m_txParamsNdp = txParams;
    }
    else if (frameType == "Trigger")
    {
        m_beamformerFrameInfo.m_txParamsBfrpTrigger = txParams;
    }
    else
    {
        NS_FATAL_ERROR("Unrecognized frame type.");
    }
}

void
CsBeamformer::SetBeamformerFrames(Ptr<WifiMpdu> mdpu, std::string frameType)
{
    if (frameType == "NDPA")
    {
        m_beamformerFrameInfo.m_ndpa = mdpu;

        m_csStaIdList.clear();
        CtrlNdpaHeader ndpaHeader;
        mdpu->GetPacket()->PeekHeader(ndpaHeader);
        auto sta = ndpaHeader.begin();
        while (sta != ndpaHeader.end())
        {
            m_csStaIdList.push_back(sta->m_aid11);
            sta++;
        }
    }
    else if (frameType == "NDP")
    {
        m_beamformerFrameInfo.m_ndp = mdpu;
    }
    else if (frameType == "Trigger")
    {
        m_beamformerFrameInfo.m_trigger = mdpu;
    }
    else
    {
        NS_FATAL_ERROR("Unrecognized frame type.");
    }
}

void
CsBeamformer::SetNdpaSent(bool flag)
{
    m_sendNdpa = flag;
}

void
CsBeamformer::SetNdpSent(bool flag)
{
    m_sendNdp = flag;
}

bool
CsBeamformer::IsNdpaSent(void)
{
    return m_sendNdpa;
}

bool
CsBeamformer::IsNdpSent(void)
{
    return m_sendNdp;
}

/***********************************************************
 *          Channel sounding for Beamformee
 ***********************************************************/
NS_OBJECT_ENSURE_REGISTERED(CsBeamformee);

TypeId
CsBeamformee::GetTypeId(void)
{
    static TypeId tid = TypeId("ns3::CsBeamformee")
                            .SetParent<ChannelSounding>()
                            .AddConstructor<CsBeamformee>()
                            .SetGroupName("Wifi");

    return tid;
}

CsBeamformee::CsBeamformee()
{
    m_receiveNdpa = false;
    m_receiveNdp = false;
}

CsBeamformee::~CsBeamformee()
{
}

void
CsBeamformee::DoDispose()
{
    NS_LOG_FUNCTION(this);

    m_bfReport = nullptr;
    ClearChannelInfo();
    ChannelSounding::DoDispose();
}

void
CsBeamformee::DoInitialize()
{
    NS_LOG_FUNCTION(this);
    ChannelSounding::DoInitialize();
}

void
CsBeamformee::ClearChannelInfo()
{
    m_channelInfo.m_stStreamSnr.clear();

    for (uint16_t i = 0; i < m_channelInfo.m_deltaSnr.size(); i++)
    {
        m_channelInfo.m_deltaSnr[i].clear();
    }
    m_channelInfo.m_deltaSnr.clear();

    for (uint16_t i = 0; i < m_channelInfo.m_phi.size(); i++)
    {
        m_channelInfo.m_phi[i].clear();
    }
    m_channelInfo.m_phi.clear();

    for (uint16_t i = 0; i < m_channelInfo.m_psi.size(); i++)
    {
        m_channelInfo.m_psi[i].clear();
    }
    m_channelInfo.m_psi.clear();
}

void
CsBeamformee::PrintChannelInfo(void)
{
    for (uint16_t i = 0; i < m_channelInfo.m_stStreamSnr.size(); i++)
    {
        NS_LOG_INFO("Average SNR of stream " << std::to_string(i) << ":"
                                             << std::to_string(m_channelInfo.m_stStreamSnr[i]));
    }

    for (uint16_t i = 0; i < m_channelInfo.m_phi.size(); i++)
    {
        NS_LOG_INFO("Subcarrier " << std::to_string(i));
        for (uint16_t j = 0; j < m_channelInfo.m_phi[0].size(); j++)
        {
            NS_LOG_INFO("Angle Phi " << std::to_string(j) << ":" << m_channelInfo.m_phi[i][j]);
            NS_LOG_INFO("Angle Psi " << std::to_string(j) << ":" << m_channelInfo.m_psi[i][j]);
        }
    }

    for (uint16_t i = 0; i < m_channelInfo.m_deltaSnr.size(); i++)
    {
        NS_LOG_INFO("Subcarrier " << std::to_string(i));
        for (uint16_t j = 0; j < m_channelInfo.m_deltaSnr[0].size(); j++)
        {
            NS_LOG_INFO("DeltaSnr " << std::to_string(j) << ":"
                                    << std::to_string(m_channelInfo.m_deltaSnr[i][j]));
        }
    }
}

void
CsBeamformee::GetNdpaInfo(Ptr<const WifiMpdu> ndpa, uint16_t staId)
{
    NS_LOG_FUNCTION(this << ndpa << staId);
    uint16_t aid11 = staId & 0x07ff;

    CtrlNdpaHeader ndpaHeader;
    ndpa->GetPacket()->PeekHeader(ndpaHeader);
    m_heMimoControlHeader = HeMimoControlHeader(ndpaHeader, aid11);
}

void
CsBeamformee::GetNdpInfo(WifiTxVector txVector, uint16_t staId)
{
    NS_LOG_FUNCTION(this << txVector << staId);
    m_heMimoControlHeader.SetNr(txVector.GetNss(staId) - 1);
    m_heMimoControlHeader.SetBw(txVector.GetChannelWidth());

    ClearChannelInfo();
    CalculateChannelInfo();
    SetNdpReceived(true);
}

void
CsBeamformee::CalculateChannelInfo()
{
    HeCompressedBfReport heCompressedBfReport(m_heMimoControlHeader);

    uint16_t ns = heCompressedBfReport.GetNs();
    uint8_t na = heCompressedBfReport.GetNa();
    uint8_t nc = m_heMimoControlHeader.GetNc() + 1;
    uint8_t bits1 = heCompressedBfReport.GetBits1();
    uint8_t bits2 = heCompressedBfReport.GetBits2();

    Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable>();

    for (uint8_t i = 0; i < nc; i++)
    {
        m_channelInfo.m_stStreamSnr.push_back(x->GetInteger(0, std::pow(2, 8) - 1));
    }

    for (uint16_t i = 0; i < ns; i++)
    {
        std::vector<uint16_t> phi;
        std::vector<uint16_t> psi;
        std::vector<uint8_t> deltaSnr;

        for (uint8_t j = 0; j < na / 2; j++)
        {
            phi.push_back(x->GetInteger(0, std::pow(2, bits1) - 1));
            psi.push_back(x->GetInteger(0, std::pow(2, bits2) - 1));
        }

        for (uint8_t j = 0; j < nc; j++)
        {
            deltaSnr.push_back(x->GetInteger(0, std::pow(2, 4) - 1));
        }
        m_channelInfo.m_phi.push_back(phi);
        m_channelInfo.m_psi.push_back(psi);
        m_channelInfo.m_deltaSnr.push_back(deltaSnr);
    }
}

ChannelSounding::ChannelInfo
CsBeamformee::GetChannelInfo() const
{
    return m_channelInfo;
}

void
CsBeamformee::GenerateBfReport(uint16_t staId,
                               Mac48Address apAddress,
                               Mac48Address staAddress,
                               Mac48Address bssid)
{
    NS_LOG_FUNCTION(this << staId << apAddress << staAddress << bssid);
    WifiMacHeader hdr;
    hdr.SetType(WIFI_MAC_MGT_ACTION_NO_ACK);
    hdr.SetAddr1(apAddress);
    hdr.SetAddr2(staAddress);
    hdr.SetAddr3(bssid);
    hdr.SetDsNotTo();
    hdr.SetDsNotFrom();

    Ptr<Packet> packetBfReport = Create<Packet>();

    // Generate MU Exclusive Beamforming Report
    if (m_heMimoControlHeader.GetFeedbackType() == HeMimoControlHeader::MU)
    {
        HeMuExclusiveBfReport heMuExclusiveBfReport(m_heMimoControlHeader);
        heMuExclusiveBfReport.SetDeltaSnr(m_channelInfo.m_deltaSnr);
        packetBfReport->AddHeader(heMuExclusiveBfReport);
    }

    // Generate Compressed Beamforming Report
    HeCompressedBfReport heCompressedBfReport(m_heMimoControlHeader);
    heCompressedBfReport.SetChannelInfo(
        HeCompressedBfReport::ChannelInfo{m_channelInfo.m_stStreamSnr,
                                          m_channelInfo.m_phi,
                                          m_channelInfo.m_psi});
    packetBfReport->AddHeader(heCompressedBfReport);

    // Generate HE MIMO Control field
    packetBfReport->AddHeader(m_heMimoControlHeader);

    // Generate HE action header
    WifiActionHeader actionHdr;
    WifiActionHeader::ActionValue action;
    action.he = WifiActionHeader::HeActionValue::HE_COMPRESSED_BEAMFORMING_CQI;
    actionHdr.SetAction(WifiActionHeader::CategoryValue::HE, action);
    packetBfReport->AddHeader(actionHdr);

    m_bfReport = Create<WifiMpdu>(packetBfReport, hdr);
}

Ptr<WifiMpdu>
CsBeamformee::GetBfReport() const
{
    return m_bfReport;
}

HeMimoControlHeader
CsBeamformee::GetHeMimoControlHeader() const
{
    return m_heMimoControlHeader;
}

void
CsBeamformee::SetNdpaReceived(bool flag)
{
    m_receiveNdpa = flag;
}

void
CsBeamformee::SetNdpReceived(bool flag)
{
    m_receiveNdp = flag;
}

bool
CsBeamformee::IsNdpaReceived(void)
{
    return m_receiveNdpa;
}

bool
CsBeamformee::IsNdpReceived(void)
{
    return m_receiveNdp;
}

} // namespace ns3
