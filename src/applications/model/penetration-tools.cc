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
#include "ns3/trace-source-accessor.h"

#include <algorithm>
#include <cstdlib>
#include <time.h>
#include <fstream>

#include "penetration-tools.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PenetrationToolsApplication");

NS_OBJECT_ENSURE_REGISTERED (PenetrationTools);

TypeId
PenetrationTools::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PenetrationTools")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<PenetrationTools> ()
    .AddAttribute ("Interval", 
                   "The time to wait between packets",
                   TimeValue (MilliSeconds (200)),
                   MakeTimeAccessor (&PenetrationTools::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("RemotePort", 
                   "The destination port of the outbound packets",
                   UintegerValue (80),
                   MakeUintegerAccessor (&PenetrationTools::m_peerPort),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("PacketSize", "Size of echo data in outbound packets",
                   UintegerValue (100),
                   MakeUintegerAccessor (&PenetrationTools::SetDataSize,
                                         &PenetrationTools::GetDataSize),
                   MakeUintegerChecker<uint32_t> ())
    .AddTraceSource ("Tx", "A new packet is created and is sent",
                     MakeTraceSourceAccessor (&PenetrationTools::m_txTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}

PenetrationTools::PenetrationTools ()
{
  NS_LOG_FUNCTION (this);
  m_sent = 0;
  m_socket = 0;
  m_victimAddresses = std::list <Ipv6Address> ();
  m_sendEvent = EventId ();
  m_data = 0;
  m_dataSize = 0;
}

PenetrationTools::~PenetrationTools()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;

  delete [] m_data;
  m_data = 0;
  m_dataSize = 0;
}

void 
PenetrationTools::SetRemote (Ipv6Address ip, uint16_t port)
{
  NS_LOG_FUNCTION (this << ip << port);
  m_peerAddress = ip;
  m_peerPort = port;
}

void
PenetrationTools::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
PenetrationTools::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
}

void
PenetrationTools::StartPenetration (std::list<Ipv6Address> &victimAddresses)
{
  NS_LOG_FUNCTION (this);
  m_victimAddresses = victimAddresses;
  networkSize = m_victimAddresses.size ();
  // if we have a victim list, proccess to the penetration
  // if not just stop the application
  if (m_victimAddresses.empty())
    {
      NS_LOG_INFO ("No victims to attack");
      StopApplication();
    }
  else
    {
      SetFill("Penetration Attack");
      Penetration ();
    }
}

void
PenetrationTools::Penetration ()
{
  NS_LOG_FUNCTION (this);
  // Get the next victim address, connect to him and send the packet
  m_peerAddress = m_victimAddresses.front ();
  m_victimAddresses.pop_front ();
  if (m_socket == 0)
  {
    TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
    m_socket = Socket::CreateSocket (GetNode (), tid);
    m_socket->Bind6();
  }
  m_socket->Connect (Inet6SocketAddress (m_peerAddress, m_peerPort));
  m_socket->SetRecvCallback (MakeCallback (&PenetrationTools::HandleRead, this));
  ScheduleTransmit (m_interval);
}

void
PenetrationTools::Save (void)
{
  if (m_victimAddresses.empty())
    {
      std::ostringstream oss;
      oss << GetDataSize();
      oss << "_";
      oss << networkSize;

      std::ofstream resultFile;
      resultFile.open ( std::string("./data/"+oss.str()+"_penetration.rst").c_str(), std::ofstream::out | std::ofstream::trunc);
      resultFile << m_compromisedNodeAddress.size ();
      resultFile.close ();
      resultFile.open ( std::string("./data/"+oss.str()+"_penetration_details.rst").c_str(), std::ofstream::out | std::ofstream::trunc);
      
      for (std::set<Ipv6Address>::iterator i = m_compromisedNodeAddress.begin(); i != m_compromisedNodeAddress.end(); ++i)
        {
          // NS_LOG_INFO (*i);
          resultFile << *i << std::endl;
        }
      resultFile.close ();
    }
}

void 
PenetrationTools::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socket = 0;
    }
  Simulator::Cancel (m_sendEvent);
  Save ();
}

void 
PenetrationTools::SetDataSize (uint32_t dataSize)
{
  NS_LOG_FUNCTION (this << dataSize);

  //
  // If the attacker is setting the echo packet data size this way, we infer
  // that she doesn't care about the contents of the packet at all, so 
  // neither will we.
  //
  delete [] m_data;
  m_data = 0;
  m_dataSize = 0;
  m_size = dataSize;
}

uint32_t 
PenetrationTools::GetDataSize (void) const
{
  NS_LOG_FUNCTION (this);
  return m_size;
}

void 
PenetrationTools::SetFill (std::string fill)
{
  NS_LOG_FUNCTION (this << fill);

  uint32_t dataSize = fill.size () + 1;

  if (dataSize != m_dataSize)
    {
      delete [] m_data;
      m_data = new uint8_t [dataSize];
      m_dataSize = dataSize;
    }

  memcpy (m_data, fill.c_str (), dataSize);

  //
  // Overwrite packet size attribute.
  //
  // m_size = dataSize; // I comment this line just to still has the original size pass in param for the plot name
}

void 
PenetrationTools::ScheduleTransmit (Time dt)
{
  NS_LOG_FUNCTION (this << dt);
  m_sendEvent = Simulator::Schedule (dt, &PenetrationTools::Send, this);
}

void 
PenetrationTools::Send (void)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());
  Ptr<Packet> p;
  if (m_dataSize)
    {
      //
      // If m_dataSize is non-zero, we have a data buffer of the same size that we
      // are expected to copy and send.  This state of affairs is created if one of
      // the Fill functions is called.  In this case, m_size must have been set
      // to agree with m_dataSize
      //
      // NS_ASSERT_MSG (m_dataSize == m_size, "PenetrationTools::Send(): m_size and m_dataSize inconsistent");
      NS_ASSERT_MSG (m_data, "PenetrationTools::Send(): m_dataSize but no m_data");
      p = Create<Packet> (m_data, m_dataSize);
    }
  else
    {
      //
      // If m_dataSize is zero, the attacker has indicated that it doesn't care
      // about the data itself either by specifying the data size by setting
      // the corresponding attribute or by not calling a SetFill function.  In
      // this case, we don't worry about it either.  But we do allow m_size
      // to have a value different from the (zero) m_dataSize.
      //
      p = Create<Packet> (m_size);
    }
  // call to the trace sinks before the packet is actually sent,
  // so that tags added to the packet can be sent as well
  m_txTrace (p);
  m_socket->Send (p);

  ++m_sent;

  NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s attacker sent " << m_size << " bytes to " <<
               m_peerAddress << " port " << m_peerPort);

  // If we have more victim continue the compromise attack
  if (!m_victimAddresses.empty ()) 
    {
      Penetration ();
    }
}

void
PenetrationTools::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
  Ipv6Address victim;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      victim = Inet6SocketAddress::ConvertFrom (from).GetIpv6 ();
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s attacker received " << packet->GetSize () << " bytes from " <<
                   victim << " port " <<
                   Inet6SocketAddress::ConvertFrom (from).GetPort ());
      
      // after getting the packet, scan the data to know if you compromised the node or failed
      uint32_t dataSize = packet->GetSize () - 1;
      uint8_t data[dataSize];
      packet->CopyData (data, dataSize);
      std::string sdata (data, data+dataSize);
      if (sdata.compare ("Host Compromise") == 0)
        {
          m_compromisedNodeAddress.insert (victim);
          NS_LOG_INFO ("I'm darth vador and I crushed " << victim << " with my attack");
        }
      else
        {
          NS_LOG_INFO (victim << " survived to my attack. The force is with you!!!");
        }
    }
}

} // Namespace ns3
