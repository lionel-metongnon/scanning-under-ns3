/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 The Boeing Company
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
#include "ns3/log.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/internet-stack-helper.h"
#include "router-csma-network.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("RouterCsmaNetwork");


RouterCsmaNetwork::RouterCsmaNetwork(Ptr <Node> routerA, Ptr <Node> routerB, std::string dataRate, std::string delay)
: Network()
{
  NS_LOG_FUNCTION (this);
  m_nodes.Add (routerA);
  m_nodes.Add (routerB);
  m_router = m_nodes.Get (0);
  m_dataRate = dataRate;
  m_delay = delay;
}
RouterCsmaNetwork::RouterCsmaNetwork(Ptr <Node> routerA, std::string dataRate, std::string delay)
: Network()
{
  NS_LOG_FUNCTION (this);
  m_nodes.Create(1);
  m_nodes.Add (routerA);
  m_router = m_nodes.Get (0);
  m_dataRate = dataRate;
  m_delay = delay;
}

void 
RouterCsmaNetwork::SetMobility(std::vector<double> xy)
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
  m_csmaHelper.SetChannelAttribute ("DataRate", StringValue (m_dataRate));
  m_csmaHelper.SetChannelAttribute ("Delay", StringValue (m_delay));
  m_netDevices = m_csmaHelper.Install (m_nodes);   
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
  m_ipv6Interfaces.SetForwarding (0, true);
  m_ipv6Interfaces.SetForwarding (1, true);
}

void
RouterCsmaNetwork::EnablePcap(bool promiscious)
{
  NS_LOG_FUNCTION (this << promiscious);
  m_csmaHelper.EnablePcapAll ("csma", promiscious);
} 

RouterCsmaNetwork::~RouterCsmaNetwork()
{
  NS_LOG_FUNCTION (this);
}

} // namespace ns3