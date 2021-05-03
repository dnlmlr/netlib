#include "tcpstream.hpp"

#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

TcpStream::TcpStream(SockAddr _remote)
    : remote{_remote}, sockfd{0}
{ }
    
TcpStream::TcpStream(IpAddr remoteAddress, uint16_t port)
    : TcpStream{SockAddr{remoteAddress, port}}
{ }

TcpStream::TcpStream(const std::string &remoteAddress, uint16_t port)
    : TcpStream{SockAddr{remoteAddress, port}}
{ }

TcpStream::TcpStream(const std::string &remoteAddressPort)
    : TcpStream{SockAddr{remoteAddressPort}}
{ }

TcpStream::~TcpStream()
{
    close();
}

void TcpStream::connect()
{
    if (sockfd != 0)
        throw std::runtime_error("Can't call connect on open socket");

    int af;
    socklen_t sock_len;

    if (remote.address.type == IpAddr::Type::V4)
    {
        af = AF_INET;
        sock_len = sizeof(sockaddr_in);
    }
    else if (remote.address.type == IpAddr::Type::V6)
    {
        af = AF_INET6;
        sock_len = sizeof(sockaddr_in6);
    }
    else
    {
        throw std::runtime_error("Can't connect to IpAddr::Type::Undef");
    }

    // Create the socket and get the socket file descriptor
    sockfd = socket(af, SOCK_STREAM, 0);

    if (sockfd <= 0)
    {
        throw std::runtime_error("Creating TCP Socket failed");
    }

    if (::connect(sockfd, &remote.raw_sockaddr.generic, sock_len) != 0)
    {
        close();
        throw std::runtime_error("Connecting TCP Socket failed");
    }

}

void TcpStream::close()
{
    if (sockfd != 0)
    {
        ::close(sockfd);
    }

    sockfd = 0;
}

ssize_t TcpStream::send(const void *data, size_t len)
{
    if (sockfd == 0)
        throw std::runtime_error("Can't write to closed socket");
    
    ssize_t bytes_sent = ::write(sockfd, data, len);

    if (bytes_sent < 0)
    {
        close();
        throw std::runtime_error("Error while writing to socket");
    }

    return bytes_sent;
}

ssize_t TcpStream::read(void *data, size_t len)
{
    if (sockfd == 0)
        throw std::runtime_error("Can't read from closed socket");

    ssize_t bytes_read = ::read(sockfd, data, len);
    if (bytes_read < 0)
    {
        close();
        throw std::runtime_error("Error while reading from socket");
    }
    return bytes_read;
}

const SockAddr & TcpStream::getRemoteAddr() const
{
    return remote;
}

bool TcpStream::isClosed() const
{
    return sockfd == 0;
}