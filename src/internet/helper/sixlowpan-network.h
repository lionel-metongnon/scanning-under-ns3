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

#ifndef SIXLOWPAN_NETWORK_H
#define SIXLOWPAN_NETWORK_H

#include "ns3/mobility-module.h"
#include "ns3/sixlowpan-module.h"
#include "ns3/lr-wpan-module.h"
#include "network.h"

namespace ns3
{

class SixlowpanNetwork : public Network
{
public:

  /**
   * \brief Default Contructor.
   */
  SixlowpanNetwork();
  /**
   * \brief Contructor with the number of nodes.
   */
  SixlowpanNetwork(uint32_t nbrOfNodes);
  /**
   * \brief Configure themobility of the nodes.
   *
   * \param the coordonates of the mobility
   */
  void SetMobility(std::vector<double> xy);
    /**
   * \brief configure the L2 layer of the node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void ConfigureL2();
  /**
   * \brief Retrieve the container of the router node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void EnablePcap(bool promiscious);

  virtual ~SixlowpanNetwork();
  
private:
  LrWpanHelper m_lrWpanHelper; //<!802.15.4 support to 6LowPanNet
  MobilityHelper m_mobility;
};

}

#endif /* SIXLOWPAN_NETWORK_H */