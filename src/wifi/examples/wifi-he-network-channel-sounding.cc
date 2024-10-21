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

#include "ns3/boolean.h"
#include "ns3/channel-sounding.h"
#include "ns3/command-line.h"
#include "ns3/config.h"
#include "ns3/double.h"
#include "ns3/he-phy.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/log.h"
#include "ns3/mobility-helper.h"
#include "ns3/multi-model-spectrum-channel.h"
#include "ns3/multi-user-scheduler.h"
#include "ns3/pointer.h"
#include "ns3/qos-txop.h"
#include "ns3/rng-seed-manager.h"
#include "ns3/spectrum-wifi-helper.h"
#include "ns3/ssid.h"
#include "ns3/string.h"
#include "ns3/udp-client-server-helper.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/wifi-mac.h"
#include "ns3/wifi-net-device.h"
#include "ns3/yans-wifi-channel.h"
#include "ns3/yans-wifi-helper.h"

#include <cmath>
#include <functional>

// This is a simple example to show how to configure channel sounding process in an IEEE 802.11ax
// network with UDP traffic, which is based on examples/wireless/wifi-he-network.cc. The network has
// one access point (AP) and several stations. In channel sounding process, the AP sends NDPA frame,
// NDP frame, and BFRP trigger frame (if it is an MU case) to stations. Then stations will send
// beamforming report frames back to the AP. The example can be used to analyze channel sounding
// overhead and network throughput. The following parameters related to channel sounding process can
// be configured in this example: (1) Bandwidth (2) Subcarrier grouping (3) Codebook size (4) Size
// of compressed beamforming feedback matrix (5) MCS used to feedback beamforming report frame (6)
// Channel sounding interval
//
// For further details on the implementation, please refer to the following paper:
// J. Zhang, S. Avallone, and D. Blough, "Implementation and Evaluation of IEEE 802.11ax Channel
// Sounding Frame Exchange in ns-3," Proc. of the Workshop on ns-3, to appear, 2023.
//

/*
  Simulation parameters used to generate data points in the simulation sections(Figure 4 - Figure
 10) of the above paper are introduced as follows.

  (1) Figure 4:
    The paramter of channelWidth is chosen as 20,40,80,160, separately.
    All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 4;
    Time channelSoundingInterval{"5ms"};
    uint32_t payloadSize = 200;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 4;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = true;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs0";
    int mcs{0};
    int nLoop = 1;

  (2) Figure 5:
    The pair of simulation parameters (numAntennas, nc) are chosen as: (2,1), (2,2), (3,1), (3,3),
 (4,1), (4,4).
    All the other simualtion parameters (Part 1) should be configured as follows:
    std::size_t nStations = 4;
    int channelWidth{160};
    Time channelSoundingInterval{"5ms"};
    uint32_t payloadSize = 200;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    bool printChannelSoundingDuration = true;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs0";
    int mcs{0};
    int nLoop = 1;

  (3) Figure 6:
    To get the 8 data points in Figure6, the paramters of (nStations, numAntennas, nc, ngSu, ngMu)
 are chosen as (1,2,1,4,4), (1,4,4,4,4), (4,2,1,4,4), (4,4,4,4,4), (1,2,1,16,16), (1,4,4,16,16),
 (4,2,1,16,16), (4,4,4,16,16).
    All the other simualtion parameters (Part 1) should be configured as
 follows:
    int channelWidth{160};
    Time channelSoundingInterval{"5ms"};
    uint32_t payloadSize = 200;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    bool printChannelSoundingDuration = true;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs0";
    int mcs{0};
    int nLoop = 1;

 (4) Figure 7:
    To get the 8 data points in Figure 7, the paramters of (nStations, numAntennas, nc,
 codebookSizeSu, codebookSizeMu) are chosen as (1,2,1,"(6,4)","(9,7)"), (1,4,4,"(6,4)","(9,7)"),
 (4,2,1,"(6,4)","(9,7)"), (4,4,4,"(6,4)","(9,7)"), (1,2,1,"(4,2)","(7,5)"), (1,4,4,"(4,2)","(7,5)"),
 (4,2,1,"(4,2)","(7,5)"), (4,4,4,"(4,2)","(7,5)").
    All the other simualtion parameters (Part 1) should be configured as follow:
    int channelWidth{160};
    Time channelSoundingInterval{"5ms"};
    uint32_t payloadSize = 200;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    bool printChannelSoundingDuration = true;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs0";
    int mcs{0};
    int nLoop = 1;

 (5) Figure 8:
    The parameter of csMode is chosen as: "HeMcs0", "HeMcs2", "HeMcs4", "HeMcs6", "HeMcs8",
 "HeMcs10", "HeMcs11" All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 4;
    int channelWidth{160};
    Time channelSoundingInterval{"5ms"};
    uint32_t payloadSize = 200;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 4;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = true;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    int mcs{0};
    int nLoop = 1;

  (6) Figure 9 (a)
    The paramter of channelSoundingInterval is chosen as "0", "5ms","10ms","20ms", separately.
    All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 2;
    int channelWidth{20};
    uint32_t payloadSize = 700;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 1;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = false;
    uint8_t nssPerSta = 1;
    uint8_t maxNumDlMuMimoSta = 2;
    std::string csMode = "HeMcs0";
    int mcs{-1};
    int nLoop = 10;

  (7) Figure 9 (b)
    The paramter of channelSoundingInterval is chosen as "0", "5ms","10ms","20ms", separately.
    All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 2;
    int channelWidth{20};
    uint32_t payloadSize = 700;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 2;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = false;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs0";
    int mcs{-1};
    int nLoop = 10;

  (8) Figure 9 (c)
    The paramter of channelSoundingInterval is chosen as "0", "5ms","10ms","20ms", separately.
    All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 1;
    int channelWidth{20};
    uint32_t payloadSize = 700;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 2;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = false;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs0";
    int mcs{-1};
    int nLoop = 10;

  (9) Figure 10 (a)
    The paramter of channelSoundingInterval is chosen as "0", "5ms","10ms","20ms", separately.
    The parameter of mcs is chosen from 4 to 11;
    All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 2;
    int channelWidth{20};
    uint32_t payloadSize = 700;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 2;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = false;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "HeMcs4";
    int nLoop = 10;

  (10 )Figure 10 (b)
    The paramter of channelSoundingInterval is chosen as "0", "5ms","10ms","20ms", separately.
    All the other simualtion parameters (Part 1) should be configured as follow:
    std::size_t nStations = 2;
    int channelWidth{20};
    uint32_t payloadSize = 700;
    uint8_t ngSu = 4;
    uint8_t ngMu = 4;
    std::string codebookSizeSu = "(6,4)";
    std::string codebookSizeMu = "(9,7)";
    uint8_t nc = 2;
    uint8_t numAntennas = 4;
    bool printChannelSoundingDuration = false;
    uint8_t nssPerSta = 2;
    uint8_t maxNumDlMuMimoSta = 1;
    std::string csMode = "0";
    int mcs{-1};
    int nLoop = 10;
*/

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("he-wifi-network");

int
main(int argc, char* argv[])
{
    // Simulation parameters (Part 1)
    std::size_t nStations = 2;            // Number of stations
    int channelWidth{20};                 // Bandwidth
    Time channelSoundingInterval{"5ms"};  // Channel sounding interval (Channel sounding is disabled
                                          // if the interval is set as "0")
    uint32_t payloadSize = 700;           // Payload size for UDP traffic
    uint8_t ngSu = 4;                     // Subcarrier grouping Ng for SU channel sounding
    uint8_t ngMu = 4;                     // Subcarrier grouping Ng for MU channel sounding
    std::string codebookSizeSu = "(6,4)"; // Codebook size for SU channel sounding
    std::string codebookSizeMu = "(9,7)"; // Codebook size for MU channel sounding
    uint8_t nc = 2;          // Number of colums in the compressed beamforming feedback matrix
    uint8_t numAntennas = 4; // Number of rows in the compressed beamforming feedback matrix
    bool printChannelSoundingDuration = false; // Whether to print channel sounding duration
    uint8_t nssPerSta = 1; // Number of streams transmitted to each station in data transmission
    uint8_t maxNumDlMuMimoSta =
        2; // Maximum number of stations in downlink MU-MIMO data transmission
    std::string csMode =
        "HeMcs0";   // Wifi mode used for beamforming report feedback (If set as "0", wifi mode is
                    // automatically selected as the same mode as in data transmission)
    int mcs{-1};    // MCS used for both beamforming report feedback in channel sounding and data
                    // transmission (-1 indicates an unset value)
    int nLoop = 10; // Number of simulation runs to get average throughput

    // Simulation parameters (Part 2)
    Time accessReqInterval{0};
    int gi = 1600; // Guard interval (gi does not affet guard interval used in NDP frame in channel
                   // sounding. Currently, guard interval used in NDP frame is fixed as 0.8us.)
    Time txopLimit{"0"};         // TXOP limits
    double simulationTime = 2.0; // seconds
    double distance = 1.0;       // Distance between AP and stations (m)
    bool enablePcap = false;

    CommandLine cmd(__FILE__);

    cmd.AddValue("distance",
                 "Distance in meters between the station and the access point",
                 distance);
    cmd.AddValue("simulationTime", "Simulation time in seconds", simulationTime);
    cmd.AddValue("nStations", "Number of non-AP HE stations", nStations);
    cmd.AddValue("channelWidth", "Channel bandwidth", channelWidth);
    cmd.AddValue(
        "channelSoundingInterval",
        "Channel sounding interval (Channel sounding is disabled if the interval is set as 0)",
        channelSoundingInterval);
    cmd.AddValue("payloadSize", "The application payload size in bytes", payloadSize);
    cmd.AddValue(
        "muSchedAccessReqInterval",
        "Duration of the interval between two requests for channel access made by the MU scheduler",
        accessReqInterval);
    cmd.AddValue("gi", "Guard interval", gi);
    cmd.AddValue("ngSu", "Subcarrier grouping Ng for SU channel sounding", ngSu);
    cmd.AddValue("ngMu", "Subcarrier grouping Ng for MU channel sounding", ngMu);
    cmd.AddValue("codebookSizeSu", "Codebook size for SU channel sounding", codebookSizeSu);
    cmd.AddValue("codebookSizeMu", "Codebook size for MU channel sounding", codebookSizeMu);
    cmd.AddValue("numAntennas",
                 "Number of rows in the compressed beamforming feedback matrix",
                 numAntennas);
    cmd.AddValue("nc", "Number of colums in the compressed beamforming feedback matrix", nc);
    cmd.AddValue("printChannelSoundingDuration",
                 "Whether to print channel sounding duration",
                 printChannelSoundingDuration);
    cmd.AddValue("nssPerSta",
                 "Number of streams transmitted to each station in data transmission",
                 nssPerSta);
    cmd.AddValue("maxNumDlMuMimoSta",
                 "Maximum number of stations in downlink MU-MIMO data transmission",
                 maxNumDlMuMimoSta);
    cmd.AddValue("mcs", "If set, limit testing to a specific MCS (0-11)", mcs);
    cmd.AddValue("txopLimit", "TXOP limits", txopLimit);
    cmd.AddValue("nLoop", "Number of simulation runs to get average throughput", nLoop);
    cmd.AddValue("enablePcap", "Whether to output PCAP file", enablePcap);
    cmd.AddValue("csMode",
                 "Wifi mode used for beamforming report feedback (If set as '0', wifi mode is "
                 "automatically selected as the same mode as in data transmission)",
                 csMode);
    cmd.Parse(argc, argv);

    Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                       EnumValue(WifiAcknowledgment::DL_MU_TF_MU_BAR));

    Config::SetDefault("ns3::HeFrameExchangeManager::PrintChannelSoundingDuration",
                       BooleanValue(printChannelSoundingDuration));

    Config::SetDefault("ns3::HeFrameExchangeManager::ChannelSoundingWifiMode", StringValue(csMode));

    std::cout << "MCS value"
              << "\t\t"
              << "Channel width"
              << "\t\t"
              << "GI"
              << "\t\t\t"
              << "Throughput" << '\n';
    int minMcs = 0;
    int maxMcs = 11;
    if (mcs >= 0 && mcs <= 11)
    {
        minMcs = mcs;
        maxMcs = mcs;
    }

    int minBw = 20;
    int maxBw = 160;
    if (channelWidth >= 20 && channelWidth <= 160)
    {
        minBw = channelWidth;
        maxBw = channelWidth;
    }

    for (int bw = minBw; bw <= maxBw; bw = 2 * bw)
    {
        for (int mcs = minMcs; mcs <= maxMcs; mcs++)
        {
            double meanThroughput = 0;
            for (int n = 0; n < nLoop; n++)
            {
                RngSeedManager::SetSeed(n + 1);
                RngSeedManager::SetRun(n + 1);

                NodeContainer wifiStaNodes;
                wifiStaNodes.Create(nStations);
                NodeContainer wifiApNode;
                wifiApNode.Create(1);

                NetDeviceContainer apDevice;
                NetDeviceContainer staDevices;
                WifiMacHelper mac;
                WifiHelper wifi;
                std::string channelStr("{0, " + std::to_string(bw) + ", ");

                wifi.SetStandard(WIFI_STANDARD_80211ax);
                std::ostringstream ossControlMode;
                StringValue ctrlRate;
                auto nonHtRefRateMbps = HePhy::GetNonHtReferenceRate(mcs) / 1e6;
                ossControlMode << "OfdmRate" << nonHtRefRateMbps << "Mbps";
                ctrlRate = StringValue(ossControlMode.str());
                channelStr += "BAND_5GHZ, 0}";
                std::ostringstream ossDataMode;
                ossDataMode << "HeMcs" << mcs;

                std::ostringstream oss;
                oss << "HeMcs" << mcs;
                wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                             "DataMode",
                                             StringValue(ossDataMode.str()),
                                             "ControlMode",
                                             StringValue(ctrlRate));

                wifi.ConfigHeOptions("GuardInterval",
                                     TimeValue(NanoSeconds(gi)),
                                     "MpduBufferSize",
                                     UintegerValue(64),
                                     "NgSu",
                                     UintegerValue(ngSu),
                                     "NgMu",
                                     UintegerValue(ngMu),
                                     "CodebookSizeSu",
                                     StringValue(codebookSizeSu),
                                     "CodebookSizeMu",
                                     StringValue(codebookSizeMu),
                                     "MaxNc",
                                     UintegerValue(nc - 1));

                Ssid ssid = Ssid("ns3-80211ax");

                Ptr<MultiModelSpectrumChannel> spectrumChannel =
                    CreateObject<MultiModelSpectrumChannel>();
                SpectrumWifiPhyHelper phy;
                phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
                phy.SetChannel(spectrumChannel);
                phy.Set("ChannelSettings", StringValue(channelStr));
                phy.Set("MaxSupportedTxSpatialStreams", UintegerValue(numAntennas));
                phy.Set("MaxSupportedRxSpatialStreams", UintegerValue(numAntennas));
                phy.Set("Antennas", UintegerValue(numAntennas));

                mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
                staDevices = wifi.Install(phy, mac, wifiStaNodes);

                mac.SetMultiUserScheduler("ns3::RrMultiUserScheduler",
                                          "EnableUlOfdma",
                                          BooleanValue(true),
                                          "EnableBsrp",
                                          BooleanValue(false),
                                          "AccessReqInterval",
                                          TimeValue(accessReqInterval),
                                          "ChannelSoundingInterval",
                                          TimeValue(channelSoundingInterval),
                                          "EnableMuMimo",
                                          BooleanValue(true),
                                          "NumStreamPerSta",
                                          UintegerValue(nssPerSta),
                                          "MaxNumDlMuMimoSta",
                                          UintegerValue(maxNumDlMuMimoSta));

                mac.SetType("ns3::ApWifiMac",
                            "EnableBeaconJitter",
                            BooleanValue(false),
                            "Ssid",
                            SsidValue(ssid));
                apDevice = wifi.Install(phy, mac, wifiApNode);

                Ptr<NetDevice> dev = wifiApNode.Get(0)->GetDevice(0);
                Ptr<WifiNetDevice> wifi_dev = DynamicCast<WifiNetDevice>(dev);
                Ptr<WifiMac> wifi_mac = wifi_dev->GetMac();
                PointerValue ptr;
                Ptr<QosTxop> edca;
                wifi_mac->GetAttribute("BE_Txop", ptr);
                edca = ptr.Get<QosTxop>();
                edca->SetTxopLimit(txopLimit);

                wifi_mac->GetAttribute("BK_Txop", ptr);
                edca = ptr.Get<QosTxop>();
                edca->SetTxopLimit(txopLimit);

                wifi_mac->GetAttribute("VI_Txop", ptr);
                edca = ptr.Get<QosTxop>();
                edca->SetTxopLimit(txopLimit);

                wifi_mac->GetAttribute("VO_Txop", ptr);
                edca = ptr.Get<QosTxop>();
                edca->SetTxopLimit(txopLimit);

                int64_t streamNumber = 150;
                streamNumber += wifi.AssignStreams(apDevice, streamNumber);
                streamNumber += wifi.AssignStreams(staDevices, streamNumber);

                /* Mobility model*/
                MobilityHelper mobility;
                Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

                positionAlloc->Add(Vector(0.0, 0.0, 0.0));

                for (std::size_t i_sta = 0; i_sta < nStations; i_sta++)
                {
                    positionAlloc->Add(Vector(distance * cos(i_sta * 2 * 3.14159 / nStations),
                                              distance * sin(i_sta * 2 * 3.14159 / nStations),
                                              0.0));
                }
                mobility.SetPositionAllocator(positionAlloc);

                mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

                mobility.Install(wifiApNode);
                mobility.Install(wifiStaNodes);

                /* Internet stack*/
                InternetStackHelper stack;
                stack.Install(wifiApNode);
                stack.Install(wifiStaNodes);

                Ipv4AddressHelper address;
                address.SetBase("192.168.1.0", "255.255.255.0");
                Ipv4InterfaceContainer staNodeInterfaces;
                Ipv4InterfaceContainer apNodeInterface;

                staNodeInterfaces = address.Assign(staDevices);
                apNodeInterface = address.Assign(apDevice);

                /* Setting applications */
                ApplicationContainer serverApp;
                auto serverNodes = std::ref(wifiStaNodes);
                Ipv4InterfaceContainer serverInterfaces;
                NodeContainer clientNodes;
                for (std::size_t i = 0; i < nStations; i++)
                {
                    serverInterfaces.Add(staNodeInterfaces.Get(i));
                    clientNodes.Add(wifiApNode.Get(0));
                }

                // UDP flow
                uint16_t port = 9;
                UdpServerHelper server(port);
                serverApp = server.Install(serverNodes.get());
                serverApp.Start(Seconds(0.0));
                serverApp.Stop(Seconds(simulationTime + 1));

                for (std::size_t i = 0; i < nStations; i++)
                {
                    UdpClientHelper client(serverInterfaces.GetAddress(i), port);
                    client.SetAttribute("MaxPackets",
                                        UintegerValue(4294967295U));             // 4294967295U
                    client.SetAttribute("Interval", TimeValue(Time("0.00001"))); // packets/s
                    client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                    ApplicationContainer clientApp = client.Install(clientNodes.Get(i));
                    clientApp.Start(Seconds(1.0));
                    clientApp.Stop(Seconds(simulationTime + 1));
                }

                if (enablePcap)
                {
                    phy.EnablePcap("wifi-channel-sounding", apDevice.Get(0));
                }

                Simulator::Schedule(Seconds(0), &Ipv4GlobalRoutingHelper::PopulateRoutingTables);

                Simulator::Stop(Seconds(simulationTime + 1));
                Simulator::Run();

                uint64_t rxBytes = 0;

                for (uint32_t i = 0; i < serverApp.GetN(); i++)
                {
                    rxBytes +=
                        payloadSize * DynamicCast<UdpServer>(serverApp.Get(i))->GetReceived();
                }

                double throughput = (rxBytes * 8) / (simulationTime * 1000000.0); // Mbit/s

                Simulator::Destroy();

                meanThroughput += throughput;
            }
            std::cout << mcs << "\t\t\t" << bw << " MHz\t\t\t" << gi << " ns\t\t\t"
                      << meanThroughput / nLoop << " Mbit/s" << std::endl;
        }
    }
    return 0;
}
