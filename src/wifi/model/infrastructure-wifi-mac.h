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
     * Enable or disable QoS support for the device.
     *
     * \param enable whether QoS is supported
     */
    void SetQosSupported(bool enable);
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
    /**
     * Function to indicate that the MPDU has been successfully sent.
     *
     * \param mpdu the MPDU to send
     */
    void virtual TxOk(Ptr<const WifiMpdu> mpdu) = 0;
    
    Ptr<const WifiMpdu> m_currentMpdu;    // MPDU to send
    


  protected:
    Ptr<WifiPsdu> m_currentPacket;  //!< Current packet transmitted/to be transmitted
    Ptr<Txop> m_currentTxop;        //!< Current TXOP
    WifiTxVector m_currentTxVector; //!< TXVECTOR used for the current packet transmission

    bool m_WiFiSensingSupported;    //!< WiFi Sensing supported
    bool m_muSensingSupported;      //!< WiFi Sensing in MU-OFDMA supported
    bool m_csSupported;             //!< Channel Sounding supported

  private:
    Time m_beaconInterval; //!< Expected interval between two beacon transmissions
    Time m_cfpMaxDuration; //!< CFP max duration

    Time m_lastNavStart;    //!< The time when the latest NAV started
    Time m_lastNavDuration; //!< The duration of the latest NAV

    Time m_cfpStart;          //!< The time when the latest CF period started
    Time m_lastBeacon;        //!< The time when the last beacon frame transmission started
    Time m_cfpForeshortening; //!< The delay the current CF period should be foreshortened

};


} // namespace ns3

#endif /* INFRASTRUCTURE_WIFI_MAC_H */
