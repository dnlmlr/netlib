#include "resolver.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>

#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using namespace netlib;

IpAddr Resolver::resolveHostnameAF(const std::string &hostname, int af)
{
    IpAddr ipa;

    addrinfo *results;

    // This addrinfo is used as hint to filter the resolve
    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    // Filter by the address family (ipv4, ipv6 or both)
    hints.ai_family = af;
    // Sock Stream is only used to not get duplicates
    hints.ai_socktype = SOCK_STREAM;

    // Resolve the hostname into a linkedlist that is allocated into results
    if (getaddrinfo(hostname.c_str(), nullptr, &hints, &results) != 0)
    {
        throw std::runtime_error("Hostname could not be resolved");
    }

    // If no result liked list was allocated, something went wrong
    if (results == nullptr)
    {
        throw std::runtime_error("Hostname could not be resolved");
    }

    // Iterate through the linked list
    for (addrinfo *curr = results; curr != nullptr; curr = curr->ai_next)
    {
        // The current result is Ipv4
        if (curr->ai_family == AF_INET)
        {
            sockaddr_in * sa4 = (sockaddr_in*)curr->ai_addr;

            char str_addr[INET_ADDRSTRLEN];

            // Convert the raw in4_addr to string
            inet_ntop(AF_INET, &sa4->sin_addr, str_addr, INET_ADDRSTRLEN);

            ipa = IpAddr::V4(str_addr);
            break;
        }
        // The current result is Ipv6
        else if (curr->ai_family == AF_INET6)
        {
            sockaddr_in6 * sa6 = (sockaddr_in6*)curr->ai_addr;

            char str_addr[INET6_ADDRSTRLEN];

            // Convert the raw in6_addr to string
            inet_ntop(AF_INET6, &sa6->sin6_addr, str_addr, INET6_ADDRSTRLEN);

            ipa = IpAddr::V6(str_addr);
            break;
        }
    }

    // Free the allocated linked list
    freeaddrinfo(results);

    return ipa;
}

std::vector<IpAddr> Resolver::resolveHostnameAllAF(const std::string &hostname, int af)
{
    // See resolveHostnameAF for comments

    std::vector<IpAddr> ips;

    addrinfo *results;

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = af;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname.c_str(), nullptr, &hints, &results) != 0)
    {
        throw std::runtime_error("Hostname could not be resolved");
    }

    if (results == nullptr)
    {
        throw std::runtime_error("Hostname could not be resolved");
    }

    for (addrinfo *curr = results; curr != nullptr; curr = curr->ai_next)
    {
        if (curr->ai_family == AF_INET)
        {
            sockaddr_in * sa4 = (sockaddr_in*)curr->ai_addr;

            char str_addr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &sa4->sin_addr, str_addr, INET_ADDRSTRLEN);

            ips.push_back(IpAddr::V4(str_addr));
        }
        else if (curr->ai_family == AF_INET6)
        {
            sockaddr_in6 * sa6 = (sockaddr_in6*)curr->ai_addr;

            char str_addr[INET6_ADDRSTRLEN];

            inet_ntop(AF_INET6, &sa6->sin6_addr, str_addr, INET6_ADDRSTRLEN);

            ips.push_back(IpAddr::V6(str_addr));
        }
    }

    freeaddrinfo(results);

    return ips;
}

IpAddr Resolver::resolveHostnameIpv4(const std::string &hostname)
{
    return resolveHostnameAF(hostname, AF_INET);
}

IpAddr Resolver::resolveHostnameIpv6(const std::string &hostname)
{
    return resolveHostnameAF(hostname, AF_INET6);
}

std::vector<IpAddr> Resolver::resolveHostnameAllIpv4(const std::string &hostname)
{
    return resolveHostnameAllAF(hostname, AF_INET);
}

std::vector<IpAddr> Resolver::resolveHostnameAllIpv6(const std::string &hostname)
{
    return resolveHostnameAllAF(hostname, AF_INET6);
}

std::vector<IpAddr> Resolver::resolveHostnameAll(const std::string &hostname)
{
    return resolveHostnameAllAF(hostname, AF_UNSPEC);
}