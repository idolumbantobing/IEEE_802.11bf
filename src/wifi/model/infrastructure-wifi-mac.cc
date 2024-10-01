/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017
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
 * Author: Ido Manuel Lumbantobing <idomanueltobing@gmail.com>
 */

#include "infrastructure-wifi-mac.h"

#include "wifi-phy.h"

#include "ns3/he-frame-exchange-manager.h"
#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("InfrastructureWifiMac");

NS_OBJECT_ENSURE_REGISTERED(InfrastructureWifiMac);

TypeId
InfrastructureWifiMac::GetTypeId(void)
{
    static TypeId tid =
        TypeId("ns3::InfrastructureWifiMac")
            .SetParent<WifiMac>()
            .SetGroupName("Wifi")
            .AddAttribute("CfpMaxDuration",
                          "The maximum size of the CFP (used when supports PCF)",
                          TimeValue(MicroSeconds(51200)),
                          MakeTimeAccessor(&InfrastructureWifiMac::m_cfpMaxDuration),
                          MakeTimeChecker())
            ;
    return tid;
}

InfrastructureWifiMac::InfrastructureWifiMac()
    : m_WiFiSensingSupported(false),
      m_csSupported(false),
      m_muMimoSupported(false),
      m_muSensingSupported(false),
      m_currentPacket(0),
      m_currentTxop(0),
      m_lastNavStart(Seconds(0)),
      m_lastNavDuration(Seconds(0)),
      m_cfpStart(NanoSeconds(0)),
      m_lastBeacon(Seconds(0)),
      m_cfpForeshortening(Seconds(0)),
      m_currentMpdu(0)
{
    NS_LOG_FUNCTION(this);

    m_txop = CreateObject<Txop>();
    GetTxop()->SetInfMac(this);
    m_txop->SetTxMiddle(m_txMiddle);
    // m_txop->SetTxOkCallback(MakeCallback(&InfrastructureWifiMac::TxOk, this));
    // m_txop->SetTxFailedCallback(MakeCallback(&InfrastructureWifiMac::TxFailed, this));
    m_txop->SetTxDroppedCallback(MakeCallback(&InfrastructureWifiMac::NotifyTxDrop, this));
}

InfrastructureWifiMac::~InfrastructureWifiMac()
{
    NS_LOG_FUNCTION(this);
}

void
InfrastructureWifiMac::SetQosSupported(bool enable)
{
    // NS_ASSERT_MSG(!(GetPcfSupported() && enable),
    //               "QoS cannot be enabled when PCF support is activated (not supported)");
    WifiMac::SetQosSupported(enable);
}

void
InfrastructureWifiMac::SetPcfSupported(bool enable)
{
    m_WiFiSensingSupported = enable;
}

bool
InfrastructureWifiMac::GetPcfSupported() const
{
    return m_WiFiSensingSupported;
}

void
InfrastructureWifiMac::SetMac(const Ptr<WifiMac> mac)
{
    m_mac = mac;
}

/*
 *************************************
 Attempt to add PCF from ns3.33
 Public Functions and Attributes for Infratructure Wifi Mac
 *************************************
*/

bool
InfrastructureWifiMac::IsCfPeriod(uint8_t linkId = 0U) const
{
    NS_LOG_FUNCTION(this);
    return (GetPcfSupported() && m_cfpStart.IsStrictlyPositive());
}

void
InfrastructureWifiMac::SetCfpMaxDuration(Time cfpMaxDuration)
{
    m_cfpMaxDuration = cfpMaxDuration;
}

Time
InfrastructureWifiMac::GetCfpMaxDuration(void) const
{
    return m_cfpMaxDuration;
}

Time
InfrastructureWifiMac::GetRemainingCfpDuration(void) const
{
    NS_LOG_FUNCTION(this);
    Time remainingCfpDuration =
        std::min(m_cfpStart,
                 m_cfpStart + m_cfpMaxDuration - Simulator::Now() - m_cfpForeshortening);
    if (!remainingCfpDuration.IsPositive())
    {
        return Seconds(0);
    }
    // NS_ASSERT(remainingCfpDuration.IsPositive());
    return remainingCfpDuration;
}

bool
InfrastructureWifiMac::GetCtsToSelfSupported() const
{
    return WifiMac::GetCtsToSelfSupported();
}

void
InfrastructureWifiMac::SetInfo(Ptr<Txop> txop, Ptr<const WifiMpdu> mpdu)
{
    m_currentPacket = Create<WifiPsdu>(mpdu, true);
    m_currentMpdu = mpdu;
    m_currentTxop = txop;
}

void
InfrastructureWifiMac::StartCfPeriod()
{
    NS_LOG_FUNCTION(this);
    m_cfpStart = Simulator::Now();
    m_cfpForeshortening = NanoSeconds(0);
}

void
InfrastructureWifiMac::StopCfPeriod()
{
    NS_LOG_FUNCTION(this);
    m_cfpStart = NanoSeconds(0);
    m_cfpForeshortening = NanoSeconds(0);
}

void
InfrastructureWifiMac::SetTxop(Ptr<Txop> txop)
{
    m_currentTxop = txop;
}

/*
 *************************************
 Attempt to add PCF from ns3.33
 Protected Functions and Attributes for Infratructure Wifi Mac
 *************************************
*/

void
InfrastructureWifiMac::EndTxNoAck(uint8_t linkId)
{
    NS_LOG_FUNCTION(this);
    NS_ASSERT(GetPcfSupported());

    if (m_currentPacket == 0)
    {
        m_cfpStart = Simulator::Now();
    }

    if (m_currentPacket->GetHeader(0).IsBeacon())
    {
        // StartCfPeriod();
        m_currentTxop->EndTxNoAck(linkId, m_currentMpdu);
    }
    else if (m_currentPacket->GetHeader(0).IsCfEnd())
    {
        m_currentTxop->EndTxNoAck(linkId, m_currentMpdu, false);
    }
    if (!IsCfPeriod(linkId))
    {
        m_currentTxop = 0;
    }
}

WifiTxVector
InfrastructureWifiMac::GetRtsTxVector(Ptr<const WifiMpdu> item, uint8_t linkId) const
{
    return GetWifiRemoteStationManager(linkId)->GetRtsTxVector(item->GetHeader().GetAddr1());
}
/*
 *************************************
 Attempt to add PCF from ns3.33
 Private Functions and Attributes for Infratructure Wifi Mac
 *************************************
*/

/*
 *************************************
 Attempt to add PCF from ns3.33
 Integration MAC Low Transmission Parameters in Infrastructure MAC
 Public, Protected, Private Function and Attributes for MAC Low Transmission Parameters
 *************************************
*/

/*
 *************************************
 Attempt to add Channel Sounding from ns3.37
 Public Functions and Attributes for Infratructure Wifi Mac
 *************************************
*/

} // namespace ns3
