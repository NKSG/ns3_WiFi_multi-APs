/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
* Copyright (c) 2005,2006,2007 INRIA
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation;
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* Original Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
* Modified Author: Alvin Hsu <alvin1019127@gmail.com>
*/


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/propagation-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/athstats-helper.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/internet-module.h"
#include <iostream>

#define PI 3.14159265
#define PI_e5 314158

using namespace ns3;
class MultipleAp
{
  public:
    MultipleAp();
    void SetApStaNumber(int in_apNum, int in_staNum);
    void SetWifiMac();
    void SetMobility();
    void SetApp();
    void Run();
    void DevTxTrace (std::string context, Ptr<const Packet> p);
    void DevRxTrace (std::string context, Ptr<const Packet> p);
    void PhyRxOkTrace (std::string context, Ptr<const Packet> packet,
              double snr, WifiMode mode, enum WifiPreamble preamble);
    void PhyRxErrorTrace (std::string context, Ptr<const Packet> packet, double snr);
    void PhyTxTrace (std::string context, Ptr<const Packet> packet,
            WifiMode mode, WifiPreamble preamble, uint8_t txPower);
    void PhyStateTrace (std::string context, Time start, Time duration, enum WifiPhy::State state);
  private:
    int m_apNum;
    int m_staNum;
    int m_rxOkCount;
    double m_bytesTotal;
    
    WifiHelper m_wifi ;
    MobilityHelper m_mobility;
    NodeContainer m_stas;
    NodeContainer m_ap;
    NetDeviceContainer m_staDevs;
    NetDeviceContainer m_apDevs;
    PacketSocketHelper m_packetSocket;
    NqosWifiMacHelper m_wifiMac;
    YansWifiPhyHelper m_wifiPhy;
    Ptr<YansWifiChannel> m_wifiChannel;
    PacketSocketAddress m_socket;
    InternetStackHelper m_stack;
    Ipv4InterfaceContainer m_staInterface;
    Ipv4InterfaceContainer m_apInterface;
    ApplicationContainer m_cbrApps;
    ApplicationContainer m_pingApps;
    ApplicationContainer m_sinkApps;
};

MultipleAp::MultipleAp()
{
  // constructor
  m_wifi = WifiHelper::Default ();
  m_rxOkCount = 0;
  m_bytesTotal = 0;
}

void
MultipleAp::SetApStaNumber(int in_apNum, int in_staNum)
{
  m_apNum = in_apNum;
  m_staNum = in_staNum;
  m_stas.Create(m_staNum);
  m_ap.Create(m_apNum);
}

void
MultipleAp::SetWifiMac()
{
  m_wifiMac = NqosWifiMacHelper::Default ();
  m_wifiPhy = YansWifiPhyHelper::Default ();
  // Create wifi channel
  
  Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel> ();
  lossModel->SetDefaultLoss (0);
  m_wifiChannel = CreateObject <YansWifiChannel> ();
  m_wifiChannel->SetPropagationLossModel (lossModel);
  m_wifiChannel->SetPropagationDelayModel (CreateObject <ConstantSpeedPropagationDelayModel> ());
  m_wifiPhy.SetChannel (m_wifiChannel);
  // IP interface and MAC
  m_wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  m_stack.Install (m_ap);
  m_stack.Install (m_stas);
  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.0.0.0", "255.0.0.0");
  // setup ap
  for (int i = 0; i < m_apNum; ++i) {
    
    m_wifiMac.SetType ("ns3::AdhocWifiMac");
    m_apDevs.Add(m_wifi.Install (m_wifiPhy, m_wifiMac, m_ap.Get(i)));
    m_apInterface.Add(ipv4.Assign(m_apDevs.Get(i)));
  }
  // setup stas and the number of stas is average in every Ssid
  for (int i = 0; i < m_staNum; ++i) {
    m_wifiMac.SetType ("ns3::AdhocWifiMac");
    m_staDevs.Add(m_wifi.Install (m_wifiPhy, m_wifiMac, m_stas.Get(i)));
    m_staInterface.Add(ipv4.Assign(m_staDevs.Get(i)));
  }
}

void
MultipleAp::SetMobility()
{
  // the position is used for --visualize, in fact no hidden terminals
  Ptr<ListPositionAllocator> m_apPosAlloc;
  Ptr<ListPositionAllocator> m_nodePosAlloc;
  m_mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  m_mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
  m_apPosAlloc = CreateObject<ListPositionAllocator> ();
  m_nodePosAlloc = CreateObject<ListPositionAllocator> ();
  double m_radius = 50;
  for(int i=0; i<m_apNum; ++i){
    m_apPosAlloc->Add(Vector(m_radius*std::cos(i*2*PI/m_apNum),
                      m_radius*std::sin(i*2*PI/m_apNum), 1));
  }
  m_mobility.SetPositionAllocator(m_apPosAlloc);
  for(int i=0; i<m_apNum; ++i){
    m_mobility.Install(m_ap.Get(i));
  }
  
  for(int i=0; i<m_staNum; ++i){
   size_t inAp = i/(m_staNum/m_apNum);
   int serveNum = m_staNum/m_apNum;
   double nodeRadius = 20;
   m_nodePosAlloc->Add(Vector(m_radius*std::cos(inAp*2*PI/m_apNum)+
                       nodeRadius*std::cos(2*PI/serveNum*(i%serveNum)),
                       m_radius*std::sin(inAp*2*PI/m_apNum)+
                       nodeRadius*std::sin(2*PI/serveNum*(i%serveNum)),
                       1));
  }
  m_mobility.SetPositionAllocator(m_nodePosAlloc);
  for(int i=0; i<m_staNum; ++i){
    m_mobility.Install(m_stas.Get(i));
  }
}



void
MultipleAp::SetApp()
{
  int serveNum = m_staNum/m_apNum;
  std::cout << "=== " << m_apNum << ", " << m_staNum << " ===\n";
  
  //Downlink
  for (int i = 0; i < m_staNum; ++i){
    //std::cout << m_staInterface.GetAddress(i) << ":" <<
    // m_apInterface.GetAddress(static_cast<int>(i/serveNum)) << "\n";
    // Using poisson arrival traffic as onoff application
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(m_staInterface.GetAddress(i), 5010));    
    //onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.005]"));
    //onoff.SetAttribute("OffTime", StringValue ("ns3::ExponentialRandomVariable[Mean=0.05]"));
    onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    
    PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", InetSocketAddress(m_staInterface.GetAddress(i), 5010));
    onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    // NS3's bug data rate and start time should be different
    std::ostringstream oss;
    oss << "20Mbps";
    onoff.SetAttribute("DataRate", StringValue(oss.str()));
    onoff.SetAttribute("PacketSize", UintegerValue (1024));
    onoff.SetAttribute("StartTime", TimeValue (Seconds (1.000000+std::rand()%100/100.0)));
    m_cbrApps.Add(onoff.Install(m_ap.Get(static_cast<int>(i/serveNum))));
    
    m_sinkApps.Add (sinkHelper.Install (m_stas.Get (i)));
    m_cbrApps.Start(Seconds(1.0));
    m_cbrApps.Stop(Seconds(10));
    // NS3's bug any data transmission is after echo transmission
    uint16_t echoPort = 9;
    UdpEchoClientHelper echoClientHelper (m_apInterface.GetAddress(static_cast<int>(i/serveNum)), echoPort);
    echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.001)));
    echoClientHelper.SetAttribute ("PacketSize", UintegerValue (10));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.001+i/20.0)));
    m_pingApps.Add (echoClientHelper.Install (m_stas.Get (i)));
  }
  
  // Uplink
  for (int i = 0; i < m_staNum; ++i){
    //std::cout << m_staInterface.GetAddress(i) << ":" <<
    // m_apInterface.GetAddress(static_cast<int>(i/serveNum)) << "\n";
    // Using poisson arrival traffic as onoff application
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(m_apInterface.GetAddress(static_cast<int>(i/serveNum)), 5010));    
    //onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=0.005]"));
    //onoff.SetAttribute("OffTime", StringValue ("ns3::ExponentialRandomVariable[Mean=0.05]"));
    onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    
    PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", InetSocketAddress(m_apInterface.GetAddress(static_cast<int>(i/serveNum)), 5010));
    onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    // NS3's bug data rate and start time should be different
    std::ostringstream oss;
    oss << "20Mbps";
    onoff.SetAttribute("DataRate", StringValue(oss.str()));
    onoff.SetAttribute("PacketSize", UintegerValue (1024));
    onoff.SetAttribute("StartTime", TimeValue (Seconds (1.000000+std::rand()%100/100.0)));
    m_cbrApps.Add(onoff.Install(m_stas.Get(i)));
    
    m_sinkApps.Add (sinkHelper.Install (m_ap.Get(static_cast<int>(i/serveNum))));
    m_cbrApps.Start(Seconds(1.0));
    m_cbrApps.Stop(Seconds(10));
    // NS3's bug any data transmission is after echo transmission
    uint16_t echoPort = 9;
    UdpEchoClientHelper echoClientHelper (m_staInterface.GetAddress(i), echoPort);
    echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.001)));
    echoClientHelper.SetAttribute ("PacketSize", UintegerValue (10));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.001+i/20.0)));
    m_pingApps.Add (echoClientHelper.Install (m_ap.Get(static_cast<int>(i/serveNum))));
  }
}

void
MultipleAp::Run()
{
  double totalThroughput = 0;
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  Simulator::Stop (Seconds (12.0));
  //Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxOk",
  // MakeCallback (&MultipleAp::PhyRxOkTrace, this));
  //Config::Connect ("/NodeList/*/DeviceList/*/Phy/State/RxError",
  // MakeCallback (&MultipleAp::PhyRxErrorTrace, this));
  //
  Simulator::Run ();
  // Flow monitor for throughput etc. information
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin ();
       i != stats.end (); ++i){
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
      std::cout << " Tx Bytes: " << i->second.txBytes << "\n";
      std::cout << " Rx Bytes: " << i->second.rxBytes << "\n";
      std::cout << " Throughput: " << i->second.rxBytes*8.0/9.0/1024/1024 << " Mbps\n";
      totalThroughput+=(i->second.rxBytes*8.0/9.0/1024/1024);
  }
  Simulator::Destroy ();
  std::cout << "Total throughput: " << totalThroughput << "\n";
  std::cout << "======================\n";
  //std::cout << "Average throughput per AP: " << totalThroughput/m_apNum << "\n";
  //std::cout << "Average throughput per sta: " << totalThroughput/m_staNum << "\n";
  //std::cout << m_rxOkCount << " " << m_bytesTotal << "\n";
}

void
MultipleAp::DevTxTrace (std::string context, Ptr<const Packet> p)
{
  
}
void
MultipleAp::DevRxTrace (std::string context, Ptr<const Packet> p)
{
  
}

void
MultipleAp::PhyRxOkTrace (std::string context, Ptr<const Packet> packet,
              double snr, WifiMode mode, enum WifiPreamble preamble)
{
  Ptr<Packet> m_currentPacket;
  WifiMacHeader hdr;
  double currentByte = packet->GetSize();
  m_currentPacket = packet->Copy();
  m_currentPacket->RemoveHeader (hdr);
  if(hdr.IsData()){
    m_rxOkCount++;
    m_bytesTotal+=currentByte;
  }
}

void
MultipleAp::PhyRxErrorTrace (std::string context, Ptr<const Packet> packet, double snr)
{
}

void
MultipleAp::PhyTxTrace (std::string context, Ptr<const Packet> packet,
            WifiMode mode, WifiPreamble preamble, uint8_t txPower)
{
}

void
MultipleAp::PhyStateTrace (std::string context, Time start,
             Time duration, enum WifiPhy::State state)
{
}

int main (int argc, char *argv[])
{
  CommandLine cmd;
  int apNum;
  int staNum;
  // ./waf --run="scratch/wifi-ap --apNum=2 --staNum=8" --visualize
  //cmd.Usage ("[apNum] [staNum]");
  cmd.AddValue ("apNum", "number of WiFi APs", apNum);
  cmd.AddValue ("staNum", "number of STAs", staNum);
  cmd.Parse (argc, argv);
  Packet::EnablePrinting ();
  // control rts/cts
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("20000"));
  // disable fragmentation
  Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
  MultipleAp myExp;
  myExp.SetApStaNumber(apNum, staNum);
  myExp.SetWifiMac();
  myExp.SetMobility();
  myExp.SetApp();
  myExp.Run();
  return 0;
}
