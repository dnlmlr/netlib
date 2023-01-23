/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#ifndef _IPADDR_HPP
#define _IPADDR_HPP

#include <string>
#include <netinet/in.h>

namespace netlib
{


/**
 * @brief The IpAddr class represents an ip address that can either be of type
 * Ipv4 or of type Ipv6.
 */
class IpAddr
{
private:

    /**
     * @brief The type of this IpAddr (either Ipv4, Ipv6 or undefined)
     */
    enum class Type {
        /** @brief The IpAddr represents an Ipv4 address. */
        V4,
        
        /** @brief The IpAddr represents an Ipv6 address. */
        V6,
        
        /** @brief The IpAddr has not been set, so the address type is undefined. */
        Undef
    };

    /**
     * @brief The low level raw in_addr or in6_addr struct, depending on the 
     * type.
     * 
     * If the IpAddr type is V4, this is in_addr, if the type is V6, this is 
     * in6_addr. 
     */
    union {
        /** @brief Raw struct representing an ipv4 address */
        in_addr v4;
        /** @brief Raw struct representing an ipv6 address */
        in6_addr v6;
    } raw_addr;

    /**
     * @brief The ip address in string representation
     */
    std::string str_addr;

    /**
     * @brief The ip address type (Ipv4, Ipv6 or undefined)
     */
    Type type;

public:

    /**
     * @brief The default constructor creates the Ipv4 address "0.0.0.0".
     */
    IpAddr();

    /**
     * @brief Create an IpAddr from the given string representation. The 
     * address type is determined automatically. 
     * 
     * If string can't be parsed as either Ipv4 or Ipv6 address, an exception 
     * is thrown.
     * 
     * @param address The string representing an ip address. 
     */
    IpAddr(const std::string &address);

    /**
     * @brief Create an Ipv4 IpAddr from the given string representation. 
     * 
     * If the string cannot be parsed as an Ipv4 address, an exception is 
     * thrown.
     * 
     * @param address The string representing an Ipv4 ip address. 
     * 
     * @returns The newly constructed IpAddr object.
     */
    static IpAddr V4(const std::string &address);

    /**
     * @brief Create an Ipv6 IpAddr from the given string representation. 
     * 
     * If the string cannot be parsed as an Ipv6 address, an exception is 
     * thrown.
     * 
     * @param address The string representing an Ipv6 ip address. 
     * 
     * @return The newly constructed IpAddr object.
     */
    static IpAddr V6(const std::string &address);

    /**
     * @brief Check if the IpAddr is of type Ipv4.
     * 
     * @return True if it is an Ipv4 address, false otherwise.
     */
    bool isIpv4() const;

    /**
     * @brief Check if the IpAddr is of type Ipv6.
     * 
     * @return True if it is an Ipv6 address, false otherwise.
     */
    bool isIpv6() const;

    /**
     * @brief Check if the IpAddr is of type undefined.
     * 
     * @return True if the address is not defined, false otherwise.
     */
    bool isUndefined() const;

    /**
     * @brief Get the string representation of the IpAddr.
     * 
     * @returns A string that represents the IpAddr. The string is in the same
     * form as it was provided while constructing the address.
     */
    const std::string & getAddressString() const;

    friend class SockAddr;
    friend class TcpStream;
    friend class TcpListener;
    friend class UdpSocket;
    friend class Resolver;

};


} // namespace netlib

#endif // _IPADDR_HPP