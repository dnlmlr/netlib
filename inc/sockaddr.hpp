#ifndef _SOCKADDR_HPP
#define _SOCKADDR_HPP

#include <string>
#include <netinet/in.h>

#include "ipaddr.hpp"

namespace netlib
{


/**
 * @brief SockAddr represents the combination of an ip address and a port number.
 * 
 * @note This implementation of SockAddr does not contain the transfer protocol.
 */
class SockAddr
{
public:

    /**
     * @brief RawSockAddr can be interpreted as either the generic sockaddr, 
     * the Ipv4 sockaddr_in or the Ipv6 sockaddr_in6.
     * 
     * This type is used to simplify the type punning required for working 
     * with the network stack.
     */
    union RawSockAddr {
        /** @brief Interpret the memory as generic sockaddr */
        sockaddr generic;
        /** @brief Interpret the memory as Ipv4 sockaddr_in */
        sockaddr_in v4;
        /** @brief Interpret the memory as Ipv6 sockaddr_in6 */
        sockaddr_in6 v6;
    };

private:

    /**
     * @brief The ip address associated with the SockAddr.
     */
    IpAddr address;

    /**
     * @brief The port number associated with the SockAddr.
     */
    uint16_t port;

    /**
     * @brief The raw sockaddr, used for interfacing with the network stack.
     * 
     * This must represent the same state as the SockAddr IpAddr & port at 
     * all times.
     */
    RawSockAddr raw_sockaddr;

    /**
     * @brief Parse the given IpAddr + port and set the raw sockaddr accordingly.
     * 
     * @param address The ip address that will be used in the raw socket.
     * @param port The port number taht will be used in the raw socket.
     */
    void setRawSockaddr(IpAddr address, uint16_t port);

    /**
     * @brief Create the SockAddr by parsing a given raw sockaddr with 
     * specified type.
     * 
     * This extracts the IpAddr with the specified type, as well as the prot 
     * from the sockaddr.
     * 
     * @param raw_sockaddr Pointer to the raw sockaddr that should be parsed.
     * @param type The ip type that raw_sockaddr contains. Must be Ipv4 or Ipv6.
     */
    SockAddr(sockaddr *raw_sockaddr, IpAddr::Type type);

public:

    /**
     * @brief The default constructor creates a SockAddr with the Ipv4 address 
     * 0.0.0.0 and the port 0.
     */
    SockAddr();

    /**
     * @brief Construct a SockAddr from IpAddr and port.
     * 
     * @param address The ip address.
     * @param port The port.
     */
    SockAddr(IpAddr address, uint16_t port);

    /**
     * @brief Construct SockAddr from a string and a port. The string will be 
     * parsed as in IpAddr automatically. 
     * 
     * If the address cannot be parsed as a valid Ipv4 or Ipv6 address, an 
     * exception is thrown.
     * 
     * @param address The ip address in string representation. This can be Ipv4 
     * or Ipv6.
     * @param port The port.
     */
    SockAddr(const std::string & address, uint16_t port);

    /**
     * @brief Construct SockAddr from a string containing both the address and 
     * port. The string must be in the form "address:port". To specify an Ipv6 
     * address, it has to be placed inside brackets "[ipv6addr]:port".
     * 
     * If any part of the string fails to be parsed, an exception is thrown.
     * 
     * Exampe for Ipv4: "127.0.0.1:8080". Example for Ipv6: "[::1]:8080".
     * 
     * @param address_port The ip address and port combination as string.
     */
    SockAddr(const std::string & address_port);

    /**
     * @brief Get the ip address.
     */
    const IpAddr & getIpAddress() const;

    /**
     * @brief Get the ip address in string form.
     */
    const std::string & getIpAddressString() const;

    /**
     * @brief Get the port number.
     */
    const uint16_t getPort() const;

    friend class TcpStream;
    friend class TcpListener;
    friend class UdpSocket;

};


} // namespace netlib

#endif // _SOCKADDR_HPP