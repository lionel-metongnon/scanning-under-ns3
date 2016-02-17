/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright  (c) 2011 The Boeing Company
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

#ifndef NETWORK_H
#define NETWORK_H

#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/net-device-container.h"
#include "ns3/ipv6-list-routing-helper.h"
#include <map>
#include "ns3/ripng-helper.h"
#include "ns3/ipv6-interface-container.h"
#include "ns3/simple-ref-count.h"

#include <string>

namespace ns3
{

class Network : public SimpleRefCount<Network>
{
public:
  /**
   * \brief Default Contructor.
   */
  Network ();
  /**
   * \brief Contructor with the number of nodes.
   */
  Network (uint32_t nbrOfNodes);
  /**
   * \brief Retrieve the router node (s).
   *
   * \returns Ptr<Node>.
   */
  Ptr<Node>  GetRouter  (void);
  /**
   * \brief Retrieve the container of all the nodes except the router.
   *
   * \returnsNodeContainer
   */
  NodeContainer GetNodes (void);
  /**
   * \briefRetrieve the container of all the NetDeviceContainer
   *
   * \returns the NetDeviceContainer.
   */
  NetDeviceContainer GetNetworkDevices  (void);
  /**
   * \brief Retrieve the container of the router node (s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  void SetApplication (Ptr<Application> app);
    /**
   * \brief Retrieve the container of the router node (s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void SetMobility (std::vector<double> xy) = 0;
    /**
   * \brief configure the L2 layer of the node(s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void ConfigureL2 () = 0;
  /**
   * \brief Retrieve the container of the router node (s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void EnableRouting (Ipv6ListRoutingHelper &listRH, RipNgHelper &ripNgRouting, uint32_t priority);
  /**
   * \brief Retrieve the container of the router node (s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual void ConfigureL3 (Ipv6Address ipv6Address, Ipv6Prefix &prefix, Ipv6ListRoutingHelper listRH);
  /**
   * \brief Retrieve the container of the router node (s).
   *
   * \param index the index of the requested NetDevice
   * \returns the requested NetDevice.
   */
  virtual std::vector<Ipv6Address> GetNodesAddresses (void);
  virtual Ipv6InterfaceContainer GetIpv6Interfaces (void);
  virtual void EnablePcap (bool promiscious) = 0;

  virtual ~Network();

protected:
  NodeContainer m_nodes; //!< Container of the normal Nodes
  Ptr<Node> m_router; //!< The router Nodes
  NetDeviceContainer m_netDevices; //!< Container of router Nodes
  Ipv6InterfaceContainer m_ipv6Interfaces; //!< Container of router Nodes
  ApplicationContainer m_apps; //!< Container of nodes application
};

} // namespace ns3

#endif /* NETWORK_H */