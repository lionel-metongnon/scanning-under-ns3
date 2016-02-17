/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright 2007 University of Washington
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
 */
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/assert.h"
#include "ns3/ipv6-address.h"
#include "ns3/nstime.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/object-vector.h"
#include "ns3/trace-source-accessor.h"

#include "coap-server.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CoapServerApplication");

NS_OBJECT_ENSURE_REGISTERED (CoapServer);

TypeId
CoapServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CoapServer")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<CoapServer> ()
    .AddAttribute ("Interval", 
                   "The time to wait between packets",
                   // TimeValue (Seconds (1.0)),
                   TimeValue (Time ("2ms")),
                   MakeTimeAccessor (&CoapServer::m_interval),
                   MakeTimeChecker ())
    // .AddAttribute ("ClientsAddress",
    //                "The list of the CoapServer clientsof this server Node.",
    //                ObjectVectorValue (),
    //                MakeObjectVectorAccessor (&CoapServer::m_clientAddresses),
    //                MakeObjectVectorChecker<Ipv6Address> ())
    .AddAttribute ("RemotePort", 
                   "The destination port of the outbound packets",
                   UintegerValue (80),
                   MakeUintegerAccessor (&CoapServer::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize", "Size of echo data in outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&CoapServer::SetDataSize,
                                         &CoapServer::GetDataSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&CoapServer::m_txTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}

CoapServer::CoapServer ()
{
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_socket = 0;
  m_sendEvent = EventId ();
  m_index = 0;
}

CoapServer::~CoapServer()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
}

void 
CoapServer::SetRemote (uint16_t port)
{
  NS_LOG_FUNCTION (this<< port);
  m_peerPort = port;
}

void
CoapServer::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
CoapServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
}

void 
CoapServer::SetClientAddresses (std::vector<Ipv6Address> &clientAddresses)
{
  NS_LOG_FUNCTION (this);
  m_clientAddresses = clientAddresses;
  if (m_clientAddresses.empty ())
    {
      NS_LOG_INFO ("Any Clients are provide to the server");
      StopApplication ();
    }
  else
    {
      Prepare ();
    }
}

void
CoapServer::Prepare (void)
{
  NS_LOG_FUNCTION (this);
  if (m_index == m_clientAddresses.size ())
  {
    m_index = 0;
  }
  if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      m_socket->Bind6();
    }
  m_socket->Connect (Inet6SocketAddress (m_clientAddresses.at (m_index), m_peerPort));
  m_socket->SetRecvCallback (MakeCallback (&CoapServer::HandleRead, this));

  if (m_index == 0)
    {
      ScheduleTransmit (Seconds (60));
    }
  else
    {
      ScheduleTransmit (Seconds (0.2));
    }
}
void 
CoapServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = 0;
    }
}

void 
CoapServer::SetDataSize (uint32_t dataSize)
{
  NS_LOG_FUNCTION (this << dataSize);

  //
  // If the server is setting the echo packet data size this way, we infer
  // that she doesn't care about the contents of the packet at all, so 
  // neither will we.
  //
  m_size = dataSize;
}

uint32_t 
CoapServer::GetDataSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_size;
}

void 
CoapServer::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
    // m_sendEvent = Simulator::Schedule (Time ("20ms"), &CoapServer::Send, this);

  m_sendEvent = Simulator::Schedule (dt, &CoapServer::Send, this);
}

void 
CoapServer::Send (void)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Packet> p = Create<Packet> (100);
  // call to the trace sinks before the packet is actually sent,
  // so that tags added to the packet can be sent as well
  m_txTrace (p);
  m_socket->Send (p);

  ++m_sent;

  NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server sent " << m_size << " bytes to " <<
               m_clientAddresses.at (m_index) << " port " << m_peerPort);

  ++m_index;
  Prepare ();
}

void
CoapServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Address from;
  NS_LOG_INFO ("Server CoAP packet");
  while ((packet = socket->RecvFrom (from)))
    {
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s server received " << packet->GetSize () << " bytes from " <<
                   Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
                   Inet6SocketAddress::ConvertFrom (from).GetPort ());
    }
}

} // Namespace ns3
