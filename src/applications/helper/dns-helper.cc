/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 INRIA
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
 * Author: Mathieu Lacage <mathieu.lacage@sophia.inria.fr>
 */
#include "dns-helper.h"
#include "ns3/dns-server.h"
#include "ns3/dns-client.h"
#include "ns3/dns-vicious-client.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

DnsServerHelper::DnsServerHelper (uint16_t port)
{
  m_factory.SetTypeId (DnsServer::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void 
DnsServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
DnsServerHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DnsServerHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DnsServerHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
DnsServerHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<DnsServer> ();
  node->AddApplication (app);

  return app;
}

DnsClientHelper::DnsClientHelper (Address address, uint16_t port)
{
  m_factory.SetTypeId (DnsClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (address));
  SetAttribute ("RemotePort", UintegerValue (port));
}

DnsClientHelper::DnsClientHelper (Ipv4Address address, uint16_t port)
{
  m_factory.SetTypeId (DnsClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

DnsClientHelper::DnsClientHelper (Ipv6Address address, uint16_t port)
{
  m_factory.SetTypeId (DnsClient::GetTypeId ());
  SetAttribute ("RemoteAddress", AddressValue (Address(address)));
  SetAttribute ("RemotePort", UintegerValue (port));
}

void 
DnsClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

void
DnsClientHelper::SetFill (Ptr<Application> app, std::string fill)
{
  app->GetObject<DnsClient>()->SetFill (fill);
}

void
DnsClientHelper::SetFill (Ptr<Application> app, uint8_t fill, uint32_t dataLength)
{
  app->GetObject<DnsClient>()->SetFill (fill, dataLength);
}

void
DnsClientHelper::SetFill (Ptr<Application> app, uint8_t *fill, uint32_t fillLength, uint32_t dataLength)
{
  app->GetObject<DnsClient>()->SetFill (fill, fillLength, dataLength);
}

ApplicationContainer
DnsClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DnsClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DnsClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
DnsClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<DnsClient> ();
  node->AddApplication (app);

  return app;
}

DnsViciousClientHelper::DnsViciousClientHelper (
  Ipv6Address srcAddress,
  Ipv6Address dstAddress,
  uint16_t port)
{
  m_factory.SetTypeId (DnsViciousClient::GetTypeId ());
  SetAttribute ("LocalAddress", Ipv6AddressValue (srcAddress));
  SetAttribute ("RemoteAddress", Ipv6AddressValue (dstAddress));
  SetAttribute ("RemotePort", UintegerValue (port));
}

void 
DnsViciousClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

void
DnsViciousClientHelper::SetFill (Ptr<Application> app, std::string fill)
{
  app->GetObject<DnsClient>()->SetFill (fill);
}

void
DnsViciousClientHelper::SetFill (Ptr<Application> app, uint8_t fill, uint32_t dataLength)
{
  app->GetObject<DnsClient>()->SetFill (fill, dataLength);
}

void
DnsViciousClientHelper::SetFill (
  Ptr<Application> app,
  uint8_t *fill,
  uint32_t fillLength,
  uint32_t dataLength)
{
  app->GetObject<DnsClient>()->SetFill (fill, fillLength, dataLength);
}

ApplicationContainer
DnsViciousClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DnsViciousClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
DnsViciousClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
DnsViciousClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<DnsViciousClient> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
