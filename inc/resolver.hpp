/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#ifndef _RESOLVER_HPP
#define _RESOLVER_HPP

#include <vector>

#include "ipaddr.hpp"

namespace netlib
{


/**
 * @brief Provide multiple ways to resolve hostnames to Ipv4 and/or Ipv6 
 * addresses.
 */
class Resolver
{
private:

    /**
     * @brief Resolve a given hostname to first ip address that is found. The 
     * address family can be specified to narrow the resolve to specifically 
     * only Ipv4, Ipv6 or both.
     * 
     * If the hostname could not be resolved, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * @param address_family The address family to be used as filter. Should be
     * AF_INET for Ipv4 only, AF_INET6 for Ipv6 only or AF_UNSPEC for both.
     * 
     * @return The first IpAddress that was resolved for the given hostname.
     */
    static IpAddr resolveHostnameAF(const std::string &hostname, int address_family);

    /**
     * @brief Resolve a given hostname and get all associated ip addresses. The 
     * address family can be specified to narrow the resolve to specifically 
     * only Ipv4, Ipv6 or both.
     * 
     * If the hostname could not be resolved at all, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * @param address_family The address family to be used as filter. Should be
     * AF_INET for Ipv4 only, AF_INET6 for Ipv6 only or AF_UNSPEC for both.
     * 
     * @return A list of all ip addresses resolved by the hostname. If 
     * address_family filters only certain types, this only contains those.
     */
    static std::vector<IpAddr> resolveHostnameAllAF(const std::string &hostname, int address_family);

public:

    /**
     * @brief Resolve a given hostname to first Ipv4 ip address that is found.
     * 
     * If the hostname could not be resolved, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * 
     * @return The first IpAddress that was resolved for the given hostname.
     */
    static IpAddr resolveHostnameIpv4(const std::string &hostname);

    /**
     * @brief Resolve a given hostname to first Ipv6 ip address that is found.
     * 
     * If the hostname could not be resolved, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * 
     * @return The first IpAddress that was resolved for the given hostname.
     */
    static IpAddr resolveHostnameIpv6(const std::string &hostname);

    /**
     * @brief Resolve a given hostname and get all associated Ipv4 ip addresses.
     * 
     * If the hostname could not be resolved at all, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * 
     * @return A list of all Ipv4 ip addresses resolved by the hostname.
     */
    static std::vector<IpAddr> resolveHostnameAllIpv4(const std::string &hostname);

    /**
     * @brief Resolve a given hostname and get all associated Ipv6 ip addresses.
     * 
     * If the hostname could not be resolved at all, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * 
     * @return A list of all Ipv6 ip addresses resolved by the hostname.
     */
    static std::vector<IpAddr> resolveHostnameAllIpv6(const std::string &hostname);

    /**
     * @brief Resolve a given hostname and get all associated ip addresses. 
     * This contains both Ipv4 and Ipv6 ip addresses.
     * 
     * If the hostname could not be resolved at all, an exception is thrown.
     * 
     * @param hostname The string representing the hostname that should be 
     * resolved.
     * 
     * @return A list of all Ipv4 and Ipv6 ip addresses resolved by the hostname.
     */
    static std::vector<IpAddr> resolveHostnameAll(const std::string &hostname);

};


} // namespace netlib

#endif // _RESOLVER_HPP