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
#include "csma-network.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("CsmaNetwork");


CsmaNetwork::CsmaNetwork(std::string dataRate, std::string delay)
: Network(1)
{
  NS_LOG_FUNCTION (this);
  m_bridge = NULL;
  m_dataRate = dataRate;
  m_delay = delay;
}

CsmaNetwork::CsmaNetwork(uint32_t nbrOfNodes, std::string dataRate, std::string delay)
: Network(nbrOfNodes)
{
  NS_LOG_FUNCTION (this);
  if (nbrOfNodes > 1)
    {
      m_bridge = CreateObject<Node> ();
    }
    m_dataRate = dataRate;
    m_delay = delay;
}

void 
CsmaNetwork::SetMobility(std::vector<double> xy)
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
CsmaNetwork::EnablePcap(bool promiscious)
{
  NS_LOG_FUNCTION (this << promiscious);
  m_csmaHelper.EnablePcapAll ("csma", promiscious);
} 

CsmaNetwork::~CsmaNetwork()
{
  NS_LOG_FUNCTION (this);
}

} // namespace ns3