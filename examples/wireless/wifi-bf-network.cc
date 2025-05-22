/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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

#include "ns3/boolean.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/core-module.h"
#include "ns3/double.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/he-phy.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-flow-classifier.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mac48-address.h"
#include "ns3/mobility-helper.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/on-off-helper.h"
#include "ns3/packet-sink-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/propagation-loss-model.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/three-gpp-propagation-loss-model.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/wifi-module.h"
#include "ns3/yans-wifi-helper.h"

#include <cmath>
#include <random>

/* This is a simple example in order to show the frames exchanged in 802.11 PCF.
 * The output prints the overal throughput as well as the number of different PCF frames that have
 * been transmitted.
 *
 * It is possible to tune some parameters using the command line:
 *   - number of connected stations
 *   - enable/disable PCF
 *   - enable PCAP output file generation in order to vizualise frame exchange.
 *   - configure UDP data traffic:
 *     -> select traffic direction: --trafficDirection=<upstream|downstream>
 *
 * For example, one can observe the benefit of PCF over DCF when the number of stations increased:
 *   ./waf --run "wifi-pcf enablePcf=0 --nWifi=10" => DCF only
 *   ./waf --run "wifi-pcf enablePcf=1 --nWifi=10" => alternance of PCF and DCF
 *
 * One can also change the value of cfpMaxDuration: a shorter valer means the granted time for PCF
 * is shorter, and so it's benefit is reduced.
 *   ./waf --run "wifi-pcf enablePcf=1 --nWifi=10 --cfpMaxDuration=10240"
 *
 * One can also see the different types of piggybacked frames depending on the traffic direction and
 * whether PCF is enabled or not:
 *   ./waf --run "wifi-pcf enablePcf=0 --nWifi=1" => only CF_POLL and DATA_NULL frames should be
 * seen
 *   ./waf --run "wifi-pcf enablePcf=1 --nWifi=1 --trafficDirection=upstream" => no DATA_NULL frames
 * should be seen
 *   ./waf --run "wifi-pcf enablePcf=1 --nWifi=1 --trafficDirection=downstream" => no CF_END_ACK
 * frames should be seen
 */

/* This is a simple example in order to show the frames exchanged in sensing transmission of
 * 802.11bf.
 *
 * It is possible to tune some parameters using the command line:
 *   - number of connected stations
 *   - enable/disable Wi-Fi sensing
 *   - enable PCAP output file generation in order to vizualise frame exchange.
 *   - configure UDP data traffic:
 *     -> select traffic direction: --trafficDirection=<upstream|downstream>
 *
 * For example, to observe the effect number of stations increased:
 *   ./ns3 run "wifi-bf-network.cc enablePcf=0 --nWifi=10" => DCF only
 *   ./ns3 run "wifi-bf-network.cc --nWifi=10" => alternance of PCF and DCF
 *
 * One can also change the value of cfpMaxDuration: a shorter valer means the granted time for PCF
 * is shorter, and so it's benefit is reduced.
 *   ./ns3 --run "wifi-pcf enablePcf=1 --nWifi=10 --cfpMaxDuration=10240"
 *
 * Throughput for BSS 1: 0 Mbit/s
 * Throughput for BSS 2: 20.748 Mbit/s
 * # Signal (dbm): -24.7374
 * # Noise (dbm): -94.7009
 * # SNR (db): 69.9636
 * # tx beacons: 40
 * # tx CF-END: 26
 * # tx CF-END-ACK: 0
 * # tx CF-POLL: 3770
 * # tx DATA-NULL-ANNOUNCEMENT: 1884
 * # tx DATA-NULL: 1884
 * # tx CSI-BEAMFORMING-REPORT: 3768
 * # tx DATA: 3718
 * # tx CTS: 0
 * # tx CTS-to-self: 3768
 * # average Latency: 0.000945553
 * # inner Counter: 1884
 *
# total Latency: 1.78142
 */

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("bf-wifi-network");

uint64_t m_countBeacon;
uint64_t m_countCfPoll;
uint64_t m_countCfEnd;
uint64_t m_countCfEndAck;
uint64_t m_countDataNullAnnouncement;
uint64_t m_countDataNull;
uint64_t m_countData;
uint64_t m_countCTS;
uint64_t m_countCTStoSelf;
uint64_t m_csiBeamformingReport;
uint32_t nStations;
uint32_t nStations_net2;
Time m_sumDelay;
Time m_avgDelay;
Time m_sumTrueLatency;
Time m_avgTrueLatency;
Time m_sumTrueLatency_net2;
Time m_avgTrueLatency_net2;
bool enableCTStoSelf = true;
bool firstSensingPhase = true;
bool stillSensing = false;
bool multipleBss;
double nBss;
int nBfBss;
int nAxBss;
double m_innerCounter;
double m_collisionCounter;
double radius;
double numerator;
double denominator;
double ratio;
bool residentialDensity;
bool enableFrameAggregation;
std::vector<int> indoorOfficeApOrder_vec(12, 0);

// Global variables for use in callbacks.
double g_signalDbmAvg; //!< Average signal power [dBm]
double g_noiseDbmAvg;  //!< Average noise power [dBm]
uint32_t g_samples;    //!< Number of samples

typedef struct Bss
{
    uint32_t nStations_sensing;
    uint32_t nStations_no_sensing;
    uint32_t nAp = 1;
    NodeContainer wifiStaNode, wifiApNode, wifiStaNode_net2, WifiApNode_net2;
    NetDeviceContainer apDevice, apDevice_net2, staDevices, staDevices_net2;
    Ipv4AddressHelper address, address_net2;
    Ipv4InterfaceContainer ApInterface, ApInterface_net2, StaInterface, StaInterface_net2;
} Bss;

typedef struct Calculation
{
    Time m_sumTrueLatency;
    Time m_avgTrueLatency;
    double m_innerCounter;
    double m_collisionCounter;
    bool stillSensing;
} Calculation;

std::map<Mac48Address, Calculation> allCalculation;
std::map<Mac48Address, Calculation> allCalculation_net2;

void
MonitorSniffRx(Ptr<const Packet> packet,
               uint16_t channelFreqMhz,
               WifiTxVector txVector,
               MpduInfo aMpdu,
               SignalNoiseDbm signalNoise,
               uint16_t staId)

{
    g_samples++;
    g_signalDbmAvg += ((signalNoise.signal - g_signalDbmAvg) / g_samples);
    g_noiseDbmAvg += ((signalNoise.noise - g_noiseDbmAvg) / g_samples);
}

void
MonitorChannelAccess(Mac48Address addr, ns3::Time currentTime, bool AccessGranted)
{
    if (!AccessGranted)
    {
        if (allCalculation.find(addr) != allCalculation.end())
        {
            allCalculation.find(addr)->second.m_sumTrueLatency = currentTime;
            if (allCalculation.find(addr)->second.stillSensing)
            {
                m_collisionCounter++;
                allCalculation.find(addr)->second.m_collisionCounter++;
            }
            else
            {
                allCalculation.find(addr)->second.stillSensing = true;
                stillSensing = true;
            }
        }
        else if (allCalculation_net2.find(addr) != allCalculation_net2.end())
        {
            if (allCalculation_net2.find(addr)->second.stillSensing)
            {
                m_collisionCounter++;
                allCalculation_net2.find(addr)->second.m_collisionCounter++;
            }
            else
            {
                allCalculation_net2.find(addr)->second.m_sumTrueLatency = currentTime;
                allCalculation_net2.find(addr)->second.stillSensing = true;
            }
        }
    }
    else
    {
        if (allCalculation.find(addr) != allCalculation.end())
        {
            if (allCalculation.find(addr)->second.stillSensing)
            {
                allCalculation.find(addr)->second.m_avgTrueLatency =
                    allCalculation.find(addr)->second.m_avgTrueLatency +
                    (currentTime - allCalculation.find(addr)->second.m_sumTrueLatency);
                allCalculation.find(addr)->second.m_innerCounter++;
                allCalculation.find(addr)->second.stillSensing = false;
            }
        }
        else if (allCalculation_net2.find(addr) != allCalculation_net2.end())
        {
            if (allCalculation_net2.find(addr)->second.stillSensing)
            {
                allCalculation_net2.find(addr)->second.m_avgTrueLatency +=
                    (currentTime - allCalculation_net2.find(addr)->second.m_sumTrueLatency);
                allCalculation_net2.find(addr)->second.m_innerCounter++;
                allCalculation_net2.find(addr)->second.stillSensing = false;
            }
        }
    }
}

void
TxCallback(std::string context, Ptr<const Packet> p, double txPowerW)
{
    WifiMacHeader hdr;
    p->PeekHeader(hdr);
    hdr.GetAddr1();
    if (hdr.IsBeacon())
    {
        m_countBeacon++;
    }
    else if (hdr.IsCfPoll())
    {
        m_countCfPoll++;
    }
    else if (hdr.IsCfEnd())
    {
        if (hdr.IsCfAck())
        {
            m_countCfEndAck++;
        }
        else
        {
            m_countCfEnd++;
        }
    }
    else if (hdr.IsNdpa())
    {
        m_countDataNullAnnouncement++;
    }
    else if (hdr.IsActionNoAck())
    {
        m_csiBeamformingReport++;
    }
    else if (hdr.IsData())
    {
        if (!hdr.HasData())
        {
            m_countDataNull++;
        }
        else
        {
            m_countData++;
        }
    }
    else if (hdr.IsCts())
    {
        if (enableCTStoSelf)
        {
            m_countCTStoSelf++;
        }
        else
        {
            m_countCTS++;
        }
    }
}

void
RxEndCallback(Ptr<const Packet> packet)
{
    WifiMacHeader hdr;
    packet->PeekHeader(hdr);
    if (hdr.IsActionNoAck() && Simulator::Now() > Seconds(1))
    {
        m_csiBeamformingReport++;
        // if (multipleBss)
        // {
        //     // if (m_csiBeamformingReport % (nBfBss * nStations) == 0 && m_innerCounter < 1)
        //     // {
        //     //     m_avgTrueLatency = m_avgTrueLatency + (Simulator::Now() - m_sumTrueLatency);
        //     //     m_innerCounter++;
        //     //     // std::cout << "All reporting obtained in " << Simulator::Now() << std::endl;
        //     // }
        //     if (m_csiBeamformingReport % nStations == 0 && m_innerCounter < 1)
        //     {
        //         // std::cout << "All reporting obtained in " << Simulator::Now() << std::endl;
        //         m_avgTrueLatency = m_avgTrueLatency + (Simulator::Now() - m_sumTrueLatency);
        //         m_innerCounter++;
        //     }
        // }
        // else
        // {
        //     if (m_csiBeamformingReport % nStations == 0 && m_innerCounter < 1)
        //     {
        //         // std::cout << "All reporting obtained in " << Simulator::Now() << std::endl;
        //         m_avgTrueLatency = m_avgTrueLatency + (Simulator::Now() - m_sumTrueLatency);
        //         m_innerCounter++;
        //     }
        // }
    }
}

// Function to split a string by a delimiter and return a vector of substrings
std::vector<std::string>
split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

// Function to convert IP address string to an integer
uint32_t
ipToInt(const std::string& ip)
{
    std::vector<std::string> octets = split(ip, '.');
    if (octets.size() != 4)
    {
        throw std::invalid_argument("Invalid IP address format");
    }

    uint32_t ipInt = 0;
    for (size_t i = 0; i < 4; ++i)
    {
        int octet = std::stoi(octets[i]);
        if (octet < 0 || octet > 255)
        {
            throw std::out_of_range("IP address octet out of range");
        }
        ipInt |= (octet << ((3 - i) * 8));
    }
    return ipInt;
}

double
getRandomCoordinate(double min, double max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

Ptr<ListPositionAllocator>
setLocationScenario(int scenario,
                    Ptr<ListPositionAllocator> positionAlloc,
                    std::vector<Bss> allBss,
                    double radius)
{
    if (scenario == 1)
    {
        double x_baseAp = 0.0;
        double y_baseAp = 0.0;
        double baseAngle = 0.0;
        double rasioAngle = 0.0;

        baseAngle = 2.0 * M_PI / nBss;
        if (multipleBss)
        {
            rasioAngle = 1 / (sin(baseAngle / 2));
            rasioAngle = 3;
        }
        std::cout << "rasioAngle: " << rasioAngle << std::endl;
        for (int i = 0; i < nBss; i++)
        {
            x_baseAp = (rasioAngle * radius) * cos(baseAngle * i);
            y_baseAp = (rasioAngle * radius) * sin(baseAngle * i);
            if (i < nBfBss)
            {
                std::cout << "(bf)AP: " << x_baseAp << ", " << y_baseAp << std::endl;
            }
            else
            {
                std::cout << "(ax)AP: " << x_baseAp << ", " << y_baseAp << std::endl;
            }
            positionAlloc->Add(Vector(x_baseAp, y_baseAp, 0.0));
            if (i < nBfBss)
            {
                double Angle = 2 * M_PI / allBss[i].nStations_sensing;
                for (uint32_t j = 0; j < allBss[i].nStations_sensing; j++)
                {
                    double x = radius * cos(Angle * j) + x_baseAp;
                    double y = radius * sin(Angle * j) + y_baseAp;
                    std::cout << "(bf)STA: " << x << ", " << y << std::endl;
                    positionAlloc->Add(Vector(x, y, 0.0));
                }
            }
            else
            {
                double Angle = 2 * M_PI / allBss[i].nStations_no_sensing;
                for (uint32_t j = 0; j < allBss[i].nStations_no_sensing; j++)
                {
                    double x = radius * cos(Angle * j) + x_baseAp;
                    double y = radius * sin(Angle * j) + y_baseAp;
                    std::cout << "(ax)STA: " << x << ", " << y << std::endl;
                    positionAlloc->Add(Vector(x, y, 0.0));
                }
            }
        }
    }
    else if (scenario == 2)
    {
        double areaSize = 5.0;
        // The lower row
        for (int i = 0; i < 6; i++)
        {
            double x = (10.0 + (i * 20.0));
            double y = 15.0;
            // AP placing based on the 3GPP Indoor Office
            positionAlloc->Add(Vector(x, y, 3.0));
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                std::cout << "(bf)AP: " << x << ", " << y << std::endl;
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                std::cout << "(ax)AP: " << x << ", " << y << std::endl;
            }

            // Uniform distribution of STA around the AP
            for (int j = 0; j < nStations; j++)
            {
                x += getRandomCoordinate(-areaSize, areaSize);
                y += getRandomCoordinate(-areaSize, areaSize);
                positionAlloc->Add(Vector(x, y, 1.0));
                if (indoorOfficeApOrder_vec[i] == 1)
                {
                    std::cout << "(bf)STA: " << x << ", " << y << std::endl;
                }
                else if (indoorOfficeApOrder_vec[i] == 0)
                {
                    std::cout << "(ax)STA: " << x << ", " << y << std::endl;
                }
            }
        }

        // The higher row
        for (int i = 0; i < 6; i++)
        {
            double x = (10.0 + (i * 20.0));
            double y = 35.0;
            // AP placing based on the 3GPP Indoor Office
            positionAlloc->Add(Vector(x, y, 3.0));
            if (indoorOfficeApOrder_vec[i + 6] == 1)
            {
                std::cout << "(bf)AP: " << x << ", " << y << std::endl;
            }
            else if (indoorOfficeApOrder_vec[i + 6] == 0)
            {
                std::cout << "(ax)AP: " << x << ", " << y << std::endl;
            }

            // Uniform distribution of STA around the AP
            for (int j = 0; j < nStations; j++)
            {
                x += getRandomCoordinate(-areaSize, areaSize);
                y += getRandomCoordinate(-areaSize, areaSize);
                positionAlloc->Add(Vector(x, y, 1.0));
                if (indoorOfficeApOrder_vec[i + 6] == 1)
                {
                    std::cout << "(bf)STA: " << x << ", " << y << std::endl;
                }
                else if (indoorOfficeApOrder_vec[i + 6] == 0)
                {
                    std::cout << "(ax)STA: " << x << ", " << y << std::endl;
                }
            }
        }
    }
    else
    {
        double x_baseAp = 0.0;
        double y_baseAp = 0.0;
        double baseAngle = 0.0;
        double rasioAngle = 0.0;

        baseAngle = 2.0 * M_PI / nBss;
        if (multipleBss)
        {
            rasioAngle = 1 / (sin(baseAngle / 2));
            rasioAngle = 3;
        }
        std::cout << "rasioAngle: " << rasioAngle << std::endl;
        for (int i = 0; i < nBss; i++)
        {
            x_baseAp = (rasioAngle * radius) * cos(baseAngle * i);
            y_baseAp = (rasioAngle * radius) * sin(baseAngle * i);
            if (i < nBfBss)
            {
                std::cout << "(bf)AP: " << x_baseAp << ", " << y_baseAp << std::endl;
            }
            else
            {
                std::cout << "(ax)AP: " << x_baseAp << ", " << y_baseAp << std::endl;
            }
            positionAlloc->Add(Vector(x_baseAp, y_baseAp, 0.0));
            if (i < nBfBss)
            {
                double Angle = 2 * M_PI / allBss[i].nStations_sensing;
                for (uint32_t j = 0; j < allBss[i].nStations_sensing; j++)
                {
                    double x = radius * cos(Angle * j) + x_baseAp;
                    double y = radius * sin(Angle * j) + y_baseAp;
                    std::cout << "(bf)STA: " << x << ", " << y << std::endl;
                    positionAlloc->Add(Vector(x, y, 0.0));
                }
            }
            else
            {
                double Angle = 2 * M_PI / allBss[i].nStations_no_sensing;
                for (uint32_t j = 0; j < allBss[i].nStations_no_sensing; j++)
                {
                    double x = radius * cos(Angle * j) + x_baseAp;
                    double y = radius * sin(Angle * j) + y_baseAp;
                    std::cout << "(ax)STA: " << x << ", " << y << std::endl;
                    positionAlloc->Add(Vector(x, y, 0.0));
                }
            }
        }
    }

    return positionAlloc;
}

std::vector<Bss>
setNumberDevice(int scenario)
{
    if (scenario == 1)
    {
        if (nBss == 1 && nAxBss == 0)
        {
            multipleBss = false;
            nBfBss = 1;
        }
        else if (nBss == 1 && nAxBss == 1)
        {
            multipleBss = false;
            nBfBss = 0;
        }
        else
        {
            multipleBss = true;
            denominator = nBss;
            ratio = numerator / denominator;
            if (ratio == 0.0)
            {
                nBfBss = int(nBss) - nAxBss;
            }
            else
            {
                nAxBss = int(nBss * ratio);
                nBfBss = nBss - nAxBss;
            }
        }

        Bss default_Bss;
        default_Bss.nStations_sensing = nStations;
        default_Bss.nStations_no_sensing = nStations_net2;
        std::vector<Bss> allBss(nBss, default_Bss);

        for (int i = 0; i < nBfBss; i++)
        {
            char address_tempconst[18];
            std::snprintf(address_tempconst,
                          sizeof(address_tempconst),
                          "00:00:00:00:00:%02x",
                          i + 1);
            Mac48Address address = Mac48Address(address_tempconst);
            Calculation calc;
            calc.m_avgTrueLatency = Seconds(0);
            calc.m_sumTrueLatency = Seconds(0);
            calc.m_innerCounter = 0;
            calc.m_collisionCounter = 0;
            calc.stillSensing = false;
            allCalculation.insert({address, calc});
        }

        int j = 0;
        for (int i = nBfBss; i < nBss; i++)
        // for (int i = nBfBss; i < nBfBss+1; i++)
        {
            char address_tempconst[18];
            j = i + nStations * nBfBss;
            std::snprintf(address_tempconst,
                          sizeof(address_tempconst),
                          "00:00:00:00:00:%02x",
                          j + 1);
            Mac48Address address = Mac48Address(address_tempconst);
            Calculation calc;
            calc.m_avgTrueLatency = Seconds(0);
            calc.m_sumTrueLatency = Seconds(0);
            calc.m_innerCounter = 0;
            calc.m_collisionCounter = 0;
            calc.stillSensing = false;
            allCalculation_net2.insert({address, calc});
        }

        for (int i = 0; i < nBfBss; i++)
        {
            NodeContainer wifiStaNode, wifiApNode;
            wifiStaNode.Create(allBss[i].nStations_sensing);
            wifiApNode.Create(allBss[i].nAp);
            std::cout << i + 1 << ". BSS(bf) " << "has " << allBss[i].nStations_sensing
                      << " stations " << "and " << allBss[i].nAp << " AP" << std::endl;
            allBss[i].wifiStaNode = wifiStaNode;
            allBss[i].wifiApNode = wifiApNode;
        }
        for (int i = nBfBss; i < nBss; i++)
        {
            NodeContainer wifiStaNode_net2, WifiApNode_net2;
            wifiStaNode_net2.Create(allBss[i].nStations_no_sensing);
            WifiApNode_net2.Create(allBss[i].nAp);
            std::cout << i + 1 << ". BSS(ax) " << "has " << allBss[i].nStations_no_sensing
                      << " stations " << "and " << allBss[i].nAp << " AP" << std::endl;
            allBss[i].wifiStaNode_net2 = wifiStaNode_net2;
            allBss[i].WifiApNode_net2 = WifiApNode_net2;
        }

        return allBss;
    }
    else if (scenario == 2)
    {
        nBss = 40;
        nBfBss = 2;
        nAxBss = nBss - nBfBss;
        Bss default_Bss;
        default_Bss.nStations_sensing = 1;
        default_Bss.nStations_no_sensing = 1;
        std::vector<Bss> allBss_sce2(nBss, default_Bss);
        for (int i = 0; i < nBfBss; i++)
        {
            char address_tempconst[18];
            std::snprintf(address_tempconst,
                          sizeof(address_tempconst),
                          "00:00:00:00:00:%02x",
                          i + 1);
            Mac48Address address = Mac48Address(address_tempconst);
            Calculation calc;
            calc.m_avgTrueLatency = Seconds(0);
            calc.m_sumTrueLatency = Seconds(0);
            calc.m_innerCounter = 0;
            calc.m_collisionCounter = 0;
            calc.stillSensing = false;
            allCalculation.insert({address, calc});
        }

        for (int i = nBfBss; i < nBss; i++)
        {
            char address_tempconst[18];
            std::snprintf(address_tempconst,
                          sizeof(address_tempconst),
                          "00:00:00:00:00:%02x",
                          i + 1 + (i * nStations_net2));
            Mac48Address address = Mac48Address(address_tempconst);
            Calculation calc;
            calc.m_avgTrueLatency = Seconds(0);
            calc.m_sumTrueLatency = Seconds(0);
            calc.m_innerCounter = 0;
            calc.m_collisionCounter = 0;
            calc.stillSensing = false;
            allCalculation_net2.insert({address, calc});
        }

        for (int i = 0; i < nBfBss; i++)
        {
            NodeContainer wifiStaNode, wifiApNode;
            wifiStaNode.Create(allBss_sce2[i].nStations_sensing);
            wifiApNode.Create(allBss_sce2[i].nAp);
            std::cout << i + 1 << ". BSS(bf) " << "has " << allBss_sce2[i].nStations_sensing
                      << " stations " << "and " << allBss_sce2[i].nAp << " AP" << std::endl;
            allBss_sce2[i].wifiStaNode = wifiStaNode;
            allBss_sce2[i].wifiApNode = wifiApNode;
        }
        int j = 0;
        for (int i = nBfBss; i < nBss; i++)
        {
            char address_tempconst[18];
            j = i + nStations * nBfBss;
            std::snprintf(address_tempconst,
                          sizeof(address_tempconst),
                          "00:00:00:00:00:%02x",
                          j + 1);
            Mac48Address address = Mac48Address(address_tempconst);
            Calculation calc;
            calc.m_avgTrueLatency = Seconds(0);
            calc.m_sumTrueLatency = Seconds(0);
            calc.m_innerCounter = 0;
            calc.m_collisionCounter = 0;
            calc.stillSensing = false;
            allCalculation_net2.insert({address, calc});
        }
        return allBss_sce2;
    }
    else if (scenario == 2)
    {
        multipleBss = true;
        nBss = 12;
        if (nBfBss > nBss)
        {
            std::cout << "Invalid number of IEEE 802.11bf BSS" << std::endl;
        }
        nAxBss = nBss - nBfBss;

        // Fill the indoorOfficeApOrder_vec with 1s representing 802.11bf
        for (int i = 0; i < nBfBss; ++i)
        {
            indoorOfficeApOrder_vec[i] = 1;
        }

        // Randomize the order of the vector
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(indoorOfficeApOrder_vec.begin(), indoorOfficeApOrder_vec.end(), g);

        Bss default_Bss;
        default_Bss.nStations_sensing = nStations;
        default_Bss.nStations_no_sensing = nStations;
        std::vector<Bss> allBss(nBss, default_Bss);

        for (int i = 0; i < nBss; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                char address_tempconst[18];
                std::snprintf(address_tempconst,
                              sizeof(address_tempconst),
                              "00:00:00:00:00:%02x",
                              i + 1 + (i * nStations));
                Mac48Address address = Mac48Address(address_tempconst);
                Calculation calc;
                calc.m_avgTrueLatency = Seconds(0);
                calc.m_sumTrueLatency = Seconds(0);
                calc.m_innerCounter = 0;
                calc.m_collisionCounter = 0;
                calc.stillSensing = false;
                allCalculation.insert({address, calc});
            }
        }

        for (int i = 0; i < nBss; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                NodeContainer wifiStaNode, wifiApNode;
                wifiStaNode.Create(allBss[i].nStations_sensing);
                wifiApNode.Create(allBss[i].nAp);
                std::cout << i + 1 << ". BSS(bf) " << "has " << allBss[i].nStations_sensing
                          << " stations " << "and " << allBss[i].nAp << " AP" << std::endl;
                allBss[i].wifiStaNode = wifiStaNode;
                allBss[i].wifiApNode = wifiApNode;
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                NodeContainer wifiStaNode_net2, WifiApNode_net2;
                wifiStaNode_net2.Create(allBss[i].nStations_no_sensing);
                WifiApNode_net2.Create(allBss[i].nAp);
                std::cout << i + 1 << ". BSS(ax) " << "has " << allBss[i].nStations_no_sensing
                          << " stations " << "and " << allBss[i].nAp << " AP" << std::endl;
                allBss[i].wifiStaNode_net2 = wifiStaNode_net2;
                allBss[i].WifiApNode_net2 = WifiApNode_net2;
            }
        }

        return allBss;
    }
}

int
main(int argc, char* argv[])
{
    Time::SetResolution(Time::NS);
    /***********************************/
    // Parameter for PCF               //
    /***********************************/
    nBss = 1;
    nAxBss = 0;
    nStations = 1;
    nStations_net2 = 1;
    bool enablePcap = false;
    bool enableWiFiSensing = true;
    multipleBss = true;
    bool downlink{true};
    uint64_t cfpMaxDurationMs = 50;   // milliseconds
    uint64_t sensingInterval = 50;    // milliseconds
    uint64_t sensingIntervalType = 0; // 0: Constant, 1: Poisson-distributed
    double simulationTime = 5.0;      // seconds
    radius = 2.0;                     // meters
    numerator = 0.0;                  // numerator for the ratio in multiple BSS scenario
    denominator = nBss;               // denominator for the ratio in multiple BSS scenario
    ratio = numerator / denominator;  // ratio of number of bf to ax BSS in multiple BSS scenario
    bool udp = true;
    uint16_t sensingPriority = 0; // Priority for sensing, AC_BE by default
    int scenario = 1;
    residentialDensity = 1;
    enableFrameAggregation = true;
    int trafficType = 0; // 0: Constant Bit Rate, 1: Poisson Traffic

    /***********************************/
    // Parameter for Channel Sounding  //
    /***********************************/
    bool enableChannelSounding = true;    // Enable/disable channel sounding
    int channelWidth{20};                 // Bandwidth
    Time channelSoundingInterval{"5ms"};  // Channel sounding interval (Channel sounding is disabled
                                          // if the interval is set as "0")
    uint32_t payloadSize = 700;           // Payload size
    uint8_t ngSu = 4;                     // Subcarrier grouping Ng for SU channel sounding
    uint8_t ngMu = 4;                     // Subcarrier grouping Ng for MU channel sounding
    std::string codebookSizeSu = "(6,4)"; // Codebook size for SU channel sounding
    std::string codebookSizeMu = "(9,7)"; // Codebook size for MU channel sounding
    uint8_t nc = 2;           // Number of colums in the compressed beamforming feedback matrix
    uint8_t SoundingType = 0; // Sounding type (0: SU, 1: SU+MU, 2: MU)

    /*******************************************/
    // MU-OFDMA Setup in Physical Layer        //
    /*******************************************/
    bool enableWiFiMuSensing = 1;
    bool enableUlOfdma = 1;
    uint8_t maxNumDlMuMimoSta =
        1; // Maximum number of stations in downlink MU-MIMO data transmission
    std::string csMode =
        "HeMcs0"; // Wifi mode used for beamforming report feedback (If set as "0", wifi mode is
                  // automatically selected as the same mode as in data transmission)
    double frequency{5}; // whether 2.4, 5 or 6 GHz
    int mcs{6}; // MCS used for both beamforming report feedback in channel sounding and data
                // transmission (-1 indicates an unset value)

    /*******************************************/
    // Seedn Settings for Randomizer           //
    /*******************************************/
    int iseed = 12452;

    /*******************************************/
    // Parameter tuning with CommandLine        //
    /*******************************************/
    CommandLine cmd(__FILE__);
    cmd.AddValue("nBss", "Number of BSS", nBss);
    cmd.AddValue("nStations", "Number of wifi STA devices", nStations);
    cmd.AddValue("nStations_net2", "Number of wifi STA devices for the second BSS", nStations_net2);
    cmd.AddValue("enableWiFiSensing", "Enable/disable WiFi Sensing mode", enableWiFiSensing);
    cmd.AddValue("downlink",
                 "Data traffic direction for IEEE 802.11ax BSS: upstream (all STAs -> AP) or "
                 "downstream (AP -> all STAs)",
                 downlink);
    cmd.AddValue("cfpMaxDuration", "CFP max duration in microseconds", cfpMaxDurationMs);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("enablePcap", "Enable/disable PCAP output", enablePcap);
    cmd.AddValue("channelWidth", "Channel bandwidth", channelWidth);
    cmd.AddValue(
        "channelSoundingInterval",
        "Channel sounding interval (Channel sounding is disabled if the interval is set as 0)",
        channelSoundingInterval);
    cmd.AddValue("payloadSize", "The application payload size in bytes", payloadSize);
    cmd.AddValue("ngSu", "Subcarrier grouping Ng for SU channel sounding", ngSu);
    cmd.AddValue("ngMu", "Subcarrier grouping Ng for MU channel sounding", ngMu);
    cmd.AddValue("codebookSizeSu", "Codebook size for SU channel sounding", codebookSizeSu);
    cmd.AddValue("codebookSizeMu", "Codebook size for MU channel sounding", codebookSizeMu);
    cmd.AddValue("nc", "Number of colums in the compressed beamforming feedback matrix", nc);
    cmd.AddValue("maxNumDlMuMimoSta",
                 "Maximum number of stations in downlink MU-MIMO data transmission",
                 maxNumDlMuMimoSta);
    cmd.AddValue("mcs", "If set, limit testing to a specific MCS (0-11)", mcs);
    cmd.AddValue("enablePcap", "Whether to output PCAP file", enablePcap);
    cmd.AddValue("csMode",
                 "Wifi mode used for beamforming report feedback (If set as '0', wifi mode is "
                 "automatically selected as the same mode as in data transmission)",
                 csMode);
    cmd.AddValue("seed", "Seed for random number generator", iseed);
    cmd.AddValue("radius", "Radius of the circle for the model distance", radius);
    cmd.AddValue("soundingtype", "Sounding type (0: SU, 1: SU+MU, 2: MU)", SoundingType);
    cmd.AddValue("frequency", "Frequency (2.4, 5, 6 GHz)", frequency);
    cmd.AddValue("nBss", "Number of BSS", nBss);
    cmd.AddValue("nAxBss", "Number of BSS for ax", nAxBss);
    cmd.AddValue("nBfBss", "Number of BSS for bf", nBfBss);
    cmd.AddValue("enableMuMIMO", "Enable/disable MU-MIMO", enableWiFiMuSensing);
    cmd.AddValue("numerator", "Numerator for the ratio in multiple BSS scenario", numerator);
    cmd.AddValue("udp", "Enable/disable UDP traffic", udp);
    cmd.AddValue("sensingPriority", "Priority for sensing", sensingPriority);
    cmd.AddValue("scenario", "Scenario for the location of the APs and STAs", scenario);
    cmd.AddValue("residentialDensity",
                 "Density scenario for the residential layout",
                 residentialDensity);
    cmd.AddValue("sensingInterval", "Rate for sensing procedure", sensingInterval);
    cmd.AddValue("sensingIntervalType", "Type of sensing interval", sensingIntervalType);
    cmd.AddValue("enableFrameAggregation",
                 "Enable/disable frame aggregation",
                 enableFrameAggregation);
    cmd.AddValue("trafficType",
                 "Traffic type of communication Wi-Fi protocol (0: Constant, 1: Poisson)",
                 trafficType);
    cmd.Parse(argc, argv);

    RngSeedManager::SetSeed(iseed);
    RngSeedManager::SetRun(10);

    m_countBeacon = 0;
    m_countCfEnd = 0;
    m_countCfEndAck = 0;
    m_countCfPoll = 0;
    m_countDataNullAnnouncement = 0;
    m_countDataNull = 0;
    m_countData = 0;
    m_countCTS = 0;
    m_countCTStoSelf = 0;
    m_innerCounter = 0;
    m_sumDelay = Seconds(0);
    m_avgDelay = Seconds(0);
    m_sumTrueLatency = Seconds(0);
    m_avgTrueLatency = Seconds(0);

    std::vector<Bss> allBss = setNumberDevice(scenario);

    WifiHelper wifi;
    WifiMacHelper macAp, macSta;

    std::string channelStr("{0, " + std::to_string(channelWidth) + ", ");
    StringValue ctrlRate;
    auto nonHtRefRateMbps = HePhy::GetNonHtReferenceRate(mcs) / 1e6;

    std::ostringstream ossDataMode;
    ossDataMode << "HeMcs" << mcs;

    if (frequency == 6)
    {
        wifi.SetStandard(WIFI_STANDARD_80211bf);
        ctrlRate = StringValue(ossDataMode.str());
        channelStr += "BAND_6GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue(48));
    }
    else if (frequency == 5)
    {
        wifi.SetStandard(WIFI_STANDARD_80211bf);
        std::ostringstream ossControlMode;
        ossControlMode << "OfdmRate" << nonHtRefRateMbps << "Mbps";
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_5GHZ, 0}";
    }
    else if (frequency == 2.4)
    {
        wifi.SetStandard(WIFI_STANDARD_80211bf);
        std::ostringstream ossControlMode;
        ossControlMode << "ErpOfdmRate" << nonHtRefRateMbps << "Mbps";
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_2_4GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue(40));
    }
    else
    {
        std::cout << "Wrong frequency value!" << std::endl;
        return 0;
    }

    // Set the default Acknowledgment mode to NONE
    Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                       EnumValue(WifiAcknowledgment::NONE));

    Ptr<MultiModelSpectrumChannel> spectrumChannel = CreateObject<MultiModelSpectrumChannel>();

    if (scenario == 2)
    {
        Ptr<ThreeGppIndoorOfficePropagationLossModel> ThreeGppLossModel =
            CreateObject<ThreeGppIndoorOfficePropagationLossModel>();
        ThreeGppLossModel->SetFrequency(5.180e9);
        spectrumChannel->AddPropagationLossModel(ThreeGppLossModel);
    }
    else
    {
        Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel>();
        lossModel->SetFrequency(5.180e9);
        spectrumChannel->AddPropagationLossModel(lossModel);
    }

    Ptr<ConstantSpeedPropagationDelayModel> delayModel =
        CreateObject<ConstantSpeedPropagationDelayModel>();
    spectrumChannel->SetPropagationDelayModel(delayModel);

    SpectrumWifiPhyHelper phy;
    phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    phy.SetChannel(spectrumChannel);
    // phy.SetErrorRateModel("ns3::TableBasedErrorRateModel");
    phy.Set("ChannelSettings", StringValue(channelStr));
    phy.Set("TxPowerStart", DoubleValue(23.0));
    phy.Set("TxPowerEnd", DoubleValue(23.0));
    phy.Set("TxPowerLevels", UintegerValue(1));

    // YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    // YansWifiPhyHelper phy;
    // phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    // phy.SetChannel(channel.Create());
    // phy.Set("ChannelSettings", StringValue(channelStr));
    // phy.Set("TxPowerStart", DoubleValue(23.0));
    // phy.Set("TxPowerEnd", DoubleValue(23.0));
    // phy.Set("TxPowerLevels", UintegerValue(1));

    Ssid ssid = Ssid("wifi-bf-network");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(ossDataMode.str()),
                                 "ControlMode",
                                 ctrlRate);
    // wifi.SetRemoteStationManager("ns3::MinstrelWifiManager");
    wifi.ConfigHeOptions("NgSu",
                         UintegerValue(ngSu),
                         "NgMu",
                         UintegerValue(ngMu),
                         "CodebookSizeSu",
                         StringValue(codebookSizeSu),
                         "CodebookSizeMu",
                         StringValue(codebookSizeMu),
                         "MaxNc",
                         UintegerValue(nc - 1));

    if (scenario == 2)
    {
        for (int i = 0; i < 12; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                macAp.SetType("ns3::ApWifiMac",
                              "Ssid",
                              SsidValue(ssid),
                              "WiFiSensingSupported",
                              BooleanValue(enableWiFiSensing),
                              "CfpMaxDuration",
                              TimeValue(MicroSeconds(cfpMaxDurationMs * 1024)),
                              "CtsToSelfSupported",
                              BooleanValue(enableCTStoSelf),
                              "ChannelSoundingSupported",
                              BooleanValue(enableChannelSounding),
                              "QosSupported",
                              BooleanValue(true),
                              "BeaconGeneration",
                              BooleanValue(true),
                              "SensingPriority",
                              UintegerValue(sensingPriority),
                              "SensingInterval",
                              TimeValue(MilliSeconds(sensingInterval)),
                              "SensingIntervalType",
                              UintegerValue(sensingIntervalType));

                macAp.SetMultiUserScheduler("ns3::RrMultiUserScheduler",
                                            "ChannelSoundingInterval",
                                            TimeValue(channelSoundingInterval),
                                            "EnableUlOfdma",
                                            BooleanValue(enableUlOfdma),
                                            "EnableMuMimo",
                                            BooleanValue(enableWiFiMuSensing),
                                            "MaxNumDlMuMimoSta",
                                            UintegerValue(maxNumDlMuMimoSta),
                                            "NStations",
                                            UintegerValue(nStations),
                                            "SoundingType",
                                            UintegerValue(SoundingType));
                allBss[i].apDevice = wifi.Install(phy, macAp, allBss[i].wifiApNode);
                macSta.SetType("ns3::StaWifiMac",
                               "Ssid",
                               SsidValue(ssid),
                               "ActiveProbing",
                               BooleanValue(false),
                               "WiFiSensingSupported",
                               BooleanValue(enableWiFiSensing),
                               "CfpMaxDuration",
                               TimeValue(MicroSeconds(cfpMaxDurationMs * 1024)),
                               "CtsToSelfSupported",
                               BooleanValue(enableCTStoSelf),
                               "QosSupported",
                               BooleanValue(true),
                               "ManualConnection",
                               BooleanValue(true));
                allBss[i].staDevices = wifi.Install(phy, macSta, allBss[i].wifiStaNode);
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                Time accessReqInterval{0};
                WifiMacHelper macAp_net2, macSta_net2;
                WifiHelper wifi;
                Ssid ssid = Ssid("wifi-ax-network");
                wifi.SetStandard(WIFI_STANDARD_80211ax);
                std::ostringstream ossControlMode;
                ossControlMode << "OfdmRate" << nonHtRefRateMbps << "Mbps";
                ctrlRate = StringValue(ossControlMode.str());
                channelStr += "BAND_5GHZ, 0}";
                wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                             "DataMode",
                                             StringValue(ossDataMode.str()),
                                             "ControlMode",
                                             ctrlRate);
                wifi.ConfigHeOptions("GuardInterval", TimeValue(NanoSeconds(3200)));

                macAp_net2.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

                if (!enableFrameAggregation)
                {
                    macAp_net2.SetType(
                        "ns3::ApWifiMac",
                        "BE_MaxAmpduSize",
                        UintegerValue(0), // Enable A-MPDU with the highest maximum size allowed by
                                          // the standard
                        "BE_MaxAmsduSize",
                        UintegerValue(
                            0)); // Enable A-MSDU with the highest maximum size (in Bytes) allowed
                }

                allBss[i].apDevice_net2 = wifi.Install(phy, macAp_net2, allBss[i].WifiApNode_net2);

                macSta_net2.SetType("ns3::StaWifiMac",
                                    "Ssid",
                                    SsidValue(ssid),
                                    "ManualConnection",
                                    BooleanValue(true));

                if (!enableFrameAggregation)
                {
                    macSta_net2.SetType(
                        "ns3::StaWifiMac",
                        "BE_MaxAmpduSize",
                        UintegerValue(0), // Enable A-MPDU with the highest maximum size allowed by
                                          // the standard
                        "BE_MaxAmsduSize",
                        UintegerValue(
                            0)); // Enable A-MSDU with the highest maximum size (in Bytes) allowed
                }

                allBss[i].staDevices_net2 =
                    wifi.Install(phy, macSta_net2, allBss[i].wifiStaNode_net2);
            }
        }
    }
    else
    {
        if (nBfBss >= 0)
        {
            macAp.SetType("ns3::ApWifiMac",
                          "Ssid",
                          SsidValue(ssid),
                          "WiFiSensingSupported",
                          BooleanValue(enableWiFiSensing),
                          "CfpMaxDuration",
                          TimeValue(MicroSeconds(cfpMaxDurationMs * 1024)),
                          "CtsToSelfSupported",
                          BooleanValue(enableCTStoSelf),
                          "ChannelSoundingSupported",
                          BooleanValue(enableChannelSounding),
                          "QosSupported",
                          BooleanValue(true),
                          "BeaconGeneration",
                          BooleanValue(true),
                          "SensingPriority",
                          UintegerValue(sensingPriority),
                          "SensingInterval",
                          TimeValue(MilliSeconds(sensingInterval)),
                          "SensingIntervalType",
                          UintegerValue(sensingIntervalType));

            macAp.SetMultiUserScheduler("ns3::RrMultiUserScheduler",
                                        "ChannelSoundingInterval",
                                        TimeValue(channelSoundingInterval),
                                        "EnableUlOfdma",
                                        BooleanValue(enableUlOfdma),
                                        "EnableMuMimo",
                                        BooleanValue(enableWiFiMuSensing),
                                        "MaxNumDlMuMimoSta",
                                        UintegerValue(maxNumDlMuMimoSta),
                                        "NStations",
                                        UintegerValue(nStations),
                                        "SoundingType",
                                        UintegerValue(SoundingType));
            for (int i = 0; i < nBfBss; i++)
            {
                allBss[i].apDevice = wifi.Install(phy, macAp, allBss[i].wifiApNode);
            }
            macSta.SetType("ns3::StaWifiMac",
                           "Ssid",
                           SsidValue(ssid),
                           "ActiveProbing",
                           BooleanValue(false),
                           "WiFiSensingSupported",
                           BooleanValue(enableWiFiSensing),
                           "CfpMaxDuration",
                           TimeValue(MicroSeconds(cfpMaxDurationMs * 1024)),
                           "CtsToSelfSupported",
                           BooleanValue(enableCTStoSelf),
                           "QosSupported",
                           BooleanValue(true),
                           "ManualConnection",
                           BooleanValue(true));
            for (int i = 0; i < nBfBss; i++)
            {
                allBss[i].staDevices = wifi.Install(phy, macSta, allBss[i].wifiStaNode);
            }
        }

        Time accessReqInterval{0};
        WifiMacHelper macAp_net2, macSta_net2;
        WifiHelper wifi;
        Ssid ssid = Ssid("wifi-ax-network");
        wifi.SetStandard(WIFI_STANDARD_80211ax);
        std::ostringstream ossControlMode;
        ossControlMode << "OfdmRate" << nonHtRefRateMbps << "Mbps";
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_5GHZ, 0}";
        wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                     "DataMode",
                                     StringValue(ossDataMode.str()),
                                     "ControlMode",
                                     ctrlRate);
        wifi.ConfigHeOptions("GuardInterval", TimeValue(NanoSeconds(3200)));

        macAp_net2.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));

        if (!enableFrameAggregation)
        {
            macAp_net2.SetType(
                "ns3::ApWifiMac",
                "BE_MaxAmpduSize",
                UintegerValue(
                    0), // Enable A-MPDU with the highest maximum size allowed by the standard
                "BE_MaxAmsduSize",
                UintegerValue(0)); // Enable A-MSDU with the highest maximum size (in Bytes) allowed
        }

        for (int i = nBfBss; i < nBss; i++)
        {
            allBss[i].apDevice_net2 = wifi.Install(phy, macAp_net2, allBss[i].WifiApNode_net2);
        }

        macSta_net2.SetType("ns3::StaWifiMac",
                            "Ssid",
                            SsidValue(ssid),
                            "ManualConnection",
                            BooleanValue(true));

        if (!enableFrameAggregation)
        {
            macSta_net2.SetType(
                "ns3::StaWifiMac",
                "BE_MaxAmpduSize",
                UintegerValue(
                    0), // Enable A-MPDU with the highest maximum size allowed by the standard
                "BE_MaxAmsduSize",
                UintegerValue(0)); // Enable A-MSDU with the highest maximum size (in Bytes) allowed
        }

        for (int i = nBfBss; i < nBss; i++)
        {
            allBss[i].staDevices_net2 = wifi.Install(phy, macSta_net2, allBss[i].wifiStaNode_net2);
        }
    }

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

    // positionAlloc = setLocationScenario(1, positionAlloc, allBss, radius);
    positionAlloc = setLocationScenario(scenario, positionAlloc, allBss, radius);

    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    if (scenario == 2)
    {
        for (int i = 0; i < 12; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                mobility.Install(allBss[i].wifiApNode);
                mobility.Install(allBss[i].wifiStaNode);
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                mobility.Install(allBss[i].WifiApNode_net2);
                mobility.Install(allBss[i].wifiStaNode_net2);
            }
        }
    }
    else
    {
        for (int i = 0; i < nBss; i++)
        {
            if (i < nBfBss)
            {
                mobility.Install(allBss[i].wifiApNode);
                mobility.Install(allBss[i].wifiStaNode);
            }
            else
            {
                mobility.Install(allBss[i].WifiApNode_net2);
                mobility.Install(allBss[i].wifiStaNode_net2);
            }
        }
    }

    /* Internet Stack */
    InternetStackHelper stack;
    if (scenario == 2)
    {
        for (int i = 0; i < 12; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                stack.Install(allBss[i].wifiApNode);
                stack.Install(allBss[i].wifiStaNode);
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                stack.Install(allBss[i].WifiApNode_net2);
                stack.Install(allBss[i].wifiStaNode_net2);
            }
        }
    }
    else
    {
        for (int i = 0; i < nBss; i++)
        {
            if (i < nBfBss)
            {
                stack.Install(allBss[i].wifiApNode);
                stack.Install(allBss[i].wifiStaNode);
            }
            else
            {
                stack.Install(allBss[i].WifiApNode_net2);
                stack.Install(allBss[i].wifiStaNode_net2);
            }
        }
    }

    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");
    if (scenario == 2)
    {
        for (int i = 0; i < 12; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                allBss[i].address = address;
                allBss[i].ApInterface = address.Assign(allBss[i].apDevice);
                allBss[i].StaInterface = address.Assign(allBss[i].staDevices);
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                allBss[i].address_net2 = address;
                allBss[i].ApInterface_net2 = address.Assign(allBss[i].apDevice_net2);
                allBss[i].StaInterface_net2 = address.Assign(allBss[i].staDevices_net2);
            }
        }
    }
    else
    {
        for (int i = 0; i < nBss; i++)
        {
            if (i < nBfBss)
            {
                allBss[i].address = address;
                allBss[i].ApInterface = address.Assign(allBss[i].apDevice);
                allBss[i].StaInterface = address.Assign(allBss[i].staDevices);
            }
            else
            {
                allBss[i].address_net2 = address;
                allBss[i].ApInterface_net2 = address.Assign(allBss[i].apDevice_net2);
                allBss[i].StaInterface_net2 = address.Assign(allBss[i].staDevices_net2);
            }
        }
    }

    if (scenario == 2)
    {
        for (int i = 0; i < 12; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                Ptr<WifiNetDevice> AP_net_device =
                    DynamicCast<ns3::WifiNetDevice>(allBss[i].apDevice.Get(0));
                auto apWifiMac = StaticCast<ApWifiMac>(AP_net_device->GetMac());
                for (uint32_t j = 0; j < allBss[i].nStations_sensing; j++)
                {
                    Ptr<WifiNetDevice> STA_net_device =
                        DynamicCast<ns3::WifiNetDevice>(allBss[i].staDevices.Get(j));
                    auto staWifiMac = StaticCast<StaWifiMac>(STA_net_device->GetMac());
                    staWifiMac->SetBssid(apWifiMac->GetAddress(), 0U);
                }
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                Ptr<WifiNetDevice> AP_net_device =
                    DynamicCast<ns3::WifiNetDevice>(allBss[i].apDevice_net2.Get(0));
                auto apWifiMac = StaticCast<ApWifiMac>(AP_net_device->GetMac());
                for (uint32_t j = 0; j < allBss[i].nStations_no_sensing; j++)
                {
                    Ptr<WifiNetDevice> STA_net_device =
                        DynamicCast<ns3::WifiNetDevice>(allBss[i].staDevices_net2.Get(j));
                    auto staWifiMac = StaticCast<StaWifiMac>(STA_net_device->GetMac());
                    staWifiMac->SetBssid(apWifiMac->GetAddress(), 0U);
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < nBfBss; i++)
        {
            Ptr<WifiNetDevice> AP_net_device =
                DynamicCast<ns3::WifiNetDevice>(allBss[i].apDevice.Get(0));
            auto apWifiMac = StaticCast<ApWifiMac>(AP_net_device->GetMac());
            for (uint32_t j = 0; j < allBss[i].nStations_sensing; j++)
            {
                Ptr<WifiNetDevice> STA_net_device =
                    DynamicCast<ns3::WifiNetDevice>(allBss[i].staDevices.Get(j));
                auto staWifiMac = StaticCast<StaWifiMac>(STA_net_device->GetMac());
                staWifiMac->SetBssid(apWifiMac->GetAddress(), 0U);
            }
        }

        for (int i = nBfBss; i < nBss; i++)
        {
            Ptr<WifiNetDevice> AP_net_device =
                DynamicCast<ns3::WifiNetDevice>(allBss[i].apDevice_net2.Get(0));
            auto apWifiMac = StaticCast<ApWifiMac>(AP_net_device->GetMac());
            for (uint32_t j = 0; j < allBss[i].nStations_no_sensing; j++)
            {
                Ptr<WifiNetDevice> STA_net_device =
                    DynamicCast<ns3::WifiNetDevice>(allBss[i].staDevices_net2.Get(j));
                auto staWifiMac = StaticCast<StaWifiMac>(STA_net_device->GetMac());
                staWifiMac->SetBssid(apWifiMac->GetAddress(), 0U);
            }
        }
    }

    ApplicationContainer serverApplications, clientApplications;
    ApplicationContainer serverApplications_net2, clientApplications_net2;

    if (scenario == 2)
    {
        for (int i = 0; i < 12; i++)
        {
            if (indoorOfficeApOrder_vec[i] == 1)
            {
                /* Setting Applications for IEEE 802.11bf network*/
                uint32_t portNumber = 9;
                auto serverNodes = std::ref(allBss[i].wifiStaNode);
                Ipv4InterfaceContainer serverInterfaces;
                NodeContainer clientNodes;
                for (uint32_t index = 0; index < allBss[i].wifiStaNode.GetN(); ++index)
                {
                    serverInterfaces.Add(allBss[i].StaInterface.Get(index));
                    clientNodes.Add(allBss[i].wifiApNode.Get(0));
                }

                if (udp)
                {
                    // UDP flow
                    UdpServerHelper server(portNumber);
                    serverApplications = server.Install(serverNodes.get());
                    serverApplications.Start(Seconds(0.0));
                    serverApplications.Stop(Seconds(simulationTime + 1));

                    for (uint32_t index = 0; index < allBss[i].wifiStaNode.GetN(); ++index)
                    {
                        UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber);
                        client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
                        // client.SetAttribute("Interval",
                        //                     TimeValue(Time("0.00001"))); // packets/s
                        client.SetAttribute("Interval",
                                            TimeValue(Time("1"))); // packets/s;
                        client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                        clientApplications = client.Install(clientNodes.Get(index));
                        clientApplications.Start(Seconds(1.0));
                        clientApplications.Stop(Seconds(simulationTime + 1));
                    }
                }
                else
                {
                    // TCP flow
                    uint32_t portNumber_net2 = 50000;
                    Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), portNumber_net2));
                    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", localAddress);
                    serverApplications = packetSinkHelper.Install(serverNodes.get());
                    serverApplications.Start(Seconds(0.0));
                    serverApplications.Stop(Seconds(simulationTime + 1));

                    for (uint32_t index = 0; index < allBss[i].wifiStaNode.GetN(); ++index)
                    {
                        OnOffHelper onoff("ns3::TcpSocketFactory", Ipv4Address::GetAny());
                        onoff.SetAttribute("OnTime",
                                           StringValue("ns3::ConstantRandomVariable[Constant=1]"));
                        onoff.SetAttribute("OffTime",
                                           StringValue("ns3::ConstantRandomVariable[Constant=0]"));
                        onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
                        onoff.SetAttribute(
                            "DataRate",
                            DataRateValue(1000000000 / allBss[i].wifiStaNode.GetN())); // bit/s
                        AddressValue remoteAddress(
                            InetSocketAddress(serverInterfaces.GetAddress(index), portNumber_net2));
                        onoff.SetAttribute("Remote", remoteAddress);
                        clientApplications = onoff.Install(clientNodes.Get(index));
                        clientApplications.Start(Seconds(1.0));
                        clientApplications.Stop(Seconds(simulationTime + 1));
                    }
                }
            }
            else if (indoorOfficeApOrder_vec[i] == 0)
            {
                /* Setting applications for IEEE 802.11ax network*/
                uint32_t portNumber_net2 = 9;
                auto serverNodes = downlink ? std::ref(allBss[i].wifiStaNode_net2)
                                            : std::ref(allBss[i].WifiApNode_net2);
                Ipv4InterfaceContainer serverInterfaces;
                NodeContainer clientNodes;
                for (uint32_t index = 0; index < allBss[i].nStations_no_sensing; ++index)
                {
                    serverInterfaces.Add(downlink ? allBss[i].StaInterface_net2.Get(index)
                                                  : allBss[i].ApInterface_net2.Get(0));
                    allBss[i].WifiApNode_net2.Get(0);
                    Ptr<WifiNetDevice> wifinet22 =
                        DynamicCast<ns3::WifiNetDevice>(allBss[i].apDevice_net2.Get(0));
                    auto apWifiMac = StaticCast<ApWifiMac>(wifinet22->GetMac());
                    Ptr<QosTxop> qosTxop = apWifiMac->GetQosTxop(AC_BE);
                    u_int32_t minCw = 15, maxCw = 1023;
                    ns3::AcIndex ac = AC_BE;
                    switch (ac)
                    {
                    case AC_VO:
                        qosTxop->SetMinCw((minCw + 1) / 4 - 1);
                        qosTxop->SetMaxCw((minCw + 1) / 2 - 1);
                        qosTxop->SetAifsn(2);
                        break;
                    case AC_VI:
                        qosTxop->SetMinCw((minCw + 1) / 2 - 1);
                        qosTxop->SetMaxCw(minCw);
                        qosTxop->SetAifsn(2);
                        break;
                    case AC_BE:
                        qosTxop->SetMinCw(minCw);
                        qosTxop->SetMaxCw(maxCw);
                        qosTxop->SetAifsn(3);
                        break;
                    case AC_BK:
                        qosTxop->SetMinCw(minCw);
                        qosTxop->SetMaxCw(maxCw);
                        qosTxop->SetAifsn(7);
                        break;
                    }
                    clientNodes.Add(downlink ? allBss[i].WifiApNode_net2.Get(0)
                                             : allBss[i].wifiStaNode_net2.Get(index));
                }

                if (udp)
                {
                    // UDP flow
                    UdpServerHelper server(portNumber_net2);
                    serverApplications_net2 = server.Install(serverNodes.get());
                    serverApplications_net2.Start(Seconds(0.0));
                    serverApplications_net2.Stop(Seconds(simulationTime + 1));
                    for (uint32_t index = 0; index < allBss[i].wifiStaNode_net2.GetN(); ++index)
                    {
                        UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber_net2);
                        client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
                        std::string interval = "0.00001"; // packet every 10 us
                        if (trafficType == 1)
                        {
                            client.SetAttribute("EnablePoisson", BooleanValue(trafficType));
                            client.SetAttribute(
                                "PoissonLambda",
                                DoubleValue(1 / (atof(interval.c_str())))); // packet per second
                        }
                        else
                        {
                            client.SetAttribute("Interval",
                                                TimeValue(Time(interval))); // packet every 10 us
                        }
                        client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                        clientApplications_net2 = client.Install(clientNodes.Get(index));
                        clientApplications_net2.Start(Seconds(1.0));
                        clientApplications_net2.Stop(Seconds(simulationTime + 1));
                    }
                }
                else
                {
                    // TCP flow
                    uint32_t portNumber_net2 = 50000;
                    Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), portNumber_net2));
                    PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", localAddress);
                    serverApplications_net2 = packetSinkHelper.Install(serverNodes.get());
                    serverApplications_net2.Start(Seconds(0.0));
                    serverApplications_net2.Stop(Seconds(simulationTime + 1));

                    for (uint32_t index = 0; index < allBss[i].wifiStaNode_net2.GetN(); ++index)
                    {
                        // For TCP poisson traffic is not yet used here
                        OnOffHelper onoff("ns3::TcpSocketFactory", Ipv4Address::GetAny());
                        onoff.SetAttribute("OnTime",
                                           StringValue("ns3::ConstantRandomVariable[Constant=1]"));
                        onoff.SetAttribute("OffTime",
                                           StringValue("ns3::ConstantRandomVariable[Constant=0]"));
                        onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
                        onoff.SetAttribute(
                            "DataRate",
                            DataRateValue(1000000000 / allBss[i].wifiStaNode_net2.GetN())); // bit/s
                        AddressValue remoteAddress(
                            InetSocketAddress(serverInterfaces.GetAddress(index), portNumber_net2));
                        onoff.SetAttribute("Remote", remoteAddress);
                        clientApplications_net2 = onoff.Install(clientNodes.Get(index));
                        clientApplications_net2.Start(Seconds(1.0));
                        clientApplications_net2.Stop(Seconds(simulationTime + 1));
                    }
                }
            }
        }

        Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyTxBegin",
                        MakeCallback(&TxCallback));

        // Config::ConnectWithoutContext(
        //     "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyRxEnd",
        //     MakeCallback(&RxEndCallback));

        Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/"
                                      "$ns3::WifiPhy/MonitorChannelAccess",
                                      MakeCallback(&MonitorChannelAccess));

        Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/"
                                      "$ns3::WifiPhy/MonitorSnifferRx",
                                      MakeCallback(&MonitorSniffRx));

        if (enablePcap)
        {
            if (multipleBss)
            {
                // phy.EnablePcap("bf-wifi-network", allBss[nBfBss - 1].staDevices);
                // phy.EnablePcap("bf-wifi-network", allBss[nBfBss].staDevices_net2);
            }
            else
            {
                // phy.EnablePcap("bf-wifi-network", apDevice);
                // phy.EnablePcap("bf-wifi-network", apDevice_net2);
                // phy.EnablePcap("bf-wifi-network", staDevices);
                // phy.EnablePcap("bf-wifi-network", staDevices_net2);
                // phy.EnablePcapAll("bf-wifi-network");
            }
        }

        Simulator::Stop(Seconds(simulationTime + 1));
        Simulator::Run();

        double throughput = 0;
        double throughput_net2 = 0;
        uint64_t rxBytes = 0;
        if (udp)
        {
            for (uint32_t index = 0; index < serverApplications.GetN(); index++)
            {
                rxBytes += payloadSize *
                           DynamicCast<UdpServer>(serverApplications.Get(index))->GetReceived();
                throughput += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
            }
        }
        else
        {
            for (uint32_t index = 0; index < serverApplications.GetN(); index++)
            {
                rxBytes += DynamicCast<PacketSink>(serverApplications.Get(index))->GetTotalRx();
            }
            throughput += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
        }

        std::cout << "Overal throughput from 802.11bf BSS: " << throughput << " Mbit/s"
                  << std::endl;

        if (udp)
        {
            for (uint32_t index = 0; index < serverApplications_net2.GetN(); index++)
            {
                rxBytes +=
                    payloadSize *
                    DynamicCast<UdpServer>(serverApplications_net2.Get(index))->GetReceived();
            }
            throughput_net2 += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
        }
        else
        {
            for (uint32_t index = 0; index < serverApplications_net2.GetN(); index++)
            {
                rxBytes +=
                    DynamicCast<PacketSink>(serverApplications_net2.Get(index))->GetTotalRx();
            }
            throughput_net2 += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
        }

        std::cout << "Overal throughput from 802.11ax BSS: " << throughput_net2 << " Mbit/s"
                  << std::endl;

        if (throughput_net2 + throughput > 0)
        {
            std::cout << "# Signal (dbm): " << g_signalDbmAvg << std::endl;
            std::cout << "# Noise (dbm): " << g_noiseDbmAvg << std::endl;
            std::cout << "# SNR (db): " << (g_signalDbmAvg - g_noiseDbmAvg) << std::endl;
        }
        else
        {
            std::cout << "# Signal (dbm): " << "N/A" << std::endl;
            std::cout << "# Noise (dbm): " << "N/A" << std::endl;
            std::cout << "# SNR (db): " << "N/A" << std::endl;
        }

        m_avgTrueLatency = Seconds(0);
        m_innerCounter = 0;
        for (auto it = allCalculation.begin(); it != allCalculation.end(); ++it)
        {
            if (it->second.m_avgTrueLatency > Seconds(0) && it->second.m_innerCounter > 0)
            {
                m_avgTrueLatency += it->second.m_avgTrueLatency / it->second.m_innerCounter;
            }
            m_innerCounter += it->second.m_innerCounter;
            m_collisionCounter += it->second.m_collisionCounter;
        }

        std::cout << "# tx beacons: " << m_countBeacon << std::endl;
        std::cout << "# tx CF-END: " << m_countCfEnd << std::endl;
        std::cout << "# tx CF-END-ACK: " << m_countCfEndAck << std::endl;
        std::cout << "# tx CF-POLL: " << m_countCfPoll << std::endl;
        std::cout << "# tx DATA-NULL-ANNOUNCEMENT: " << m_countDataNullAnnouncement << std::endl;
        std::cout << "# tx DATA-NULL: " << m_countDataNull << std::endl;
        std::cout << "# tx CSI-BEAMFORMING-REPORT: " << m_csiBeamformingReport << std::endl;
        std::cout << "# tx DATA: " << m_countData << std::endl;
        std::cout << "# tx CTS: " << m_countCTS << std::endl;
        std::cout << "# tx CTS-to-self: " << m_countCTStoSelf << std::endl;
        if (nBfBss > 0)
        {
            if (m_avgTrueLatency.GetSeconds() > 0.0)
            {
                std::cout << "# average latency: " << m_avgTrueLatency.GetSeconds() / nBfBss
                          << std::endl;
            }
            else
            {
                std::cout << "# average latency: -nan " << std::endl;
            }
        }
        std::cout << "# successful sensing: " << m_innerCounter << std::endl;
        int unsuccessfulSensing =
            simulationTime * (1000 / sensingInterval) * nBfBss - m_innerCounter;
        if (unsuccessfulSensing < 0)
            unsuccessfulSensing = 0;
        std::cout << "# unsuccessfull sensing: " << unsuccessfulSensing << std::endl;
    }
    else
    {
        /* Setting Applications for IEEE 802.11bf network*/
        uint32_t portNumber = 9;
        for (int i = 0; i < nBfBss; i++)
        {
            auto serverNodes = std::ref(allBss[i].wifiStaNode);
            Ipv4InterfaceContainer serverInterfaces;
            NodeContainer clientNodes;
            for (uint32_t index = 0; index < allBss[i].wifiStaNode.GetN(); ++index)
            {
                serverInterfaces.Add(allBss[i].StaInterface.Get(index));
                clientNodes.Add(allBss[i].wifiApNode.Get(0));
            }

            if (udp)
            {
                // UDP flow for Access Point
                UdpServerHelper server(portNumber);
                serverApplications = server.Install(serverNodes.get());
                serverApplications.Start(Seconds(0.0));
                serverApplications.Stop(Seconds(simulationTime + 1));

                for (uint32_t index = 0; index < allBss[i].wifiStaNode.GetN(); ++index)
                {
                    UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber);
                    client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
                    // client.SetAttribute("MaxPackets", UintegerValue(1U));
                    client.SetAttribute("Interval",
                                        TimeValue(Time("0.00001"))); // packets/s
                    // client.SetAttribute("Interval",
                    //                     TimeValue(Time("1"))); // packets/s;
                    client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                    clientApplications = client.Install(clientNodes.Get(index));
                    clientApplications.Start(Seconds(1.0));
                    clientApplications.Stop(Seconds(simulationTime + 1));
                }
            }
            else
            {
                // TCP flow
                uint32_t portNumber_net2 = 50000;
                Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), portNumber_net2));
                PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", localAddress);
                serverApplications = packetSinkHelper.Install(serverNodes.get());
                serverApplications.Start(Seconds(0.0));
                serverApplications.Stop(Seconds(simulationTime + 1));

                for (uint32_t index = 0; index < allBss[i].wifiStaNode.GetN(); ++index)
                {
                    OnOffHelper onoff("ns3::TcpSocketFactory", Ipv4Address::GetAny());
                    onoff.SetAttribute("OnTime",
                                       StringValue("ns3::ConstantRandomVariable[Constant=1]"));
                    onoff.SetAttribute("OffTime",
                                       StringValue("ns3::ConstantRandomVariable[Constant=0]"));
                    onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
                    onoff.SetAttribute(
                        "DataRate",
                        DataRateValue(1000000000 / allBss[i].wifiStaNode.GetN())); // bit/s
                    AddressValue remoteAddress(
                        InetSocketAddress(serverInterfaces.GetAddress(index), portNumber_net2));
                    onoff.SetAttribute("Remote", remoteAddress);
                    clientApplications = onoff.Install(clientNodes.Get(index));
                    clientApplications.Start(Seconds(1.0));
                    clientApplications.Stop(Seconds(simulationTime + 1));
                }
            }
        }

        /* Setting applications for IEEE 802.11ax network*/
        uint32_t portNumber_net2 = 9;
        for (int i = nBfBss; i < nBss; i++)
        {
            auto serverNodes = downlink ? std::ref(allBss[i].wifiStaNode_net2)
                                        : std::ref(allBss[i].WifiApNode_net2);
            Ipv4InterfaceContainer serverInterfaces;
            NodeContainer clientNodes;
            for (uint32_t index = 0; index < allBss[i].nStations_no_sensing; ++index)
            {
                serverInterfaces.Add(downlink ? allBss[i].StaInterface_net2.Get(index)
                                              : allBss[i].ApInterface_net2.Get(0));
                allBss[i].WifiApNode_net2.Get(0);
                Ptr<WifiNetDevice> wifinet22 =
                    DynamicCast<ns3::WifiNetDevice>(allBss[i].apDevice_net2.Get(0));
                auto apWifiMac = StaticCast<ApWifiMac>(wifinet22->GetMac());
                Ptr<QosTxop> qosTxop = apWifiMac->GetQosTxop(AC_BE);
                u_int32_t minCw = 15, maxCw = 1023;
                ns3::AcIndex ac = AC_BE;
                switch (ac)
                {
                case AC_VO:
                    qosTxop->SetMinCw((minCw + 1) / 4 - 1);
                    qosTxop->SetMaxCw((minCw + 1) / 2 - 1);
                    qosTxop->SetAifsn(2);
                    break;
                case AC_VI:
                    qosTxop->SetMinCw((minCw + 1) / 2 - 1);
                    qosTxop->SetMaxCw(minCw);
                    qosTxop->SetAifsn(2);
                    break;
                case AC_BE:
                    qosTxop->SetMinCw(minCw);
                    qosTxop->SetMaxCw(maxCw);
                    qosTxop->SetAifsn(3);
                    break;
                case AC_BK:
                    qosTxop->SetMinCw(minCw);
                    qosTxop->SetMaxCw(maxCw);
                    qosTxop->SetAifsn(7);
                    break;
                }
                clientNodes.Add(downlink ? allBss[i].WifiApNode_net2.Get(0)
                                         : allBss[i].wifiStaNode_net2.Get(index));
            }

            if (udp)
            {
                // UDP flow
                UdpServerHelper server(portNumber_net2);
                serverApplications_net2 = server.Install(serverNodes.get());
                serverApplications_net2.Start(Seconds(0.0));
                serverApplications_net2.Stop(Seconds(simulationTime + 1));
                for (uint32_t index = 0; index < allBss[i].wifiStaNode_net2.GetN(); ++index)
                {
                    UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber_net2);
                    client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
                    std::string interval = "0.00001"; // packet every 10 us
                    if (trafficType == 1)
                    {
                        client.SetAttribute("EnablePoisson", BooleanValue(trafficType));
                        client.SetAttribute(
                            "PoissonLambda",
                            DoubleValue(1 / (atof(interval.c_str())))); // packet per second
                    }
                    else
                    {
                        client.SetAttribute("Interval",
                                            TimeValue(Time(interval))); // packet every 10 us
                    }
                    client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                    clientApplications_net2 = client.Install(clientNodes.Get(index));
                    clientApplications_net2.Start(Seconds(1.0));
                    clientApplications_net2.Stop(Seconds(simulationTime + 1));
                }
            }
            else
            {
                // TCP flow
                uint32_t portNumber_net2 = 50000;
                Address localAddress(InetSocketAddress(Ipv4Address::GetAny(), portNumber_net2));
                PacketSinkHelper packetSinkHelper("ns3::TcpSocketFactory", localAddress);
                serverApplications_net2 = packetSinkHelper.Install(serverNodes.get());
                serverApplications_net2.Start(Seconds(0.0));
                serverApplications_net2.Stop(Seconds(simulationTime + 1));

                for (uint32_t index = 0; index < allBss[i].wifiStaNode_net2.GetN(); ++index)
                {
                    // For TCP poisson traffic is not yet used here
                    OnOffHelper onoff("ns3::TcpSocketFactory", Ipv4Address::GetAny());
                    onoff.SetAttribute("OnTime",
                                       StringValue("ns3::ConstantRandomVariable[Constant=1]"));
                    onoff.SetAttribute("OffTime",
                                       StringValue("ns3::ConstantRandomVariable[Constant=0]"));
                    onoff.SetAttribute("PacketSize", UintegerValue(payloadSize));
                    onoff.SetAttribute(
                        "DataRate",
                        DataRateValue(1000000000 / allBss[i].wifiStaNode_net2.GetN())); // bit/s
                    AddressValue remoteAddress(
                        InetSocketAddress(serverInterfaces.GetAddress(index), portNumber_net2));
                    onoff.SetAttribute("Remote", remoteAddress);
                    clientApplications_net2 = onoff.Install(clientNodes.Get(index));
                    clientApplications_net2.Start(Seconds(1.0));
                    clientApplications_net2.Stop(Seconds(simulationTime + 1));
                }
            }
        }

        Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyTxBegin",
                        MakeCallback(&TxCallback));

        // Config::ConnectWithoutContext(
        //     "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyRxEnd",
        //     MakeCallback(&RxEndCallback));

        Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/"
                                      "$ns3::WifiPhy/MonitorChannelAccess",
                                      MakeCallback(&MonitorChannelAccess));

        Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/"
                                      "$ns3::WifiPhy/MonitorSnifferRx",
                                      MakeCallback(&MonitorSniffRx));

        if (enablePcap)
        {
            if (multipleBss)
            {
                // phy.EnablePcap("bf-wifi-network", allBss[nBfBss - 1].staDevices);
                phy.EnablePcap("ax-wifi-network", allBss[nBfBss].staDevices_net2);
            }
            else
            {
                // phy.EnablePcap("bf-wifi-network", apDevice);
                // phy.EnablePcap("bf-wifi-network", apDevice_net2);
                // phy.EnablePcap("bf-wifi-network", staDevices);
                // phy.EnablePcap("bf-wifi-network", staDevices_net2);
                // phy.EnablePcapAll("bf-wifi-network");
            }
        }
        Simulator::Stop(Seconds(simulationTime + 1));
        Simulator::Run();

        double throughput = 0;
        double throughput_net2 = 0;
        uint64_t rxBytes = 0;
        if (nAxBss > 0)
        {
            if (udp)
            {
                for (uint32_t index = 0; index < serverApplications.GetN(); index++)
                {
                    rxBytes += payloadSize *
                               DynamicCast<UdpServer>(serverApplications.Get(index))->GetReceived();
                    throughput += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
                }
            }
            else
            {
                for (uint32_t index = 0; index < serverApplications.GetN(); index++)
                {
                    rxBytes += DynamicCast<PacketSink>(serverApplications.Get(index))->GetTotalRx();
                }
                throughput += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
            }

            std::cout << "Overal throughput from 802.11bf BSS: " << throughput << " Mbit/s"
                      << std::endl;

            if (udp)
            {
                for (uint32_t index = 0; index < serverApplications_net2.GetN(); index++)
                {
                    rxBytes +=
                        payloadSize *
                        DynamicCast<UdpServer>(serverApplications_net2.Get(index))->GetReceived();
                }
                throughput_net2 += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
            }
            else
            {
                for (uint32_t index = 0; index < serverApplications_net2.GetN(); index++)
                {
                    rxBytes +=
                        DynamicCast<PacketSink>(serverApplications_net2.Get(index))->GetTotalRx();
                }
                throughput_net2 += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
            }

            std::cout << "Overal throughput from 802.11ax BSS: " << throughput_net2 << " Mbit/s"
                      << std::endl;

            if (throughput_net2 + throughput > 0)
            {
                std::cout << "# Signal (dbm): " << g_signalDbmAvg << std::endl;
                std::cout << "# Noise (dbm): " << g_noiseDbmAvg << std::endl;
                std::cout << "# SNR (db): " << (g_signalDbmAvg - g_noiseDbmAvg) << std::endl;
            }
            else
            {
                std::cout << "# Signal (dbm): " << "N/A" << std::endl;
                std::cout << "# Noise (dbm): " << "N/A" << std::endl;
                std::cout << "# SNR (db): " << "N/A" << std::endl;
            }
        }
        else
        {
            for (uint32_t index = 0; index < serverApplications.GetN(); index++)
            {
                rxBytes += payloadSize *
                           DynamicCast<UdpServer>(serverApplications.Get(index))->GetReceived();
                throughput += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
            }
            std::cout << "Overal throughput from 802.11bf BSS: " << throughput << " Mbit/s"
                      << std::endl;

            if (throughput_net2 + throughput > 0)
            {
                std::cout << "# Signal (dbm): " << g_signalDbmAvg << std::endl;
                std::cout << "# Noise (dbm): " << g_noiseDbmAvg << std::endl;
                std::cout << "# SNR (db): " << (g_signalDbmAvg - g_noiseDbmAvg) << std::endl;
            }
            else
            {
                std::cout << "# Signal (dbm): " << "N/A" << std::endl;
                std::cout << "# Noise (dbm): " << "N/A" << std::endl;
                std::cout << "# SNR (db): " << "N/A" << std::endl;
            }
        }

        m_avgTrueLatency = Seconds(0);
        m_innerCounter = 0;
        for (auto it = allCalculation.begin(); it != allCalculation.end(); ++it)
        {
            if (it->second.m_avgTrueLatency > Seconds(0) && it->second.m_innerCounter > 0)
            {
                m_avgTrueLatency += it->second.m_avgTrueLatency / it->second.m_innerCounter;
            }
            m_innerCounter += it->second.m_innerCounter;
            m_collisionCounter += it->second.m_collisionCounter;
        }

        std::cout << "# tx beacons: " << m_countBeacon << std::endl;
        std::cout << "# tx CF-END: " << m_countCfEnd << std::endl;
        std::cout << "# tx CF-END-ACK: " << m_countCfEndAck << std::endl;
        std::cout << "# tx CF-POLL: " << m_countCfPoll << std::endl;
        std::cout << "# tx DATA-NULL-ANNOUNCEMENT: " << m_countDataNullAnnouncement << std::endl;
        std::cout << "# tx DATA-NULL: " << m_countDataNull << std::endl;
        std::cout << "# tx CSI-BEAMFORMING-REPORT: " << m_csiBeamformingReport << std::endl;
        std::cout << "# tx DATA: " << m_countData << std::endl;
        std::cout << "# tx CTS: " << m_countCTS << std::endl;
        std::cout << "# tx CTS-to-self: " << m_countCTStoSelf << std::endl;
        if (nBfBss > 0)
        {
            if (m_avgTrueLatency.GetSeconds() > 0.0)
            {
                std::cout << "# average latency (bf): " << m_avgTrueLatency.GetSeconds() / nBfBss
                          << std::endl;
            }
            else
            {
                std::cout << "# average latency (bf): -nan " << std::endl;
            }
        }

        m_avgTrueLatency_net2 = Seconds(0);
        for (auto it = allCalculation_net2.begin(); it != allCalculation_net2.end(); ++it)
        {
            if (it->second.m_avgTrueLatency > Seconds(0) && it->second.m_innerCounter > 0)
            {
                // m_avgTrueLatency_net2 += it->second.m_avgTrueLatency /
                // (it->second.m_innerCounter);
                m_avgTrueLatency_net2 += it->second.m_avgTrueLatency;
            }
            // std::cout << it->first << " average latency: " << it->second.stillSensing <<
            // std::endl;
        }
        if (nAxBss > 0)
        {
            if (m_avgTrueLatency_net2.GetSeconds() > 0.0)
            {
                std::cout << "# average latency (ax): "
                          << m_avgTrueLatency_net2.GetSeconds() / nAxBss << std::endl;
            }
            else
            {
                std::cout << "# average latency (ax): -nan " << std::endl;
            }
        }

        std::cout << "# successful sensing: " << m_innerCounter << std::endl;
        int unsuccessfulSensing =
            simulationTime * (1000 / sensingInterval) * nBfBss - m_innerCounter;
        if (unsuccessfulSensing < 0)
            unsuccessfulSensing = 0;
        std::cout << "# unsuccessfull sensing: " << unsuccessfulSensing << std::endl;

        Simulator::Destroy();
        return 0;
    }
}
