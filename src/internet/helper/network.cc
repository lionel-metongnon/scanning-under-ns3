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

#include "network.h"
#include "ns3/log.h"
#include "ns3/ipv6-address-helper.h"
#include "ns3/internet-stack-helper.h"
// #include "ns3/ripng-helper.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("Network");

Network::Network()
{
  NS_LOG_FUNCTION (this);
}

Network::Network(uint32_t nbrOfNodes)
{
  NS_LOG_FUNCTION (this);
  m_router = CreateObject<Node> ();
  m_nodes.Create (nbrOfNodes);
}

Ptr<Node>
Network::GetRouter(void)
{
  NS_LOG_FUNCTION (this);
  return m_router;
}

NodeContainer
Network::GetNodes(void)
{
  NS_LOG_FUNCTION (this);
  return m_nodes;
}

NetDeviceContainer
Network::GetNetworkDevices (void)
{
  NS_LOG_FUNCTION (this);
  return m_netDevices;
}

std::vector<Ipv6Address>
Network::GetNodesAddresses (void)
{
  std::vector<Ipv6Address> nodeAddresses;
  NS_LOG_FUNCTION (this);
  for (uint32_t i = 1 ; i < m_ipv6Interfaces.GetN () ; ++i)
  {
    nodeAddresses.push_back (m_ipv6Interfaces.GetAddress (i, 1));
  }
  return nodeAddresses;
}

void 
Network::EnableRouting (Ipv6ListRoutingHelper &listRH, RipNgHelper &ripNgRouting, uint32_t priority)
{
  NS_LOG_FUNCTION (this << priority);
  // RipNgHelper ripNgRouting;
  ripNgRouting.ExcludeInterface (m_router, 1);
  // listRH.Add (ripNgRouting, priority);
}

void 
Network::ConfigureL3(Ipv6Address ipv6Address, Ipv6Prefix &prefix, Ipv6ListRoutingHelper listRH)
{
  NS_LOG_FUNCTION (this << ipv6Address << prefix);
  InternetStackHelper internetv6;
  internetv6.SetIpv4StackInstall (false);
  internetv6.Install (m_nodes);

  internetv6.SetRoutingHelper (listRH);
  internetv6.Install (m_router);

  Ipv6AddressHelper ipv6;

  ipv6.SetBase (ipv6Address, prefix);
  m_ipv6Interfaces = ipv6.Assign (m_netDevices);
  m_ipv6Interfaces.SetForwarding (0, true);
  m_ipv6Interfaces.SetDefaultRouteInAllNodes (0);
}

Ipv6InterfaceContainer 
Network::GetIpv6Interfaces (void)
{
  return m_ipv6Interfaces;
}

// void 
// Network::SetApplication(Ptr<Application> app)
// {
//   ObjectFactory factory.SetTypeId (PenetrationTools::GetTypeId ());
//   factory.Set (name, value);
//   for (std::vector<Ptr<Node> >::iterator node = m_nodes.begin(); node != m_nodes.end(); ++node)
//   {
//     Ptr<Application> app = factory.Create<Application> (); 
//     node->AddApplication (app);
//     m_app.push_back (app);
//   }
// }

Network::~Network()
{
  NS_LOG_FUNCTION (this);
  m_router->Dispose ();
  m_router = 0;
  for (NodeContainer::Iterator i = m_nodes.Begin(); i != m_nodes.End(); ++i)
    {
      (*i)->Dispose ();
    }
  for (Ipv6InterfaceContainer::Iterator i = m_ipv6Interfaces.Begin(); i != m_ipv6Interfaces.End(); ++i)
    {
      (*i).first->Dispose ();
    }
  for (NetDeviceContainer::Iterator i = m_netDevices.Begin(); i != m_netDevices.End(); ++i)
    {
      (*i)->Dispose ();
    }
}

} // namespace ns3