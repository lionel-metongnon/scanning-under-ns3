/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2008 University of Washington
 * Copyright (c) 2011 Atishay Jain
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

#include <list>
#include "ns3/abort.h"
#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/simulation-singleton.h"
#include "ipv6-address-list.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Ipv6AddressList");

Ipv6AddressList::Ipv6AddressList ()
  : m_base ("::1")
{
  NS_LOG_FUNCTION (this);
  Reset ();
}

void
Ipv6AddressList::Reset (void)
{
  NS_LOG_FUNCTION (this);

  uint8_t prefix[16] = { 0};

  for (uint32_t i = 0; i < N_BITS; ++i)
    {
      for (uint32_t j = 0; j < 16; ++j)
        {
          m_netTable[i].prefix[j] = prefix[j];
        }
      for (uint32_t j = 0; j < 15; ++j)
        {
          prefix[15 - j] >>= 1;
          prefix[15 - j] |= (prefix[15 - j - 1] & 1);
        }
      prefix[0] |= MOST_SIGNIFICANT_BIT;
      for (uint32_t j = 0; j < 15; ++j)
        {
          m_netTable[i].network[j] = 0;
        }
      m_netTable[i].network[15] = 1;
      for (uint32_t j = 0; j < 15; ++j)
        {
          m_netTable[i].addr[j] = 0;
        }
      m_netTable[i].addr[15] = 1;
      for (uint32_t j = 0; j < 16; ++j)
        {
          m_netTable[i].addrMax[j] = ~prefix[j];
        }
      m_netTable[i].shift = N_BITS - i;
    }
  m_base = Ipv6Address ("::1");
}

Ipv6AddressList::~Ipv6AddressList ()
{
  NS_LOG_FUNCTION (this);
}

void
Ipv6AddressList::Init (
  const Ipv6Address net,
  const Ipv6Prefix prefix,
  const Ipv6Address interfaceId)
{
  NS_LOG_FUNCTION (this << net << prefix << interfaceId);

  m_base = interfaceId;
  //
  // We're going to be playing with the actual bits in the network and prefix so
  // pull them out into ints.
  //
  uint8_t prefixBits[16];
  prefix.GetBytes (prefixBits);
  uint8_t netBits[16];
  net.GetBytes (netBits);
  uint8_t interfaceIdBits[16];
  interfaceId.GetBytes (interfaceIdBits);
  //
  // Some quick reasonableness testing.
  //
  // Convert the network prefix into an index into the network number table.
  // The network number comes in to us properly aligned for the prefix and so
  // needs to be shifted right into the normalized position (lowest bit of the
  // network number at bit zero of the int that holds it).
  //
  uint32_t index = PrefixToIndex (prefix);
  NS_LOG_DEBUG ("Index " << index);
  uint32_t a = m_netTable[index].shift / 8;
  uint32_t b = m_netTable[index].shift % 8;
  for (int32_t j = 15 - a; j >= 0; j--)
    {
      m_netTable[index].network[j + a] = netBits[j];
    }
  for (uint32_t j = 0; j < a; j++)
    {
      m_netTable[index].network[j] = 0;
    }
  for (uint32_t j = 15; j >= a; j--)
    {
      m_netTable[index].network[j] = m_netTable[index].network[j] >> b;
      m_netTable[index].network[j] |= m_netTable[index].network[j - 1] << (8 - b);
    }
  for (int32_t j = 0; j < 16; j++)
    {
      m_netTable[index].addr[j] = interfaceIdBits[j];
    }
  return;
}

Ipv6Address
Ipv6AddressList::GetNetwork (
  const Ipv6Prefix prefix) const
{
  NS_LOG_FUNCTION (this);
  uint8_t nw[16];
  uint32_t index = PrefixToIndex (prefix);
  uint32_t a = m_netTable[index].shift / 8;
  uint32_t b = m_netTable[index].shift % 8;
  for (uint32_t j = 0; j < 16 - a; ++j)
    {
      nw[j] = m_netTable[index].network[j + a];
    }
  for (uint32_t j = 16 - a; j < 16; ++j)
    {
      nw[j] = 0;
    }
  for (uint32_t j = 0; j < 15; j++)
    {
      nw[j] = nw[j] << b;
      nw[j] |= nw[j + 1] >> (8 - b);
    }
  nw[15] = nw[15] << b;

  return Ipv6Address (nw);
}

Ipv6Address
Ipv6AddressList::NextNetwork (
  const Ipv6Prefix prefix)
{
  NS_LOG_FUNCTION (this);

  uint32_t index = PrefixToIndex (prefix);
  // Reset the base to what was initialized
  uint8_t interfaceIdBits[16];
  m_base.GetBytes (interfaceIdBits);
  for (int32_t j = 0; j < 16; j++)
    {
      m_netTable[index].addr[j] = interfaceIdBits[j];
    }

  for (int32_t j = 15; j >= 0; j--)
    {
      if (m_netTable[index].network[j] < 0xff)
        {
          ++m_netTable[index].network[j];
          break;
        }
      else
        {
          ++m_netTable[index].network[j];
        }
    }

  uint8_t nw[16];
  uint32_t a = m_netTable[index].shift / 8;
  uint32_t b = m_netTable[index].shift % 8;
  for (uint32_t j = 0; j < 16 - a; ++j)
    {
      nw[j] = m_netTable[index].network[j + a];
    }
  for (uint32_t j = 16 - a; j < 16; ++j)
    {
      nw[j] = 0;
    }
  for (uint32_t j = 0; j < 15; j++)
    {
      nw[j] = nw[j] << b;
      nw[j] |= nw[j + 1] >> (8 - b);
    }
  nw[15] = nw[15] << b;

  return Ipv6Address (nw);

}

Ipv6Address
Ipv6AddressList::GetAddress (const Ipv6Prefix prefix) const
{
  NS_LOG_FUNCTION (this);

  uint32_t index = PrefixToIndex (prefix);

  uint8_t nw[16];
  uint32_t a = m_netTable[index].shift / 8;
  uint32_t b = m_netTable[index].shift % 8;
  for (uint32_t j = 0; j < 16 - a; ++j)
    {
      nw[j] = m_netTable[index].network[j + a];
    }
  for (uint32_t j = 16 - a; j < 16; ++j)
    {
      nw[j] = 0;
    }
  for (uint32_t j = 0; j < 15; j++)
    {
      nw[j] = nw[j] << b;
      nw[j] |= nw[j + 1] >> (8 - b);
    }
  nw[15] = nw[15] << b;
  for (uint32_t j = 0; j < 16; j++)
    {
      nw[j] |= m_netTable[index].addr[j];
    }

  return Ipv6Address (nw);
}

Ipv6Address
Ipv6AddressList::NextAddress (const Ipv6Prefix prefix)
{
  NS_LOG_FUNCTION (this);

  uint32_t index = PrefixToIndex (prefix);

  uint8_t ad[16];
  uint32_t a = m_netTable[index].shift / 8;
  uint32_t b = m_netTable[index].shift % 8;
  for (uint32_t j = 0; j < 16 - a; ++j)
    {
      ad[j] = m_netTable[index].network[j + a];
    }
  for (uint32_t j = 16 - a; j < 16; ++j)
    {
      ad[j] = 0;
    }
  for (uint32_t j = 0; j < 15; j++)
    {
      ad[j] = ad[j] << b;
      ad[j] |= ad[j + 1] >> (8 - b);
    }
  ad[15] = ad[15] << b;
  for (uint32_t j = 0; j < 16; j++)
    {
      ad[j] |= m_netTable[index].addr[j];
    }
  Ipv6Address addr = Ipv6Address (ad);

  for (int32_t j = 15; j >= 0; j--)
    {
      if (m_netTable[index].addr[j] < 0xff)
        {
          ++m_netTable[index].addr[j];
          break;
        }
      else
        {
          ++m_netTable[index].addr[j];
        }
    }

  return addr;
}

uint32_t
Ipv6AddressList::PrefixToIndex (Ipv6Prefix prefix) const
{
  //
  // We've been given a prefix that has a higher order bit set for each bit of
  // the network number.  In order to translate this prefix into an index,
  // we just need to count the number of zero bits in the prefix.  We do this
  // in a loop in which we shift the prefix right until we find the first
  // nonzero bit.  This tells us the number of zero bits, and from this we
  // infer the number of nonzero bits which is the number of bits in the prefix.
  //
  // We use the number of bits in the prefix as the number of bits in the
  // network number and as the index into the network number state table.
  //
  uint8_t prefixBits[16];
  prefix.GetBytes (prefixBits);

  for (int32_t i = 15; i >= 0; --i)
    {
      for (uint32_t j = 0; j < 8; ++j)
        {
          if (prefixBits[i] & 1)
            {
              uint32_t index = N_BITS - (15 - i) * 8 - j;
              NS_ABORT_MSG_UNLESS (index > 0 && index < N_BITS, "Ip64AddressGenerator::PrefixToIndex(): Illegal Prefix");
              return index;
            }
          prefixBits[i] >>= 1;
        }
    }
  NS_ASSERT_MSG (false, "Ipv6AddressList::PrefixToIndex(): Impossible");
  return 0;
}

} // namespace ns3

