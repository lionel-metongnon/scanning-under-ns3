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

#ifndef COAP_CLIENT_H
#define COAP_CLIENT_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/address.h"
#include "ns3/random-variable-stream.h"

#include <map>

namespace ns3 {

class Socket;
class Packet;

/**
 * \ingroup applications 
 * \defgroup Iot Node App CoAP
 */

/**
 * \ingroup Iot Node App
 * \brief A CoAP client
 *
 * Every packet received is sent back.
 */
class CoapClient : public Application 
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  CoapClient ();
  virtual ~CoapClient ();

protected:
  virtual void DoDispose (void);

private:

  virtual void StartApplication (void);
  virtual void StopApplication (void);

  /**
   * \brief Handle a packet reception.
   *
   * This function is called by lower layers.
   *
   * \param socket the socket the packet was received to.
   */
  void HandleRead (Ptr<Socket> socket);

  void ShowAttackerList();

  /**
   * Set the data fill of the packet (what is sent as data to the server) to 
   * the zero-terminated contents of the fill string string.
   *
   * \warning The size of resulting echo packets will be automatically adjusted
   * to reflect the size of the fill string -- this means that the PacketSize
   * attribute may be changed as a result of this call.
   *
   * \param fill The string to use as the actual echo data bytes.
   */
  void SetFill (std::string fill);

  void ScheduleTransmit (Time dt, Ptr<Socket> socket, Address from, uint16_t count=1);
  void Send (Ptr<Socket> socket, Address from);

  uint32_t m_dataSize; //!< packet payload size (must be equal to m_size)
  uint8_t *m_data; //!< packet payload data
  uint16_t m_port; //!< Port on which we listen for incoming packets.
  uint32_t m_sendSize; //!< Size of incoming packets.
  Ptr<UniformRandomVariable>  m_magic_number;
  Ptr<Socket> m_socket; //!< IPv6 Socket
  EventId m_sendEvent; //!< Event to send the next packet
  Time m_interval; //!< Packet inter-send time
  std::map <Ipv6Address, float> attackerList;
};

} // namespace ns3

#endif /* COAP_CLIENT_H */

