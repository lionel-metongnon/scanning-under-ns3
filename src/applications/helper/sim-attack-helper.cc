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
 
#include "sim-attack-helper.h"
#include "ns3/penetration-tools.h"
#include "ns3/uinteger.h"
#include "ns3/names.h"

namespace ns3 {

ScanToolsHelper::ScanToolsHelper (uint16_t port)
{
  m_factory.SetTypeId (ScanTools::GetTypeId ());
  SetAttribute ("RemotePort", UintegerValue (port));
}

void 
ScanToolsHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

// void
// ScanToolsHelper::SetTargetNetwork (
//       std::map<std::string,
//       std::string> targetNetwork)
// {
//     SimCoap::SetTargetNetwork (targetNetwork);
// }

ApplicationContainer
ScanToolsHelper::Install (Ptr<Node> node, std::map<Ipv6Address, Ipv6Prefix> &targetedNetworks, ScanTools::ScanType scanType) const
{
  return ApplicationContainer (InstallPriv (node, targetedNetworks, scanType));
}

ApplicationContainer
ScanToolsHelper::Install (std::string nodeName, std::map<Ipv6Address, Ipv6Prefix> &targetedNetworks, ScanTools::ScanType scanType) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node, targetedNetworks, scanType));
}

ApplicationContainer
ScanToolsHelper::Install (NodeContainer c, std::map<Ipv6Address, Ipv6Prefix> &targetedNetworks, ScanTools::ScanType scanType) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i, targetedNetworks, scanType));
    }

  return apps;
}

Ptr<Application>
ScanToolsHelper::InstallPriv (Ptr<Node> node, std::map<Ipv6Address, Ipv6Prefix> &targetedNetworks, ScanTools::ScanType scanType) const
{
  Ptr<ScanTools> app = m_factory.Create<ScanTools> ();
  node->AddApplication (app);
  app->SetTargetedNetworks (targetedNetworks);
  app->SetScanType (scanType);

  return app;
}

PenetrationToolsHelper::PenetrationToolsHelper (uint16_t port)
{
  m_factory.SetTypeId (PenetrationTools::GetTypeId ());
  SetAttribute ("RemotePort", UintegerValue (port));
}

void 
PenetrationToolsHelper::SetAttribute (
  std::string name, 
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
PenetrationToolsHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
PenetrationToolsHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
PenetrationToolsHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
PenetrationToolsHelper::InstallPriv (Ptr<Node> node) const
{
  Ptr<Application> app = m_factory.Create<Application> ();
  node->AddApplication (app);

  return app;
}

} // namespace ns3
