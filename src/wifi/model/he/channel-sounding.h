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

#ifndef CHANNEL_SOUNDING_H
#define CHANNEL_SOUNDING_H

#include "multi-user-scheduler.h"

#include "ns3/ctrl-headers.h"
#include "ns3/mac48-address.h"
#include "ns3/mgt-headers.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/wifi-psdu.h"
#include "ns3/wifi-remote-station-manager.h"
#include "ns3/wifi-tx-parameters.h"

#include <cstdint>
#include <map>
#include <ostream>
#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace ns3
{

class ChannelSounding : public Object
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();
    ChannelSounding();
    virtual ~ChannelSounding();

    struct ChannelInfo //!<  Channel information
    {
        std::vector<uint8_t> m_stStreamSnr; //!< Average SNR of space-time streams
        std::vector<std::vector<uint8_t>>
            m_deltaSnr; //!< Delta SNR information for each space-time stream
        std::vector<std::vector<uint16_t>>
            m_phi; //!< Phi angle (number of subcarriers * number of angles)
        std::vector<std::vector<uint16_t>>
            m_psi; //!< Psi angle (number of subcarriers * number of angles)
    };

    /**
     * Calculate the number of bytes in the beamforming report given channel sounding parameters
     * \param bandwidth bandwidth of channel required for CSI feedback
     * \param ng subcarrier grouping parameter (4 or 16)
     * \param nc number of columns in a compressed beamforming feedback matrix
     * \param nr number of rows in a compressed beamforming feedback matrix
     * \param codeBookSize codebook size (0 or 1)
     * \param type channel sounding type (SU, MU or CQI)
     *
     * \return number of bytes in the beamforming report
     */
    static uint16_t GetBfReportLength(uint16_t bandwidth,
                                      uint8_t ng,
                                      uint8_t nc,
                                      uint8_t nr,
                                      uint8_t codeBookSize,
                                      HeMimoControlHeader::CsType type);

  protected:
    void DoDispose() override;
    void DoInitialize() override;
};

class CsBeamformer : public ChannelSounding
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    CsBeamformer();
    virtual ~CsBeamformer();

    void ClearChannelInfo();
    void ClearAllInfo();
    void PrintChannelInfo();

    struct BeamformerFrameInfo
    {
        WifiTxParameters m_txParamsNdpa;        //!< TX parameters for NDPA
        WifiTxParameters m_txParamsNdp;         //!< TX parameters for NDP
        WifiTxParameters m_txParamsBfrpTrigger; //!< TX parameters for BF trigger
        Ptr<WifiMpdu> m_trigger;                //!< BFRP trigger Frame to send
        Ptr<WifiMpdu> m_ndpa;                   //!< NDPA Frame to send
        Ptr<WifiMpdu> m_ndp;                    //!< NDP Frame to send
    };

    /**
     * Check if channel sounding is needed in current TXOP
     * \param interval channel sounding interval
     * \return whether channel sounding is needed in current TXOP
     */
    virtual bool CheckChannelSounding(Time interval);

    void SetLastCsTime(Time time);

    /**
     * Generate NDPA frame at AP
     *
     * \param apAddress the MAC address of AP
     * \param staMacAddrList Mac addresses of stations
     * \param bandwidth channel bandwidth
     * \param remoteStaManager remote station manager
     */
    void GenerateNdpaFrame(Mac48Address apAddress,
                           std::list<Mac48Address> staMacAddrList,
                           uint16_t bandwidth,
                           Ptr<WifiRemoteStationManager> remoteStaManager);

    /**
     * Get channel information in the beamforming report frame
     *
     * \param bfReport beamforming report
     * \param staId STA ID of the station which sends the beamforming report
     */
    void GetBfReportInfo(Ptr<const WifiMpdu> bfReport, uint16_t staId);

    /**
     * Check whether channel information of all the stations is received
     *
     * \return list of stations that fail to feedback channel information
     */
    std::list<uint16_t> CheckAllChannelInfoReceived();

    /**
     * Get frames and Tx parameters for frames that will be sent from the beamformer
     *
     * \return a struct of BeamformerFrameInfo containing frames and Tx vectors
     */
    BeamformerFrameInfo& GetBeamformerFrameInfo();

    /**
     * Get channel information list
     *
     * \return a map of channel information and corresponding STA ID
     */
    std::map<uint16_t, ChannelInfo> GetChannelInfoList() const;

    /**
     * Set Tx parameters for frames that will be sent from the beamformer given frame type
     *
     * \param txParams Tx parameters
     * \param frameType frame type ("NDPA", "NDP", or "Trigger")
     */
    void SetTxParameters(WifiTxParameters txParams, std::string frameType);

    /**
     * Set MDPU that will be sent from the beamformer given frame type
     *
     * \param mdpu Wifi MDPU
     * \param frameType frame type ("NDPA", "NDP", or "Trigger")
     */
    void SetBeamformerFrames(Ptr<WifiMpdu> mdpu, std::string frameType);

    /**
     * Get the number of stations involved in channel sounding
     *
     * \return the number of stations involved in channel sounding
     */
    uint8_t GetNumCsStations() const;

    /**
     * Get the list of STA IDs for all the stations that the beamformer requests CSI for
     *
     * \return the list of STA IDs
     */
    std::list<uint16_t> GetCsStaIdList() const;

    /**
     * Set whether NDPA frame is sent out
     *
     * \param flag whether NDPA frame is sent out
     */
    void SetNdpaSent(bool flag);

    /**
     * Set whether NDP frame is sent out
     *
     * \param flag whether NDP frame is sent out
     */
    void SetNdpSent(bool flag);

    /**
     * Check whether NDPA frame is sent out
     *
     * \return whether NDPA frame is sent out
     */
    bool IsNdpaSent(void);

    /**
     * Check whether NDP frame is sent out
     *
     * \return whether NDP frame is sent out
     */
    bool IsNdpSent(void);

  protected:
    void DoDispose() override;
    void DoInitialize() override;

  private:
    bool m_sendNdpa; //! Whether NDPA frame is sent out
    bool m_sendNdp;  //! Whether NDP frame is sent out
    BeamformerFrameInfo
        m_beamformerFrameInfo; //!< Store channel sounding frames sent from Bthe beamformer
    std::map<uint16_t, ChannelInfo>
        m_channelInfoList; //!<  Store channel information sent from all the beamformees:  station
                           //!<  AIDs and channel information
    std::list<uint16_t>
        m_csStaIdList; //!< Store STA ID for all the stations that the beamformer requests CSI for
  private:
    Time m_lastCs; //! Time that last channel sounding was scheduled
};

class CsBeamformee : public ChannelSounding
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);
    CsBeamformee();
    virtual ~CsBeamformee();

    void ClearChannelInfo();
    void PrintChannelInfo();

    /**
     * Calculate channel information (random channel information is generated)
     */
    virtual void CalculateChannelInfo();

    /**
     * Get measured channel information at the station
     * \return channel information
     */
    ChannelInfo GetChannelInfo() const;

    /**
     * Get information in all subfields of NDPA frame at user side
     *
     * \param ndpa NDPA frame
     * \param staId station ID of the current station
     */
    void GetNdpaInfo(Ptr<const WifiMpdu> ndpa, uint16_t staId);

    /**
     * Get NDP frame information and calculate channel information at user side (random channel
     * information is used)
     *
     * \param txVector Tx vector of the NDP frame
     * \param staId STA ID of the station
     */
    virtual void GetNdpInfo(WifiTxVector txVector, uint16_t staId);

    /**
     * Generate beamforming report at user side
     * \param staId station ID of the given station
     * \param apAddress MAC address of AP
     * \param staAddress MAC address of the given station
     * \param bssid BSSID address
     */
    void GenerateBfReport(uint16_t staId,
                          Mac48Address apAddress,
                          Mac48Address staAddress,
                          Mac48Address bssid);
    /**
     * Get beamforming report
     *
     * \return beamforming report
     */
    Ptr<WifiMpdu> GetBfReport() const;

    /**
     * Get He MIMO Control Info field
     *
     * \return He MIMO Control Info field
     */
    HeMimoControlHeader GetHeMimoControlHeader() const;

    /**
     * Set whether NDPA frame is received
     *
     * \param flag whether NDPA frame is received
     */
    void SetNdpaReceived(bool flag);

    /**
     * Set whether NDP frame is received
     *
     * \param flag whether NDP frame is received
     */
    void SetNdpReceived(bool flag);

    /**
     * Check whether NDPA frame is received
     *
     * \return whether NDPA frame is received
     */
    bool IsNdpaReceived(void);

    /**
     * Check whether NDP frame is received
     *
     * \return whether NDP frame is received
     */
    bool IsNdpReceived(void);

  protected:
    void DoDispose() override;
    void DoInitialize() override;

  private:
    bool m_receiveNdpa;                        //! Whether NDPA frame is received
    bool m_receiveNdp;                         //! Whether NDP frame is received
    Ptr<WifiMpdu> m_bfReport;                  //!< Beamforming report frame to send
    HeMimoControlHeader m_heMimoControlHeader; //!< HE MIMO Control Info field used to transmit the
                                               //!< beamforming report
    ChannelInfo m_channelInfo;                 //!< Channel information measured by the beamformee
};

} // namespace ns3

#endif /* CHANNEL_SOUNDING_H */
