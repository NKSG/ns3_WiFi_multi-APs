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
#include <time.h>
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
    void ReadChannelGain(const char* in_fileName, 
                         size_t in_apSize, size_t in_staSize);
    void ReadServeBy(const char* in_fileName, size_t in_apSize, 
                        size_t in_staSize);
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
    YansWifiChannelHelper m_wifiChannel2;
    PacketSocketAddress m_socket;
    InternetStackHelper m_stack;
    Ipv4InterfaceContainer m_staInterface;
    Ipv4InterfaceContainer m_apInterface;
    ApplicationContainer m_cbrApps;
    ApplicationContainer m_pingApps;
	  ApplicationContainer m_sinkApps;
    
    std::vector<std::vector<double> > m_readChannelGain;
    std::vector<int> m_serveBy;
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
MultipleAp::ReadChannelGain(const char* in_fileName, 
                       size_t in_apSize, size_t in_staSize)
{
  m_readChannelGain.resize(in_apSize+in_staSize);
  for(size_t i = 0; i < m_readChannelGain.size(); ++i) 
    m_readChannelGain[i].resize(in_apSize+in_staSize);
  std::ifstream in_channelGain;
  std::string line;
  in_channelGain.open(in_fileName);
  if(in_channelGain.is_open()){
    size_t countI = 0;
    while(std::getline(in_channelGain, line)){
      std::istringstream iss(line);
      std::string sub;
      size_t countJ = 0;
      while (iss>>sub){
        m_readChannelGain[countI][countJ] = std::atof(sub.c_str());
        ++countJ;
      }
      ++countI;
    }
    in_channelGain.close();
  }else{
    std::cerr << "[Error][MultipleAp::ReadChannelGain] Open file " << in_fileName << " fail!\n";
  }
  /*
  for(size_t i = 0; i < m_readChannelGain.size(); ++i){
    for(size_t j = 0; j < m_readChannelGain[i].size(); ++j){
      std::cout << m_readChannelGain[i][j] << " ";
    }
    std::cout << "\n";
  }
  */
}

void
MultipleAp::SetWifiMac()
{
  m_wifiMac = NqosWifiMacHelper::Default ();
  m_wifiPhy = YansWifiPhyHelper::Default ();
  // Create wifi channel
  Ptr<MatrixPropagationLossModel> lossModel = CreateObject<MatrixPropagationLossModel> ();
  for(int i=0; i<m_apNum; ++i){
    for(int j=i+1; j<m_apNum; ++j){  
      lossModel->SetLoss (m_ap.Get(i)->GetObject<MobilityModel>(), 
                          m_ap.Get(j)->GetObject<MobilityModel>(), 
                          m_readChannelGain[i][j]);
    }
  }    
  for(int i=0; i<m_staNum; ++i){
    for(int j=i+1; j<m_staNum; ++j){ 
      lossModel->SetLoss(m_stas.Get(i)->GetObject<MobilityModel>(), 
                         m_stas.Get(j)->GetObject<MobilityModel>(), 
                         m_readChannelGain[i+m_apNum][j+m_apNum]);
    }
  }
  for(int i=0; i<m_apNum; ++i){
    for(int j=0; j<m_staNum; ++j){ 
      lossModel->SetLoss(m_ap.Get(i)->GetObject<MobilityModel>(), 
                         m_stas.Get(j)->GetObject<MobilityModel>(), 
                         m_readChannelGain[i][j+m_apNum]);
    }
  }  
  m_wifiChannel = CreateObject <YansWifiChannel> ();
  m_wifiChannel->SetPropagationLossModel(lossModel);
  m_wifiChannel->SetPropagationDelayModel(CreateObject <ConstantSpeedPropagationDelayModel> ());
  m_wifiPhy.SetChannel(m_wifiChannel);
  m_wifiPhy.Set("EnergyDetectionThreshold", DoubleValue(-78.0));
  m_wifiPhy.Set("CcaMode1Threshold", DoubleValue(-78.0));
  m_wifiPhy.Set("TxPowerStart", DoubleValue(15.0));
  m_wifiPhy.Set("TxPowerEnd", DoubleValue(15.0));
  m_wifiPhy.Set("TxGain", DoubleValue(0));
  m_wifiPhy.Set("RxGain", DoubleValue(0));
  m_wifiPhy.Set("RxNoiseFigure", DoubleValue(7));
  // IP interface and MAC
  //m_wifi.SetRemoteStationManager ("ns3::ArfWifiManager");
  m_wifi.SetStandard (WIFI_PHY_STANDARD_80211b);
  m_wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager", 
                   "DataMode",StringValue ("DsssRate11Mbps"), 
                   "ControlMode",StringValue ("DsssRate1Mbps"));


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
  m_mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  m_mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");  
  for(int i = 0; i < m_apNum; ++i){
    m_mobility.Install(m_ap.Get(i));
  }  
  for(int i=0; i<m_staNum; ++i){
    m_mobility.Install(m_stas.Get(i));
  }
}


void
MultipleAp::ReadServeBy(const char* in_fileName, size_t in_apSize, 
                        size_t in_staSize)
{
  std::ifstream in_serveBy;
  std::string line;
  in_serveBy.open(in_fileName);
  if(in_serveBy.is_open()){
    while(std::getline(in_serveBy, line)){
      m_serveBy.push_back(std::atoi(line.c_str())-m_staNum-1);
    }
    in_serveBy.close();
  }else{
    std::cerr << "[Error][MultipleAp::ReadServeBy] Open file " << in_fileName << " fail!\n";
  }  
  size_t apIdxCount = 1;
  for(size_t i = 0; i < in_apSize; ++i){
    if(m_serveBy[i]==-2){
      m_serveBy[i] = apIdxCount;
      ++apIdxCount;
    }
  }
  /*
  for(size_t i = 0; i < m_serveBy.size(); ++i){
    std::cout << i << " is served by " << m_serveBy[i] << "\n";
  }
  */
}

void
MultipleAp::SetApp()
{  
  std::cout << "=== " << m_apNum << ", " << m_staNum << " ===\n";
  //Downlink
  for (int i = 0; i < m_staNum; ++i){
    //std::cout << m_staInterface.GetAddress(i) << " " 
    //  << m_apInterface.GetAddress(m_serveBy[i]) << "\n";
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(m_staInterface.GetAddress(i), 5010));    
    onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    // NS3's bug data rate and start time should be different
    std::ostringstream oss;
    oss << "20Mbps";
    onoff.SetAttribute("DataRate", StringValue(oss.str()));
    onoff.SetAttribute("PacketSize", UintegerValue (1024));
    onoff.SetAttribute("StartTime", TimeValue (Seconds (1.000000+std::rand()%100/100.0)));
    m_cbrApps.Add(onoff.Install(m_ap.Get(m_serveBy[i])));   
    m_cbrApps.Start(Seconds(1.0));
    m_cbrApps.Stop(Seconds(30));	
	
	  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", 
      InetSocketAddress(m_staInterface.GetAddress(i), 5010));
    m_sinkApps.Start(Seconds(1.0));
    m_sinkApps.Stop(Seconds(30));
	  m_sinkApps.Add(sinkHelper.Install(m_stas.Get(i)));
	
    // NS3's bug any data transmission is after echo transmission
    uint16_t echoPort = 9;
    UdpEchoClientHelper echoClientHelper(m_apInterface.GetAddress(m_serveBy[i]), echoPort);
    echoClientHelper.SetAttribute("MaxPackets", UintegerValue (1));
    echoClientHelper.SetAttribute("Interval", TimeValue (Seconds (0.001)));
    echoClientHelper.SetAttribute("PacketSize", UintegerValue (10));
    echoClientHelper.SetAttribute("StartTime", TimeValue (Seconds (0.001+i/20.0)));
    m_pingApps.Add(echoClientHelper.Install(m_stas.Get (i)));
  }
 
  // Uplink
  for (int i = 0; i < m_staNum; ++i){
    OnOffHelper onoff("ns3::UdpSocketFactory", InetSocketAddress(m_apInterface.GetAddress(m_serveBy[i]), 5011));    
    onoff.SetAttribute("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
    onoff.SetAttribute("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
    // NS3's bug data rate and start time should be different
    std::ostringstream oss;
    oss << "20Mbps";
    onoff.SetAttribute("DataRate", StringValue(oss.str()));
    onoff.SetAttribute("PacketSize", UintegerValue (1024));
    onoff.SetAttribute("StartTime", TimeValue (Seconds (1.000000+std::rand()%100/100.0)));
    m_cbrApps.Add(onoff.Install(m_stas.Get(i)));
    m_cbrApps.Start(Seconds(1.0));
    m_cbrApps.Stop(Seconds(30));	
	
	  PacketSinkHelper sinkHelper ("ns3::UdpSocketFactory", 
      InetSocketAddress(m_apInterface.GetAddress(m_serveBy[i]), 5011));
    m_sinkApps.Start(Seconds(1.0));
    m_sinkApps.Stop(Seconds(30));
	  m_sinkApps.Add(sinkHelper.Install(m_ap.Get(m_serveBy[i])));
	
    // NS3's bug any data transmission is after echo transmission
    uint16_t echoPort = 10;
    UdpEchoClientHelper echoClientHelper (m_staInterface.GetAddress(i), echoPort);
    echoClientHelper.SetAttribute ("MaxPackets", UintegerValue (1));
    echoClientHelper.SetAttribute ("Interval", TimeValue (Seconds (0.001)));
    echoClientHelper.SetAttribute ("PacketSize", UintegerValue (10));
    echoClientHelper.SetAttribute ("StartTime", TimeValue (Seconds (0.001+i/20.0)));
    m_pingApps.Add (echoClientHelper.Install (m_ap.Get(m_serveBy[i])));
  }
}

void
MultipleAp::Run()
{
  double totalThroughput = 0;
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll ();
  Simulator::Stop (Seconds (32.0));  
  Simulator::Run ();
  // Flow monitor for throughput etc. information
  monitor->CheckForLostPackets ();
  Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin ();
       i != stats.end (); ++i){
      Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      std::cout << "Flow " << i->first << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
      std::cout << " Tx Bytes: " << i->second.txBytes << ", ";
      std::cout << " Rx Bytes: " << i->second.rxBytes << ", ";
      std::cout << " Throughput: " << i->second.rxBytes*8.0/29.0/1024/1024 << " Mbps\n";
      totalThroughput+=(i->second.rxBytes*8.0/29.0/1024/1024);
  }
  Simulator::Destroy ();
  std::cout << "Total throughput: " << totalThroughput << "\n";
  std::cout << "======================\n";  
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
  std::string min_cw = "31";
  std::string max_cw = "1023";
  RngSeedManager::SetSeed (time(NULL));  // Changes seed from default of 1 to 3
  RngSeedManager::SetRun (time(NULL));   // Changes run number from default of 1 to 7
  Config::SetDefault ("ns3::Dcf::MinCw",StringValue(min_cw));
  Config::SetDefault ("ns3::Dcf::MaxCw",StringValue(max_cw));
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
  myExp.SetMobility();
  myExp.ReadChannelGain("./scratch/ChannelGain.txt", apNum, staNum);
  myExp.ReadServeBy("./scratch/ServeBy.txt", apNum, staNum);
  myExp.SetWifiMac();
  myExp.SetApp();
  myExp.Run();
  return 0;
}
