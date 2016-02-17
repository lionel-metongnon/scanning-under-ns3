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

#ifndef ROUTER_CSMA_NETWORK_H
#define ROUTER_CSMA_NETWORK_H

#include "ns3/csma-module.h"
#include "ns3/lr-wpan-module.h"
#include "network.h"
#include <string>

namespace ns3
{

class RouterCsmaNetwork : public Network
{
public:

  RouterCsmaNetwork(Ptr <Node> routerA, std::string dataRate, std::string delay);
  /**
   * \brief Default Contructor.
   */
  RouterCsmaNetwork(Ptr <Node> routerA, Ptr <Node> routerB, std::string dataRate, std::string delay);
  /**
   * \brief configure the L2 layer of the node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void SetMobility(std::vector<double> xy);

  virtual void ConfigureL2();
  void EnableRouting (Ipv6ListRoutingHelper &listRH, RipNgHelper &ripNgRouting, uint32_t priority);
  /**
   * \brief configure the L2 layer of the node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void ConfigureL3(Ipv6Address ipv6Address, Ipv6Prefix &prefix, Ipv6ListRoutingHelper listRH);
  /**
   * \brief Retrieve the container of the router node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void EnablePcap(bool promiscious);

  virtual ~RouterCsmaNetwork();
  
private:
  CsmaHelper m_csmaHelper; //<!802.3 support 
  Ptr<Node> m_bridge;
  std::string m_dataRate;
  std::string m_delay;
};

}

#endif /* ROUTER_CSMA_NETWORK_H */