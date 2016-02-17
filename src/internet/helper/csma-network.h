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

#ifndef CSMA_NETWORK_H
#define CSMA_NETWORK_H

#include "ns3/csma-module.h"
#include "ns3/lr-wpan-module.h"
#include "network.h"
#include <string>

namespace ns3
{

class CsmaNetwork : public Network
{
public:

  /**
   * \brief Default Contructor.
   */
  CsmaNetwork(std::string dataRate, std::string delay);
  /**
   * \brief Contructor with the number of nodes.
   */
  CsmaNetwork(uint32_t nbrOfNodes, std::string dataRate, std::string delay);
  /**
   * \brief configure the L2 layer of the node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void ConfigureL2();
  virtual void SetMobility(std::vector<double> xy);

  /**
   * \brief Retrieve the container of the router node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void EnablePcap(bool promiscious);

  virtual ~CsmaNetwork();
  
private:
  CsmaHelper m_csmaHelper; //<!802.3 support 
  Ptr<Node> m_bridge;
  std::string m_dataRate;
  std::string m_delay;
};

}

#endif /* CSMA_NETWORK_H */