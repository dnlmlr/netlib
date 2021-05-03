#include "udpsocket.hpp"

#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>


UdpSocket::UdpSocket(SockAddr _local)
    : local{_local}, sockfd{0}
{
    if (local.address.type == IpAddr::Type::V4)
    {
        raw_socklen = sizeof(sockaddr_in);
        address_family = AF_INET;
    }
    else if (local.address.type == IpAddr::Type::V6)
    {
        raw_socklen = sizeof(sockaddr_in6);
        address_family = AF_INET6;
    }
    else
    {
        throw std::runtime_error("Cant create UdpSocket from IpAddr::Type::Undef");
    }
}

UdpSocket::UdpSocket(IpAddr localAddress, uint16_t port)
    : UdpSocket{SockAddr{localAddress, port}}
{ }

UdpSocket::UdpSocket(const std::string &localAddress, uint16_t port)
    : UdpSocket{SockAddr{localAddress, port}}
{ }

UdpSocket::UdpSocket(const std::string &localAddressPort)
    : UdpSocket{SockAddr{localAddressPort}}
{ }

UdpSocket::~UdpSocket()
{
    close();
}

void UdpSocket::bind()
{
    if (sockfd > 0)
            throw std::runtime_error("Can't call bind on open socket");

    sockfd = socket(address_family, SOCK_DGRAM, 0);

    if (sockfd <= 0)
    {
        throw std::runtime_error("Creating UDP Socket failed");
    }

    if (::bind(sockfd, &local.raw_sockaddr.generic, raw_socklen) != 0)
    {
        close();
        throw std::runtime_error("Binding UDP Socket failed");
    }

}

ssize_t UdpSocket::sendTo(const SockAddr &remote, const void *data, size_t len)
{
    if (remote.address.type != local.address.type)
    {
        throw std::runtime_error("Can only send to remote addresses with the same ip type");
    }
    
    // TODO: Lookup flags
    ssize_t bytes_sent = ::sendto(sockfd, data, len, 0, &remote.raw_sockaddr.generic, raw_socklen);

    if (bytes_sent < 0)
    {
        throw std::runtime_error("Error while writing to socket");
    }

    return bytes_sent;
}

ssize_t UdpSocket::sendTo(const std::string &remoteAddr, uint16_t port, const void *data, size_t len)
{
    return sendTo(SockAddr(remoteAddr, port), data, len);
}

ssize_t UdpSocket::sendTo(const std::string &remoteAddrPort, const void *data, size_t len)
{
    return sendTo(SockAddr(remoteAddrPort), data, len);
}

ssize_t UdpSocket::receive(void *data, size_t len, SockAddr &remote)
{
    SockAddr::RawSockAddr remote_raw_saddr = {0};

    // TODO: Lookup flags
    ssize_t bytes_read = ::recvfrom(sockfd, data, len, 0, &remote_raw_saddr.generic, &raw_socklen);

    if (bytes_read < 0)
    {
        throw std::runtime_error("Error while reading from socket");
    }

    remote = SockAddr(&remote_raw_saddr.generic, local.address.type);

    return bytes_read;
}

ssize_t UdpSocket::receive(void *data, size_t len)
{
    SockAddr::RawSockAddr remote_raw_saddr = {0};

    // TODO: Lookup flags
    ssize_t bytes_read = ::recvfrom(sockfd, data, len, 0, &remote_raw_saddr.generic, &raw_socklen);

    if (bytes_read < 0)
    {
        throw std::runtime_error("Error while reading from socket");
    }

    return bytes_read;
}

void UdpSocket::close()
{
    if (sockfd != 0)
    {
        ::close(sockfd);
    }
    sockfd = 0;
}