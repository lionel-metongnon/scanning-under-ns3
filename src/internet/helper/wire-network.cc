/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015 Universite catholique de Louvain
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
 * Author: Lionel Metongnon <lionel.metongnon@uclouvain.be>
 */

#include "ns3/core-module.h"
#include "ns3/bridge-module.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/log.h"
#include "wire-network.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("WireNetwork");

CsmaNetwork::CsmaNetwork(std::string dataRate, std::string delay, uint32_t nbrOfNodes)
: Network(nbrOfNodes)
{
  NS_LOG_FUNCTION (this);
  m_bridge = NULL;
  if (nbrOfNodes > 1)
    {
      m_bridge = CreateObject<Node> ();
    }
  m_dataRate = dataRate;
  m_delay = delay;
}
CsmaNetwork::CsmaNetwork(std::string dataRate, std::string delay, NodeContainer nodes)
: Network(nodes)
{
  NS_LOG_FUNCTION (this);
  m_bridge = NULL;
  if (nodes.GetN () > 1)
    {
      m_bridge = CreateObject<Node> ();
    }
  m_dataRate = dataRate;
  m_delay = delay;
}

void 
CsmaNetwork::SetMobility(std::vector<double> xy, bool isNodeMobile)
{
  NS_LOG_FUNCTION (this);
}

void 
CsmaNetwork::ConfigureL2() 
{
  NS_LOG_FUNCTION (this << m_dataRate << m_delay);
  if (m_nodes.GetN () < 2)
    {
      m_csmaHelper.SetChannelAttribute ("DataRate", StringValue (m_dataRate));
      m_csmaHelper.SetChannelAttribute ("Delay", StringValue (m_delay));
      m_netDevices = m_csmaHelper.Install (NodeContainer (m_router, m_nodes));   
    }
  else
  {
    NetDeviceContainer bridgeNetDevices;
    NodeContainer all(m_router);
    all.Add (m_nodes);
    for (uint32_t i = 0; i < all.GetN (); ++i)
      {
        NetDeviceContainer link = m_csmaHelper.Install (NodeContainer (all.Get (i), m_bridge));
        m_netDevices.Add (link.Get (0));
        bridgeNetDevices.Add (link.Get (1));
      }
    BridgeHelper bridge; 
    bridge.Install (m_bridge, bridgeNetDevices);
  }
}

void
CsmaNetwork::EnablePcap(std::string pcapFileName, bool promiscious)
{
  NS_LOG_FUNCTION (this << pcapFileName << promiscious);
  m_csmaHelper.EnablePcapAll (pcapFileName+"_csma", promiscious);
} 

void 
CsmaNetwork::EnablePcapRouter(std::string pcapFileName)
{
  NS_LOG_FUNCTION (this << pcapFileName);
  m_csmaHelper.EnablePcap (pcapFileName+"_csma", m_router->GetDevice (0));
}

void 
CsmaNetwork::EnableAscii(std::string fileName)
{
  NS_LOG_FUNCTION (this << fileName);
  AsciiTraceHelper ascii;
  m_csmaHelper.EnableAsciiAll (ascii.CreateFileStream (fileName+"_csma.tr"));
}

CsmaNetwork::~CsmaNetwork()
{
  NS_LOG_FUNCTION (this);
}

RouterCsmaNetwork::RouterCsmaNetwork(Ptr <Node> routerA, Ptr <Node> routerB, std::string dataRate, std::string delay)
: Network()
{
  NS_LOG_FUNCTION (this);
  m_bridge = NULL;
  m_nodes.Add (routerA);
  m_nodes.Add (routerB);
  m_router = m_nodes.Get (0);
  m_dataRate = dataRate;
  m_delay = delay;
  m_index = routerA->GetNDevices ();
}

RouterCsmaNetwork::RouterCsmaNetwork(Ptr <Node> routerA, std::string dataRate, std::string delay)
: Network()
{
  NS_LOG_FUNCTION (this);
  m_bridge = NULL;
  m_nodes.Create(1);
  m_nodes.Add (routerA);
  m_router = m_nodes.Get (0);
  m_dataRate = dataRate;
  m_delay = delay;
  m_index = 0;
}

RouterCsmaNetwork::RouterCsmaNetwork(NodeContainer nodes, std::string dataRate, std::string delay)
: Network()
{
  NS_LOG_FUNCTION (this);
  m_bridge = NULL;
  if (nodes.GetN () > 1)
    {
      m_bridge = CreateObject<Node> ();
    }
  m_nodes.Create(1);
  m_nodes.Add (nodes);
  m_router = m_nodes.Get (0);
  m_dataRate = dataRate;
  m_delay = delay;
  m_index = 0;
}

void 
RouterCsmaNetwork::SetMobility(std::vector<double> xy, bool isNodeMobile)
{
  NS_LOG_FUNCTION (this);
}

void 
RouterCsmaNetwork::EnableRouting (Ipv6ListRoutingHelper &listRH, RipNgHelper &ripNgRouting, uint32_t priority)
{
  NS_LOG_FUNCTION (this);
}

void 
RouterCsmaNetwork::ConfigureL2() 
{
  NS_LOG_FUNCTION (this << m_dataRate << m_delay);
  if (m_nodes.GetN () < 3)
    {
      m_csmaHelper.SetChannelAttribute ("DataRate", StringValue (m_dataRate));
      m_csmaHelper.SetChannelAttribute ("Delay", StringValue (m_delay));
      m_netDevices = m_csmaHelper.Install (NodeContainer (m_nodes));   
    }
  else
  {
    NetDeviceContainer bridgeNetDevices;
    NodeContainer all (m_nodes);
    for (uint32_t i = 0; i < all.GetN (); ++i)
      {
        NetDeviceContainer link = m_csmaHelper.Install (NodeContainer (all.Get (i), m_bridge));
        m_netDevices.Add (link.Get (0));
        bridgeNetDevices.Add (link.Get (1));
      }
    BridgeHelper bridge; 
    bridge.Install (m_bridge, bridgeNetDevices);
  }   
}

void 
RouterCsmaNetwork::ConfigureL3(Ipv6Address ipv6Address, Ipv6Prefix &prefix, Ipv6ListRoutingHelper listRH)
{
  NS_LOG_FUNCTION (this << ipv6Address << prefix);
  InternetStackHelper internetv6;
  internetv6.SetIpv4StackInstall (false);
  internetv6.SetRoutingHelper (listRH);
  for (uint32_t i = 0; i < m_nodes.GetN(); ++i)
  {
    if (m_nodes.Get(i)->GetObject<Ipv6> () == 0)
    {
      internetv6.Install (m_nodes.Get(i));
    }
  }
  

  Ipv6AddressHelper ipv6;

  ipv6.SetBase (ipv6Address, prefix);
  m_ipv6Interfaces = ipv6.Assign (m_netDevices);
  // std::cout << m_ipv6Interfaces.GetN () <<std::endl;
  for (uint32_t i = 0; i < m_nodes.GetN(); ++i)
    {
      m_ipv6Interfaces.SetForwarding (i, true);
    }
  // m_ipv6Interfaces.SetForwarding (0, true);
  // m_ipv6Interfaces.SetForwarding (1, true);
}

void
RouterCsmaNetwork::EnablePcap(std::string pcapFileName, bool promiscious)
{
  NS_LOG_FUNCTION (this << pcapFileName << promiscious);
  m_csmaHelper.EnablePcapAll (pcapFileName+"_routercsma", promiscious);
} 

void 
RouterCsmaNetwork::EnablePcapRouter(std::string pcapFileName)
{
  NS_LOG_FUNCTION (this << pcapFileName);
  m_csmaHelper.EnablePcap (pcapFileName+"_routercsma", m_router->GetDevice (m_index));
}

void 
RouterCsmaNetwork::EnableAscii(std::string fileName)
{
  NS_LOG_FUNCTION (this << fileName);
  AsciiTraceHelper ascii;
  m_csmaHelper.EnableAsciiAll (ascii.CreateFileStream (fileName+"_routercsma.tr"));
}

RouterCsmaNetwork::~RouterCsmaNetwork()
{
  NS_LOG_FUNCTION (this);
}

} // namespace ns3