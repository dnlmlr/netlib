#include "sockaddr.hpp"

#include <cstring>
#include <stdexcept>

#include <arpa/inet.h>

using namespace netlib;

SockAddr::SockAddr()
    : SockAddr{IpAddr::V4("0.0.0.0"), 0}
{ }

SockAddr::SockAddr(IpAddr _address, uint16_t _port)
    : address{_address}, port{_port}
{
    setRawSockaddr(address, port);
}

SockAddr::SockAddr(const std::string &_address, uint16_t _port)
    : SockAddr(IpAddr(_address), _port)
{ }

SockAddr::SockAddr(const std::string &_address_port)
{
    std::string str_addr;
    std::string str_port;

    // Try to find the positions of the brackets and colons
    size_t pos_bracket1 = _address_port.find("[");
    size_t pos_bracket2 = _address_port.find("]:");
    size_t pos_colon = _address_port.find(":");

    // If no colon was found at all, the port is definitely missing
    if (pos_colon == std::string::npos)
    {
        throw std::runtime_error("SockAddr ip:port string is missing \":port\"");
    }

    // If the string starts with "[" and also contains "]:", it must be Ipv6
    if (pos_bracket1 == 0 && pos_bracket2 != std::string::npos)
    {
        str_addr = _address_port.substr(1, pos_bracket2-1);
        str_port = _address_port.substr(pos_bracket2+2);
        
        address = IpAddr::V6(str_addr);
    }
    // If the string contains "." it must be Ipv4
    else if (_address_port.find(".") != std::string::npos)
    {
        str_addr = _address_port.substr(0, pos_colon);
        str_port = _address_port.substr(pos_colon+1);
        
        address = IpAddr::V4(str_addr);
    }
    // If it is neither Ipv4 nor Ipv6, it is invalid
    else
    {
        throw std::runtime_error("Conversion from String to SockAddr failed");
    }

    // Convert the port string to int
    int port_int = std::stoi(str_port);
    // If the port is not in the range of uint16_t, it is invalid
    if (port_int < 0 || port_int > 0xffff || str_port.empty())
        throw std::runtime_error("Invalid port number");

    port = port_int;

    // Transfer the information onto the raw sockaddr
    setRawSockaddr(address, port);
}

SockAddr::SockAddr(sockaddr *_raw_sockaddr, IpAddr::Type _type)
{

    if (_type == IpAddr::Type::V4)
    {
        // Type-pun the sockaddr to Ipv4 sockaddr_in
        raw_sockaddr.v4 = *((sockaddr_in*)_raw_sockaddr);

        // Get the port in host byte order
        port = ntohs(raw_sockaddr.v4.sin_port);

        // Set the address type
        address.type = _type;
        // Copy the raw address data
        address.raw_addr.v4 = raw_sockaddr.v4.sin_addr;

        // Buffer to hold the string representation of the Ipv4 address
        char str_addr[INET_ADDRSTRLEN];

        // Convert the raw address to string
        inet_ntop(AF_INET, &raw_sockaddr.v4.sin_addr, str_addr, INET_ADDRSTRLEN);

        // Set the string in address
        address.str_addr = str_addr;
    }
    else if (_type == IpAddr::Type::V6)
    {
        // Same as if the type was Ipv4, but with a 6 instead

        raw_sockaddr.v6 = *((sockaddr_in6*)_raw_sockaddr);

        port = ntohs(raw_sockaddr.v6.sin6_port);

        address.type = _type;
        address.raw_addr.v6 = raw_sockaddr.v6.sin6_addr;

        char str_addr[INET6_ADDRSTRLEN] = {0};

        inet_ntop(AF_INET6, &raw_sockaddr.v6.sin6_addr, str_addr, INET6_ADDRSTRLEN);

        address.str_addr = str_addr;
    }
    else
    {
        throw std::runtime_error("Can't build SockAddr from IpAddr::Type::Undef");
    }

}

void SockAddr::setRawSockaddr(IpAddr _address, uint16_t _port)
{
    memset(&raw_sockaddr, 0, sizeof(raw_sockaddr));

    // Set the information in the raw sockaddr
    if (_address.isIpv4())
    {
        // Set address family to Ipv4
        raw_sockaddr.v4.sin_family = AF_INET;
        // Set the port (in network byte order)
        raw_sockaddr.v4.sin_port = htons(_port);
        // Copy the raw ip address from IpAddr
        raw_sockaddr.v4.sin_addr = _address.raw_addr.v4;
    }
    else if (_address.isIpv6())
    {
        // Set address family to Ipv6
        raw_sockaddr.v6.sin6_family = AF_INET6;
        // Set the port (in network byte order)
        raw_sockaddr.v6.sin6_port = htons(_port);
        // Copy the raw ip address from IpAddr
        raw_sockaddr.v6.sin6_addr = _address.raw_addr.v6;
    }
    else
    {
        throw std::runtime_error("Can't create SockAddr from IpAddr::Type::Undef");
    }
}

const IpAddr & SockAddr::getIpAddress() const
{
    return address;
}

const std::string & SockAddr::getIpAddressString() const
{
    return address.getAddressString();
}

const uint16_t SockAddr::getPort() const
{
    return port;
}