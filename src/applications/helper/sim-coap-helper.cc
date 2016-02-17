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
#include "sim-coap-helper.h"
#include "ns3/coap-client.h"
#include "ns3/coap-server.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

SimCoapClientHelper::SimCoapClientHelper (uint16_t port)
{
  m_factory.SetTypeId (CoapClient::GetTypeId ());
  SetAttribute ("Port", UintegerValue (port));
}

void 
SimCoapClientHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
SimCoapClientHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
SimCoapClientHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
SimCoapClientHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

ApplicationContainer 
SimCoapClientHelper::Install (Ptr<Network> n) const
{
  return Install(n->GetNodes());
}

Ptr<Application>
SimCoapClientHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<CoapClient> ();
  node->AddApplication (app);

  return app;
}

SimCoapServerHelper::SimCoapServerHelper (uint16_t port)
{
  m_factory.SetTypeId (CoapServer::GetTypeId ());
  SetAttribute ("RemotePort", UintegerValue (port));
}

void 
SimCoapServerHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
SimCoapServerHelper::Install (Ptr<Node> node, std::vector<Ipv6Address> &clientAddresses) const
{
  return ApplicationContainer (InstallPriv (node, clientAddresses));
}

ApplicationContainer
SimCoapServerHelper::Install (std::string nodeName, std::vector<Ipv6Address> &clientAddresses) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node, clientAddresses));
}

ApplicationContainer
SimCoapServerHelper::Install (NodeContainer c, std::vector<Ipv6Address> &clientAddresses) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i, clientAddresses));
    }

  return apps;
}

ApplicationContainer 
SimCoapServerHelper::Install (Ptr<Network> n, std::vector<Ipv6Address> &clientAddresses) const
{
  return Install(n->GetNodes(), clientAddresses);
}

Ptr<Application>
SimCoapServerHelper::InstallPriv (Ptr<Node> node, std::vector<Ipv6Address> &clientAddresses) const
{
  Ptr<CoapServer> app = m_factory.Create<CoapServer> ();
  node->AddApplication (app);
  app->SetClientAddresses (clientAddresses);

  return app;
}

} // namespace ns3