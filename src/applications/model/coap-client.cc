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
#include "ns3/ipv6-address.h"
#include "ns3/address-utils.h"
#include "ns3/nstime.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"

#include <cstdlib>
#include <ctime>

#include "coap-client.h"
#include "dns-vicious-client.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CoapClientApplication");

NS_OBJECT_ENSURE_REGISTERED (CoapClient);

TypeId
CoapClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CoapClient")
    .SetParent<Application> ()
    .SetGroupName("Applications")
    .AddConstructor<CoapClient> ()
    .AddAttribute ("Port", "Port on which we listen for incoming packets.",
                   UintegerValue (5683),
                   MakeUintegerAccessor (&CoapClient::m_port),
                   MakeUintegerChecker<uint16_t> ())
    .AddAttribute ("Interval", 
                   "The time between two ",
                   TimeValue (MilliSeconds (200)),
                   MakeTimeAccessor (&CoapClient::m_interval),
                   MakeTimeChecker ())
    .AddAttribute ("PacketSize", "Size of packets generated",
                      UintegerValue (65),
                      MakeUintegerAccessor (&CoapClient::m_sendSize),
                      MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

CoapClient::CoapClient ()
{
  NS_LOG_FUNCTION (this);
  m_data = 0;
  m_dataSize = 0;
  m_sendEvent = EventId ();
  m_magic_number = CreateObject<UniformRandomVariable>();
}

CoapClient::~CoapClient()
{
  NS_LOG_FUNCTION (this);
  m_socket = 0;
  attackerList.clear ();
  delete [] m_data;
  m_data = 0;
  m_dataSize = 0;
}

void
CoapClient::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  Application::DoDispose ();
}

void 
CoapClient::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  if (m_socket == 0)
    {
      m_socket = Socket::CreateSocket (GetNode (), tid);
      Inet6SocketAddress local6 = Inet6SocketAddress (Ipv6Address::GetAny (), m_port);
      m_socket->Bind (local6);
    }
  m_socket->SetRecvCallback (MakeCallback (&CoapClient::HandleRead, this));
}

void 
CoapClient::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0) 
    {
      m_socket->Close ();
      m_socket->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  Simulator::Cancel (m_sendEvent);
  ShowAttackerList ();
}

void 
CoapClient::SetFill (std::string fill)
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
}

void
CoapClient::ShowAttackerList()
{
  NS_LOG_FUNCTION (this);
  // std::cout << "Attacker list addresses are : " << std::endl;
  for (std::map<Ipv6Address, float>::iterator i = attackerList.begin(); i != attackerList.end(); ++i)
  {
    // cout << i->first << std::endl;
    NS_LOG_INFO (i->first <<" "<<i->second);
  }
}

void 
CoapClient::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Ptr<Packet> packet;
  Ipv6Address sender;
  Address from;
  while ((packet = socket->RecvFrom (from)))
    {
      sender = Inet6SocketAddress::ConvertFrom (from).GetIpv6 ();
      NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s node received " << packet->GetSize () << " bytes from " <<
                   sender << " port " <<
                   Inet6SocketAddress::ConvertFrom (from).GetPort ());

      NS_LOG_INFO ("CoAP packet");

      uint32_t dataSize = packet->GetSize () - 1;
      uint8_t data[dataSize];
      packet->CopyData (data, dataSize);
      std::string sdata((char *)data, dataSize);
      if (sdata.compare ("Penetration Attack") == 0)
        {
          std::map<Ipv6Address, float>::iterator record = attackerList.find (sender);
          if (record == attackerList.end ())
            {
              NS_LOG_INFO ("New Attacker");
              // m_magic_number->SetAttribute ("Min", DoubleValue (0.0));
              // m_magic_number->SetAttribute ("Max", DoubleValue (1.0));
              // m_magic_number->SetStream (1);
              float magicNumber = m_magic_number->GetValue ();
              attackerList.insert (std::pair<Ipv6Address, float>(sender, magicNumber));
              if (magicNumber <= 0.1)
                {
                  /* Node compromised*/
                  Ptr<DnsViciousClient> dnsAttack = GetNode()->GetApplication(0)->GetObject <DnsViciousClient>();
                  dnsAttack->ViciousMode ();
                  SetFill ("Host Compromise");
                }
              else
                {
                  SetFill ("Failed to compromise the host");
                }
            }
          else
            {
              if (record->second <= 0.1)
                {
                  SetFill ("Host already Compromise");
                }
              else
                {
                  SetFill ("You have already failed to compromise this host");
                }
            }
        }
      else if (sdata.compare ("Get Temperature Data From The Node") == 0)
        {
          m_dataSize = 0;
          NS_LOG_INFO ("Temperature Server Message");
          // for (uint16_t i = 0; i < 4; ++i)
          //   {
              // socket->SendTo (Create<Packet> (m_sendSize), 0, from);
              ScheduleTransmit (m_interval, socket, from, 2);
            // }
        }
      else
        {
          m_dataSize = 0;
        }

      Ptr<Packet> newPacket;
      if (m_dataSize)
        {
          //
          // If m_dataSize is non-zero, we have a data buffer of the same size that we
          // are expected to copy and send.  This state of affairs is created if one of
          // the Fill functions is called.  In this case, m_sendSize must have been set
          // to agree with m_dataSize
          //
          NS_ASSERT_MSG (m_data, "CoapClient::HandleRead(): m_dataSize but no m_data");
          newPacket = Create<Packet> (m_data, m_dataSize);
        }
      else
        {
          //
          // If m_dataSize is zero, we'll send any data
          //
          newPacket = Create<Packet> (m_sendSize);
        }

        // socket->SendTo (newPacket, 0, from);
        ScheduleTransmit (Seconds (.0), socket, from);

        // NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s node sent " << newPacket->GetSize () << " bytes to " <<
        //             Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
        //             Inet6SocketAddress::ConvertFrom (from).GetPort ());
        }
}

void 
CoapClient::ScheduleTransmit (Time dt, Ptr<Socket> socket, Address from, uint16_t count)
{
  NS_LOG_FUNCTION (this << dt);
  for (uint16_t i = 1; i <= count; ++i)
  {
    m_sendEvent = Simulator::Schedule (dt * i, &CoapClient::Send, this, socket, from);
  }
}

void 
CoapClient::Send (Ptr<Socket> socket, Address from)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (m_sendEvent.IsExpired ());

  Ptr<Packet> newPacket;
  if (m_dataSize)
    {
      //
      // If m_dataSize is non-zero, we have a data buffer of the same size that we
      // are expected to copy and send.  This state of affairs is created if one of
      // the Fill functions is called.  In this case, m_sendSize must have been set
      // to agree with m_dataSize
      //
      NS_ASSERT_MSG (m_data, "CoapClient::Send(): m_dataSize but no m_data");
      newPacket = Create<Packet> (m_data, m_dataSize);
    }
  else
    {
      //
      // If m_dataSize is zero, we'll send any data
      //
      newPacket = Create<Packet> (m_sendSize);
    }
  socket->SendTo (newPacket, 0, from);

  NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s node sent " << newPacket->GetSize () << " bytes to " <<
              Inet6SocketAddress::ConvertFrom (from).GetIpv6 () << " port " <<
              Inet6SocketAddress::ConvertFrom (from).GetPort ());

}

} // Namespace ns3