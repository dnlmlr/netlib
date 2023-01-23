/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#include "ipaddr.hpp"

#include <stdexcept>

#include <cstring>
#include <arpa/inet.h>

using namespace netlib;

IpAddr::IpAddr()
    : IpAddr("0.0.0.0")
{ }

IpAddr::IpAddr(const std::string &address)
    : str_addr{address}
{
    // Clear the raw address memory
    memset(&raw_addr, 0, sizeof(raw_addr));

    if (inet_pton(AF_INET, address.c_str(), &raw_addr.v4) == 1)
    { // The string address was able to be parsed as Ipv4 address
        type = Type::V4;
    }
    else if (inet_pton(AF_INET6, address.c_str(), &raw_addr.v6) == 1)
    { // The string address was able to be parsed as Ipv6 address
        type = Type::V6;
    }
    else
    { // The string address is neither a valid Ipv4, nor Ipv6 address
        throw std::runtime_error("IpAddr conversion from string failed");
    }
    
}

IpAddr IpAddr::V4(const std::string &address)
{
    IpAddr ip_addr;
    ip_addr.type = Type::V4;
    ip_addr.str_addr = address;
    
    memset(&ip_addr.raw_addr, 0, sizeof(ip_addr.raw_addr));
    
    if (inet_pton(AF_INET, address.c_str(), &ip_addr.raw_addr.v4) != 1)
    {
        throw std::runtime_error("IpAddrV4 conversion from string failed");
    }

    return ip_addr;
}

IpAddr IpAddr::V6(const std::string &address)
{
    IpAddr ip_addr;
    ip_addr.type = Type::V6;
    ip_addr.str_addr = address;
    
    memset(&ip_addr.raw_addr, 0, sizeof(ip_addr.raw_addr));
    
    if (inet_pton(AF_INET6, address.c_str(), &ip_addr.raw_addr.v6) != 1)
    {
        throw std::runtime_error("IpAddrV6 conversion from string failed");
    }

    return ip_addr;
}


bool IpAddr::isIpv4() const
{
    return type == Type::V4;
}

bool IpAddr::isIpv6() const
{
    return type == Type::V6;
}

bool IpAddr::isUndefined() const
{
    return type == Type::Undef;
}

const std::string & IpAddr::getAddressString() const
{
    return str_addr;
}
