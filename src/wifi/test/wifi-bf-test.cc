/*
 * Copyright (c) 2025 RWTH Aachen University
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
 * Authors: Ido Manuel Lumbantobing <ido.lumbantobing@rwth-aachen.de>
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

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiBfTest");

/**
 * \ingroup wifi-test
 * \ingroup tests
 *
 * \brief Wifi Test
 *
 * This test case validate the sequence of frame exchange between one AP and one STA using 802.11bf
 * The check function checks the these sequence of frames exchanged:
 * 
 * +-----------+    +---------+    +---------+    +---------+
 * | BEACONING | -> | POLLING | -> |SOUNDING | -> |REPORTING|
 * +-----------+    +---------+    +---------+    +---------+
 * 
 * The topology for this test case is made of on networks, each with one AP and one STA:
 *
 *  AP  --d1--  STA1
 *  TX1         RX1
 * 
 * Main parameters:
 *  Maximum CFP duration = 50 milliseconds
 *  Sensing interval = 50 milliseconds
 *  PHY = 11ax, 5GHz, 20MHz
 *  Distance d1 = 2 meters
 */

class TestSingleStaSensing : public TestCase
{
  public:
    /**
     * Constructor
     *
     */
    TestSingleStaSensing();
    ~TestSingleStaSensing() override;

    void DoRun() override;

  private:
    /**
     * Allocate the node positions
     * \param d1 distance d1 (in meters)
     * \param d2 distance d2 (in meters)
     * \param d3 distance d3 (in meters)
     * \param d4 distance d4 (in meters)
     * \param d5 distance d5 (in meters)
     * \return the node positions
     */
    Ptr<ListPositionAllocator> AllocatePositions(double d1);

    /**
     * Check the results
     * \param hdr the WifiMacHeader to check
     * \param transmit true if the packet was transmitted, false if it was received
     */
    void CheckResults(WifiMacHeader hdr, bool transmit);

    /**
     * Reset the results
     */
    void ResetResults();

    /**
     * Run one function
     */
    void RunOne();

    /**
     * Check the received packet
     * \param packet the packet
     */
    void RxEndCallback(Ptr<const Packet> packet);

    /**
     * Check the transmitted packet
     * \param packet the packet
     */
    void TxCallback(std::string context, Ptr<const Packet> packet, double txPowerW);

    NetDeviceContainer m_staDevices; ///< STA devices
    NetDeviceContainer m_apDevices;  ///< AP devices

    double m_d1;               ///< Distance d1 (in meters)
    double m_simulationTime;   ///< Simulation time (in seconds)
    uint64_t cfpMaxDurationMs; // milliseconds
    uint64_t sensingInterval;  // milliseconds

    int sensingPhase;         ///< Sensing phase
    double m_frequency;     // Default frequency in GHz
    uint8_t m_numberOfStaDevices; ///< Number of STA devices

    bool transmitSuccess; ///< Flag to indicate if transmission was successful
    bool receiveSuccess;  ///< Flag to indicate if reception was successful
};

TestSingleStaSensing::TestSingleStaSensing()
    : TestCase("SingleStaSensing"),
      m_staDevices(),
      m_apDevices(),
      m_d1(2.0),
      cfpMaxDurationMs(50),
      sensingInterval(50),
      sensingPhase(0),
      m_simulationTime(1.0),
      transmitSuccess(false),
      receiveSuccess(false),
      m_frequency(5.0),
      m_numberOfStaDevices(1) 
{
}

TestSingleStaSensing::~TestSingleStaSensing()
{
}

Ptr<ListPositionAllocator>
TestSingleStaSensing::AllocatePositions(double d1)
{
    Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();
    positionAlloc->Add(Vector(0.0, 0.0, 0.0)); // AP1
    positionAlloc->Add(Vector(d1, 0.0, 0.0));  // STA1
    return positionAlloc;
}

void
TestSingleStaSensing::ResetResults()
{
    transmitSuccess = false;
    receiveSuccess = false;
    sensingPhase = 0; // Reset to the initial phase
}

void
TestSingleStaSensing::CheckResults(WifiMacHeader hdr, bool transmit)
{
    WifiMacHeader hdrToCheckAp1;
    WifiMacHeader hdrToCheckSta1;

    switch (sensingPhase)
    {
    case 0: // DL Beacon Phase: MGT_BEACON
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsBeacon(), true, "AP should transmit Beacon frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsBeacon(), true, "STA should receive Beacon frame");
            receiveSuccess = true;
        }
        break;
    case 1: // DL Sensing Phase: QOSDATA CFPOLL
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsCfPoll(), true, "AP should transmit QOSDATA CFPOLL frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsCfPoll(), true, "STA should receive QOSDATA CFPOLL frame");
            receiveSuccess = true;
        }
        break;

    case 2: // DL Polling Phase: Polling Trigger Frame
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsTrigger(), true, "AP should transmit Trigger frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsTrigger(), true, "STA should receive Trigger frame");
            receiveSuccess = true;
        }
        break;

    case 3: // UL Polling Phase: CTS-to-Self
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsCts(), true, "AP should transmit CTS frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsCts(), true, "STA should receive CTS frame");
            receiveSuccess = true;
        }
        break;

    case 4: // DL NDPA Phase: NDPA frame
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsNdpa(), true, "AP should transmit NDPA frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsNdpa(), true, "STA should receive NDPA frame");
            receiveSuccess = true;
        }
        break;

    case 5: // DL NDP Phase: NDP frame
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsNdp(), true, "AP should transmit NDP frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdr.IsNdp(), true, "STA should receive NDP frame");
            receiveSuccess = true;
        }
        break;

    case 6: // UL Reporting Phase: Reporting frame
        if (transmit)
        {
            NS_TEST_ASSERT_MSG_EQ(hdrToCheckSta1.IsActionNoAck(),
                                  true,
                                  "AP should transmit Action NoAck frame");
            transmitSuccess = true;
        }
        else
        {
            NS_TEST_ASSERT_MSG_EQ(hdrToCheckAp1.IsActionNoAck(),
                                  true,
                                  "STA should receive Action NoAck frame");
            receiveSuccess = true;
        }
        break;

    default:
        NS_TEST_ASSERT_MSG_EQ(false, true, "Invalid sensing phase");
        break;
    }
    // Advance phase when both TX and RX are successful
    if (transmitSuccess && receiveSuccess)
    {
        sensingPhase = (sensingPhase + 1) % 7; // Cycle through 0-6
        transmitSuccess = false;
        receiveSuccess = false;
    }
}

void
TestSingleStaSensing::RxEndCallback(Ptr<const Packet> packet)
{
    WifiMacHeader hdr;
    packet->PeekHeader(hdr);
    if (Simulator::Now().GetSeconds() > 1.0)
    {
        CheckResults(hdr, false);
    }
}

void
TestSingleStaSensing::TxCallback(std::string context, Ptr<const Packet> packet, double txPowerW)
{
    WifiMacHeader hdr;
    packet->PeekHeader(hdr);
    if (Simulator::Now().GetSeconds() > 1.0)
    {
        CheckResults(hdr, true);
    }
}

void
TestSingleStaSensing::RunOne()
{
    ResetResults();
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(1);

    NodeContainer wifiApNodes;
    wifiApNodes.Create(1);

    std::string channelStr("{0, " + std::to_string(20) + ", ");
    StringValue ctrlRate;
    Ptr<MultiModelSpectrumChannel> spectrumChannel = CreateObject<MultiModelSpectrumChannel>();
    auto nonHtRefRateMbps = HePhy::GetNonHtReferenceRate(6) / 1e6;

    std::ostringstream ossDataMode;
    ossDataMode << "HeMcs" << 6;
    if (m_frequency == 6)
    {
        ctrlRate = StringValue(ossDataMode.str());
        channelStr += "BAND_6GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue(48));
    }
    else if (m_frequency == 5)
    {
        std::ostringstream ossControlMode;
        ossControlMode << "OfdmRate" << nonHtRefRateMbps << "Mbps";
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_5GHZ, 0}";
    }
    else if (m_frequency == 2.4)
    {
        std::ostringstream ossControlMode;
        ossControlMode << "ErpOfdmRate" << nonHtRefRateMbps << "Mbps";
        ctrlRate = StringValue(ossControlMode.str());
        channelStr += "BAND_2_4GHZ, 0}";
        Config::SetDefault("ns3::LogDistancePropagationLossModel::ReferenceLoss", DoubleValue(40));
    }

    // Set the default Acknowledgment mode to NONE
    Config::SetDefault("ns3::WifiDefaultAckManager::DlMuAckSequenceType",
                       EnumValue(WifiAcknowledgment::NONE));
    SpectrumWifiPhyHelper phy;
    phy.SetPcapDataLinkType(WifiPhyHelper::DLT_IEEE802_11_RADIO);
    phy.SetChannel(spectrumChannel);
    phy.Set("ChannelSettings", StringValue(channelStr));
    phy.Set("TxPowerStart", DoubleValue(23.0));
    phy.Set("TxPowerEnd", DoubleValue(23.0));
    phy.Set("TxPowerLevels", UintegerValue(1));

    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211bf);
    wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager",
                                 "DataMode",
                                 StringValue(ossDataMode.str()),
                                 "ControlMode",
                                 ctrlRate);

    Ssid ssid = Ssid("wifi-bf-network");
    WifiMacHelper macAp;
    macAp.SetType("ns3::ApWifiMac",
                  "Ssid",
                  SsidValue(ssid),
                  "WiFiSensingSupported",
                  BooleanValue(1),
                  "CfpMaxDuration",
                  TimeValue(MicroSeconds(cfpMaxDurationMs * 1024)),
                  "CtsToSelfSupported",
                  BooleanValue(1),
                  "ChannelSoundingSupported",
                  BooleanValue(1),
                  "QosSupported",
                  BooleanValue(true),
                  "BeaconGeneration",
                  BooleanValue(true),
                  "SensingPriority",
                  UintegerValue(0),
                  "SensingInterval",
                  TimeValue(MilliSeconds(sensingInterval)));

    macAp.SetMultiUserScheduler("ns3::RrMultiUserScheduler",
                                "ChannelSoundingInterval",
                                TimeValue(MilliSeconds(sensingInterval)),
                                "EnableUlOfdma",
                                BooleanValue(1),
                                "NStations",
                                UintegerValue(1),
                                "SoundingType",
                                UintegerValue(0));
    m_apDevices = wifi.Install(phy, macAp, wifiApNodes);

    WifiMacHelper macSta;
    macSta.SetType("ns3::StaWifiMac",
                   "Ssid",
                   SsidValue(ssid),
                   "WiFiSensingSupported",
                   BooleanValue(1),
                   "CfpMaxDuration",
                   TimeValue(MicroSeconds(cfpMaxDurationMs * 1024)),
                   "CtsToSelfSupported",
                   BooleanValue(1),
                   "QosSupported",
                   BooleanValue(true));
    m_staDevices = wifi.Install(phy, macSta, wifiStaNodes);

    MobilityHelper mobility;
    Ptr<ListPositionAllocator> positionAlloc = AllocatePositions(m_d1);
    mobility.SetPositionAllocator(positionAlloc);
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(wifiApNodes);
    mobility.Install(wifiStaNodes);

    /* Internet Stack */
    InternetStackHelper stack;
    stack.Install(wifiApNodes);
    stack.Install(wifiStaNodes);

    Ipv4AddressHelper address;
    Ipv4InterfaceContainer ApInterface, StaInterface;
    address.SetBase("192.168.1.0", "255.255.255.0");
    ApInterface = address.Assign(m_apDevices);
    StaInterface = address.Assign(m_staDevices);

    uint32_t portNumber = 9;
    auto serverNodes = std::ref(wifiStaNodes);
    Ipv4InterfaceContainer serverInterfaces;
    NodeContainer clientNodes;
    ApplicationContainer serverApplications, clientApplications;

    for (uint32_t index = 0; index < wifiStaNodes.GetN(); ++index)
    {
        serverInterfaces.Add(StaInterface.Get(index));
        clientNodes.Add(wifiApNodes.Get(0));
    }

    // UDP flow
    UdpServerHelper server(portNumber);
    serverApplications = server.Install(serverNodes.get());
    serverApplications.Start(Seconds(0.0));
    serverApplications.Stop(Seconds(m_simulationTime + 1));

    for (uint32_t index = 0; index < wifiStaNodes.GetN(); ++index)
    {
        UdpClientHelper client(serverInterfaces.GetAddress(index), portNumber);
        client.SetAttribute("MaxPackets", UintegerValue(4294967295U));
        client.SetAttribute("Interval",
                            TimeValue(Time("1"))); // packets/s;
        client.SetAttribute("PacketSize", UintegerValue(700));
        clientApplications = client.Install(clientNodes.Get(index));
        clientApplications.Start(Seconds(1.0));
        clientApplications.Stop(Seconds(m_simulationTime + 1));
    }

    Config::Connect("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyTxBegin",
                    MakeCallback(&TestSingleStaSensing::TxCallback, this));

    Config::ConnectWithoutContext(
        "/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/$ns3::WifiPhy/PhyRxEnd",
        MakeCallback(&TestSingleStaSensing::RxEndCallback, this));

    Simulator::Stop(Seconds(m_simulationTime + 1));
    Simulator::Run();
    Simulator::Destroy();
}

void
TestSingleStaSensing::DoRun()
{
    m_frequency = 2.4; // Default frequency in GHz
    RunOne();

    m_frequency = 5.0; // Default frequency in GHz
    RunOne();

    m_frequency = 6.0; // Default frequency in GHz
    RunOne();

    for (double d1 = 1.0; d1 <= 100.0; d1 += 1.0)
    {
        m_d1 = d1;
        RunOne();
    }
}

/**
 * \ingroup wifi-test
 * \ingroup tests
 *
 * \brief IEEE 802.11bf Test Suite
 */

class WifiBfTestSuite : public TestSuite
{
  public:
    WifiBfTestSuite();
};

WifiBfTestSuite::WifiBfTestSuite()
    : TestSuite("wifi-bf", PERFORMANCE)
{
    AddTestCase(new TestSingleStaSensing(), TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static WifiBfTestSuite g_WifiBfTestSuite;
