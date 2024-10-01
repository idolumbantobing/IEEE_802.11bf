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
#include "ns3/he-phy.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
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
#include "ns3/udp-client-server-helper.h"
#include "ns3/uinteger.h"
#include "ns3/wifi-acknowledgment.h"
#include "ns3/wifi-mac-header.h"
#include "ns3/yans-wifi-helper.h"

/* This is a simple example in order to show the frames exchanged in 802.11 PCF.
 * The output prints the overal throughput as well as the number of different PCF frames that have
 * been transmitted.
 *
 * It is possible to tune some parameters using the command line:
 *   - number of connected stations
 *   - enable/disable PCF
 *   - enable PCAP output file generation in order to vizualise frame exchange.
 *   - configure UDP data traffic:
 *     -> enable/disable data generation: --withData=<0|1>
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
 *     -> enable/disable data generation: --withData=<0|1>
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
double m_innerCounter;
uint32_t nBss;
uint32_t nStations;
uint32_t nStations_net2;
Time m_sumDelay;
Time m_avgDelay;
Time m_sumTrueLatency;
Time m_avgTrueLatency;
bool enableCTStoSelf = true;
bool firstSensingPhase = true;
bool stillSensing = false;
bool multipleBss;
uint32_t nBfBss;
uint32_t nAxBss;

// Global variables for use in callbacks.
double g_signalDbmAvg; //!< Average signal power [dBm]
double g_noiseDbmAvg;  //!< Average noise power [dBm]
uint32_t g_samples;    //!< Number of samples

/**
 * Monitor sniffer Rx trace
 *
 * \param packet The sensed packet.
 * \param channelFreqMhz The channel frequency [MHz].
 * \param txVector The Tx vector.
 * \param aMpdu The aMPDU.
 * \param signalNoise The signal and noise dBm.
 * \param staId The STA ID.
 */
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
    if (addr == Mac48Address("00:00:00:00:00:01") && m_innerCounter < 1)
    {
        if (!AccessGranted)
        {
            m_sumTrueLatency = currentTime;
        }
        else
        {
            m_avgTrueLatency = m_avgTrueLatency + (currentTime - m_sumTrueLatency);
            m_innerCounter++;
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
    }
}

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

int
main(int argc, char* argv[])
{
    Time::SetResolution(Time::NS);
    /***********************************/
    // Parameter for PCF               //
    /***********************************/
    nBss = 1;
    nAxBss = 0;
    nStations = 4;
    nStations_net2 = 4;
    bool enablePcap = false;
    bool enableWiFiSensing = true;
    bool withData = true;
    multipleBss = true;
    bool downlink{true};
    uint64_t cfpMaxDurationMs = 10; // milliseconds
    double simulationTime = 5.0;    // seconds
    float radius = 2.0;             // meters
    float distance = 5.0;           // meters

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
    uint8_t nc = 2;          // Number of colums in the compressed beamforming feedback matrix
    uint8_t numAntennas = 1; // Number of rows in the compressed beamforming feedback matrix
    bool printChannelSoundingDuration = false; // Whether to print channel sounding duration
    uint8_t SoundingType = 2;                  // Sounding type (0: SU, 1: SU+MU, 2: MU)

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
    cmd.AddValue("enableWiFiSensing", "Enable/disable PCF mode", enableWiFiSensing);
    cmd.AddValue("withData", "Enable/disable UDP data packets generation", withData);
    cmd.AddValue("trafficDirection",
                 "Data traffic direction (if withData is set to 1): upstream (all STAs -> AP) or "
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
    cmd.AddValue("numAntennas",
                 "Number of rows in the compressed beamforming feedback matrix",
                 numAntennas);
    cmd.AddValue("nc", "Number of colums in the compressed beamforming feedback matrix", nc);
    cmd.AddValue("printChannelSoundingDuration",
                 "Whether to print channel sounding duration",
                 printChannelSoundingDuration);
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
    cmd.AddValue("distance", "Distance between two BSS", distance);
    cmd.AddValue("soundingtype", "Sounding type (0: SU, 1: SU+MU, 2: MU)", SoundingType);
    cmd.AddValue("frequency", "Frequency (2.4, 5, 6 GHz)", frequency);
    cmd.AddValue("nBss", "Number of BSS", nBss);
    cmd.AddValue("nAxBss", "Number of BSS for ax", nAxBss);
    cmd.AddValue("enableMuMIMO", "Enable/disable MU-MIMO", enableWiFiMuSensing);
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

    Bss default_Bss;
    default_Bss.nStations_sensing = nStations;
    default_Bss.nStations_no_sensing = nStations_net2;
    std::vector<Bss> allBss(nBss, default_Bss);

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
        nBfBss = nBss - nAxBss;
        // nBfBss = nBss / 2;
    }

    for (uint32_t i = 0; i < nBfBss; i++)
    {
        NodeContainer wifiStaNode, wifiApNode;
        wifiStaNode.Create(allBss[i].nStations_sensing);
        wifiApNode.Create(allBss[i].nAp);
        std::cout << i + 1 << ". BSS(bf) " << "has " << allBss[i].nStations_sensing << " stations "
                  << "and " << allBss[i].nAp << " AP" << std::endl;
        allBss[i].wifiStaNode = wifiStaNode;
        allBss[i].wifiApNode = wifiApNode;
    }
    for (uint32_t i = nBfBss; i < nBss; i++)
    {
        NodeContainer wifiStaNode_net2, WifiApNode_net2;
        wifiStaNode_net2.Create(allBss[i].nStations_no_sensing);
        WifiApNode_net2.Create(allBss[i].nAp);
        std::cout << i + 1 << ". BSS(ax) " << "has " << allBss[i].nStations_no_sensing
                  << " stations " << "and " << allBss[i].nAp << " AP" << std::endl;
        allBss[i].wifiStaNode_net2 = wifiStaNode_net2;
        allBss[i].WifiApNode_net2 = WifiApNode_net2;
    }

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

    Ptr<FriisPropagationLossModel> lossModel = CreateObject<FriisPropagationLossModel>();
    lossModel->SetFrequency(5.180e9);
    spectrumChannel->AddPropagationLossModel(lossModel);

    Ptr<ConstantSpeedPropagationDelayModel> delayModel =
        CreateObject<ConstantSpeedPropagationDelayModel>();
    spectrumChannel->SetPropagationDelayModel(delayModel);

    SpectrumWifiPhyHelper phy;
    phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    phy.SetChannel(spectrumChannel);
    phy.Set("ChannelSettings", StringValue(channelStr));
    phy.Set("TxPowerStart", DoubleValue(23));
    phy.Set("TxPowerEnd", DoubleValue(23));
    phy.Set("TxPowerLevels", UintegerValue(1));

    Ssid ssid = Ssid("wifi-bf-network");
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(ossDataMode.str()),
                                 "ControlMode",
                                 ctrlRate);
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

    if (nBfBss >= 0)
    {
        macAp.SetType("ns3::ApWifiMac",
                      "Ssid",
                      SsidValue(ssid),
                      "WiFiSensingSupported",
                      BooleanValue(enableWiFiSensing),
                      "CfpMaxDuration",
                      TimeValue(MilliSeconds(cfpMaxDurationMs)),
                      "CtsToSelfSupported",
                      BooleanValue(enableCTStoSelf),
                      "ChannelSoundingSupported",
                      BooleanValue(enableChannelSounding),
                      "QosSupported",
                      BooleanValue(true),
                      "BeaconGeneration",
                      BooleanValue(false),
                      "MuSensingSupported",
                      BooleanValue(enableWiFiMuSensing));
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
        for (uint32_t i = 0; i < nBfBss; i++)
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
                       TimeValue(MilliSeconds(cfpMaxDurationMs)),
                       "CtsToSelfSupported",
                       BooleanValue(enableCTStoSelf),
                       "ChannelSoundingSupported",
                       BooleanValue(enableChannelSounding),
                       "QosSupported",
                       BooleanValue(true),
                       "MuSensingSupported",
                       BooleanValue(enableWiFiMuSensing));
        for (uint32_t i = 0; i < nBfBss; i++)
        {
            allBss[i].staDevices = wifi.Install(phy, macSta, allBss[i].wifiStaNode);
        }
    }

    // For the second BSS
    if (multipleBss)
    {
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
        for (uint32_t i = nBfBss; i < nBss; i++)
        {
            allBss[i].apDevice_net2 = wifi.Install(phy, macAp_net2, allBss[i].WifiApNode_net2);
        }

        macSta_net2.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid));
        for (uint32_t i = nBfBss; i < nBss; i++)
        {
            allBss[i].staDevices_net2 = wifi.Install(phy, macSta_net2, allBss[i].wifiStaNode_net2);
        }
    }

    /* Mobility Settings */
    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    double x_baseAp = 0.0;
    double y_baseAp = 0.0;
    double baseAngle = 2.0 * M_PI / nBss;
    double rasioAngle = 0;
    if (multipleBss)
    {
        rasioAngle = 1 / (sin(baseAngle / 2)) + 1;
    }
    std::cout << "rasioAngle: " << rasioAngle << std::endl;
    for (uint32_t i = 0; i < nBss; i++)
    {
        x_baseAp = (rasioAngle * radius) * cos(baseAngle * i);
        y_baseAp = (rasioAngle * radius) * sin(baseAngle * i);
        positionAlloc->Add(Vector(x_baseAp, y_baseAp, 0.0));
        if (i < nBfBss)
        {
            float Angle = 2 * 3.14159265 / allBss[i].nStations_sensing;
            for (uint32_t j = 0; j < allBss[i].nStations_sensing; j++)
            {
                float x = radius * cos(Angle * j) + x_baseAp;
                float y = radius * sin(Angle * j) + y_baseAp;
                positionAlloc->Add(Vector(x, y, 0.0));
            }
        }
        else
        {
            float Angle = 2 * 3.14159265 / allBss[i].nStations_no_sensing;
            for (uint32_t j = 0; j < allBss[i].nStations_no_sensing; j++)
            {
                float x = radius * cos(Angle * j) + x_baseAp;
                float y = radius * sin(Angle * j) + y_baseAp;
                positionAlloc->Add(Vector(x, y, 0.0));
            }
        }
    }
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    for (uint32_t i = 0; i < nBss; i++)
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

    /* Internet Stack */
    InternetStackHelper stack;
    for (uint32_t i = 0; i < nBss; i++)
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

    Ipv4AddressHelper address, address_net2;
    Ipv4InterfaceContainer ApInterface, ApInterface_net2, StaInterface, StaInterface_net2;

    std::string ipBase = "192.1.";
    std::string ipBase_net2 = "192.168.";
    uint8_t ipCounter = 1;
    for (uint32_t i = 0; i < nBss; i++)
    {
        ipCounter++;
        if (i < nBfBss)
        {
            std::string string_ip = ipBase + std::to_string(ipCounter) + ".0";
            Ipv4Address ip = Ipv4Address(ipToInt(string_ip));
            address.SetBase(ip, "255.255.255.0");
            allBss[i].address.SetBase(ip, "255.255.255.0");
            allBss[i].ApInterface = address.Assign(allBss[i].apDevice);
            allBss[i].StaInterface = address.Assign(allBss[i].staDevices);
        }
        else
        {
            std::string string_ip = ipBase_net2 + std::to_string(ipCounter) + ".0";
            Ipv4Address ip = Ipv4Address(ipToInt(string_ip));
            address.SetBase(ip, "255.255.255.0");
            allBss[i].address_net2.SetBase(ip, "255.255.255.0");
            allBss[i].ApInterface_net2 = address.Assign(allBss[i].apDevice_net2);
            allBss[i].StaInterface_net2 = address.Assign(allBss[i].staDevices_net2);
        }
    }

    /* Setting Applications */
    ApplicationContainer serverApplications, clientApplications;
    ApplicationContainer serverApplications_net2, clientApplications_net2;
    if (withData)
    {
        uint32_t portNumber = 9;
        for (uint32_t i = 0; i < nBfBss; i++)
        {
            auto serverNodes =
                downlink ? std::ref(allBss[i].wifiStaNode) : std::ref(allBss[i].wifiApNode);
            Ipv4InterfaceContainer serverInterfaces;
            NodeContainer clientNodes;
            for (uint32_t index = 0; index < allBss[i].nStations_sensing; ++index)
            {
                serverInterfaces.Add(downlink ? allBss[i].StaInterface.Get(index)
                                              : allBss[i].ApInterface.Get(0));
                clientNodes.Add(downlink ? allBss[i].wifiApNode.Get(0)
                                         : allBss[i].wifiStaNode.Get(index));
            }

            // UDP flow
            UdpServerHelper server(portNumber);
            serverApplications_net2 = server.Install(serverNodes.get());
            serverApplications_net2.Start(Seconds(0.0));
            serverApplications_net2.Stop(Seconds(simulationTime + 1));

            for (uint32_t index = 0; index < allBss[i].nStations_sensing; ++index)
            {
                UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber);
                client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
                client.SetAttribute("Interval",
                                    TimeValue(Time("0.00001"))); // packets/s
                client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                clientApplications_net2 = client.Install(clientNodes.Get(index));
                clientApplications_net2.Start(Seconds(1.0));
                clientApplications_net2.Stop(Seconds(simulationTime + 1));
            }
        }

        uint32_t portNumber_net2 = 9;
        for (uint32_t i = nBfBss; i < nBss; i++)
        {
            /* Setting applications */
            auto serverNodes = downlink ? std::ref(allBss[i].wifiStaNode_net2)
                                        : std::ref(allBss[i].WifiApNode_net2);
            Ipv4InterfaceContainer serverInterfaces;
            NodeContainer clientNodes;
            for (uint32_t index = 0; index < allBss[i].nStations_no_sensing; ++index)
            {
                serverInterfaces.Add(downlink ? allBss[i].StaInterface_net2.Get(index)
                                              : allBss[i].ApInterface_net2.Get(0));
                clientNodes.Add(downlink ? allBss[i].WifiApNode_net2.Get(0)
                                         : allBss[i].wifiStaNode_net2.Get(index));
            }

            // UDP flow
            UdpServerHelper server(portNumber_net2);
            serverApplications_net2 = server.Install(serverNodes.get());
            serverApplications_net2.Start(Seconds(0.0));
            serverApplications_net2.Stop(Seconds(simulationTime + 1));

            for (uint32_t index = 0; index < allBss[i].nStations_no_sensing; ++index)
            {
                UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber_net2);
                client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
                // client.SetAttribute("Interval",
                //                     TimeValue(Time("0.00001"))); // packets/s
                client.SetAttribute("Interval",
                                    TimeValue(Time("0.00001"))); // packets/s
                client.SetAttribute("PacketSize", UintegerValue(payloadSize));
                clientApplications_net2 = client.Install(clientNodes.Get(index));
                clientApplications_net2.Start(Seconds(0.0));
                clientApplications_net2.Stop(Seconds(simulationTime + 1));
            }
        }
    }

    // Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyTxBegin",
    //                 MakeCallback(&TxCallback));

    // Config::ConnectWithoutContext(
    //     "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyRxEnd",
    //     MakeCallback(&RxEndCallback));

    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorChannelAccess",
        MakeCallback(&MonitorChannelAccess));

    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/MonitorSnifferRx",
        MakeCallback(&MonitorSniffRx));

    if (enablePcap)
    {
        if (multipleBss)
        {
            phy.EnablePcap("bf-wifi-network", allBss[nBfBss - 1].staDevices);
            // phy.EnablePcap("bf-wifi-network", allBss[nBfBss].staDevices_net2);
        }
        else
        {
            phy.EnablePcap("bf-wifi-network", allBss[0].staDevices);
        }
    }

    Simulator::Stop(Seconds(simulationTime + 1)); // simulationTime + 1
    Simulator::Run();

    double throughput = 0;
    double throughput_net2 = 0;
    uint8_t index = 0;
    uint64_t rxBytes = 0;
    if (multipleBss)
    {
        for (uint32_t index = 0; index < serverApplications.GetN(); index++)
        {
            rxBytes +=
                payloadSize * DynamicCast<UdpServer>(serverApplications.Get(index))->GetReceived();
            throughput += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
        }
        std::cout << "Overal throughput from 802.11bf BSS: " << throughput << " Mbit/s"
                  << std::endl;

        for (uint32_t index = 0; index < serverApplications_net2.GetN(); index++)
        {
            rxBytes += payloadSize *
                       DynamicCast<UdpServer>(serverApplications_net2.Get(index))->GetReceived();
        }
        throughput_net2 += ((rxBytes * 8) / (simulationTime * 1000000.0)); // Mbit/s
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
            rxBytes +=
                payloadSize * DynamicCast<UdpServer>(serverApplications.Get(index))->GetReceived();
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
    std::cout << "# average delay: " << m_avgDelay.GetSeconds() / m_innerCounter << std::endl;
    std::cout << "# average latency: " << m_avgTrueLatency.GetSeconds() / m_innerCounter
              << std::endl;
    std::cout << "# inner Counter: " << m_innerCounter << std::endl;
    std::cout << "# total latency: " << m_avgDelay.GetSeconds() << std::endl;

    Simulator::Destroy();
    return 0;
}
