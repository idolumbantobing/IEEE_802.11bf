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
 * Author: Sébastien Deronne <sebastien.deronne@gmail.com>
 */

#ifndef INFRASTRUCTURE_WIFI_MAC_H
#define INFRASTRUCTURE_WIFI_MAC_H

#include "block-ack-type.h"
#include "channel-access-manager.h"
#include "frame-exchange-manager.h"
#include "qos-txop.h"
#include "ssid.h"
#include "wifi-mac-trailer.h"
#include "wifi-mac.h"
#include "wifi-mpdu.h"
#include "wifi-ppdu.h"
#include "wifi-psdu.h"


namespace ns3
{

/**
 * \ingroup wifi
 *
 * The Wifi MAC high model for a STA or AP in a BSS.
 */
class InfrastructureWifiMac : public WifiMac
{
  public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId(void);

    InfrastructureWifiMac();
    virtual ~InfrastructureWifiMac();

    /**
     * \param packet the packet to send.
     * \param to the address to which the packet should be sent.
     *
     * The packet should be enqueued in a TX queue, and should be
     * dequeued as soon as the channel access function determines that
     * access is granted to this MAC.
     */
    virtual void Enqueue(Ptr<Packet> packet, Mac48Address to) = 0;
    /**
     * Enable or disable QoS support for the device.
     *
     * \param enable whether QoS is supported
     */
    void SetQosSupported(bool enable);

    /*
    *************************************
    Attempt to add PCF from ns3.33
    Public Functions and Attributes for Infratructure Wifi Mac
    *************************************
    */
    /**
     * Return a TXVECTOR for the Data frame given the destination.
     * The function consults WifiRemoteStationManager, which controls the rate
     * to different destinations.
     *
     * \param item the item being asked for TXVECTOR
     * \return TXVECTOR for the given item
     */
    WifiTxVector GetDataTxVector(Ptr<const WifiMpdu> item) const;
    /**
     * \param duration the maximum duration for the CF period.
     */
    void SetCfpMaxDuration(Time duration);
    /**
     * Return whether CTS-to-self capability is supported.
     *
     * \return true if CTS-to-self is supported, false otherwise
     */
    bool GetCtsToSelfSupported(void) const;
    /**
     * Set up WifiMac associated with this MacLow.
     *
     * \param mac WifiMac associated with this MacLow
     */
    void SetMac(const Ptr<WifiMac> mac);
    /**
     * This function indicates whether Simulator::Now is in the CF period.
     *
     * \return true if Simulator::Now is in CF period,
     *         false otherwise
     */
    bool IsCfPeriod(uint8_t linkId) const;
    /**
     * \return the maximum duration for the CF period.
     */
    Time GetCfpMaxDuration(void) const;
    /**
     * \return the remaining duration for the CF period.
     */
    Time GetRemainingCfpDuration(void) const;
    /**
     * A transmission that does not require an Ack has completed.
     */
    void EndTxNoAck(uint8_t linkId);
    /**
     * Enable or disable PCF support for the device.
     *
     * \param enable whether PCF is supported
     */
    void SetPcfSupported(bool enabled);
    /**
     * Return whether the device supports PCF.
     *
     * \return true if PCF is supported, false otherwise
     */
    bool GetPcfSupported() const;
    /**
     * Enable or disable PCF support for the device.
     *
     * \param enable whether PCF is supported
     */
    void SetInfo(Ptr<Txop> txop, Ptr<const WifiMpdu> mpdu);
    /**
     * Start CF period.
     *
     */
    void StartCfPeriod();
    /**
     * Start CF period.
     *
     */
    void StopCfPeriod();
    /**
     * Callback Function to set Txop for Transmission of CF-Poll Response
     *
     * \param txop the Txop for sending response to CF-Poll
     *
     */
    void SetTxop(Ptr<Txop> txop);
    void virtual TxOk(Ptr<const WifiMpdu> mpdu) = 0;

    Ptr<const WifiMpdu> m_currentMpdu; // MPDU to send

    /*
    *************************************
    Attempt to add Channel Sounding from ns3.37
    Public functions and attributes for Infrastructure Wifi Mac
    *************************************
    */

    bool m_csSupported; //!< Channel Sounding supported
    
    /*
    *************************************
    Attempt to add MU-OFDMA
    Public functions and attributes for Infratructure Wifi Mac
    *************************************
    */
    bool m_muMimoSupported; //!< MU-OFDMA supported
    bool m_muSensingSupported; //!< MU-OFDMA supported

  protected:
    /*
     *************************************
     Attempt to add PCF from ns3.33
     Protected Functions and Attributes for Infratructure Wifi Mac
     *************************************
    */

    Ptr<WifiPsdu> m_currentPacket;  //!< Current packet transmitted/to be transmitted
    Ptr<Txop> m_currentTxop;        //!< Current TXOP
    WifiTxVector m_currentTxVector; //!< TXVECTOR used for the current packet transmission

    bool m_WiFiSensingSupported;

  private:
    /**
     * This Boolean is set \c true iff this WifiMac support PCF
     */

    ns3::Ptr<ns3::WifiMac> m_mac;
    /*
     *************************************
     Attempt to add PCF from ns3.33
     Private Functions and Attributes for Infratructure Wifi Mac
     *************************************
    */

    Time m_beaconInterval; //!< Expected interval between two beacon transmissions
    Time m_cfpMaxDuration; //!< CFP max duration

    Time m_lastNavStart;    //!< The time when the latest NAV started
    Time m_lastNavDuration; //!< The duration of the latest NAV

    Time m_cfpStart;          //!< The time when the latest CF period started
    Time m_lastBeacon;        //!< The time when the last beacon frame transmission started
    Time m_cfpForeshortening; //!< The delay the current CF period should be foreshortened

    /**
     * Return a TXVECTOR for the RTS frame given the destination.
     * The function consults WifiRemoteStationManager, which controls the rate
     * to different destinations.
     *
     * \param item the item being asked for RTS TXVECTOR
     * \return TXVECTOR for the RTS of the given item
     */
    WifiTxVector GetRtsTxVector(Ptr<const WifiMpdu> item, uint8_t linkId) const;

};


} // namespace ns3

#endif /* INFRASTRUCTURE_WIFI_MAC_H */
