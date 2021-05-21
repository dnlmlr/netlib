#include "tcplistener.hpp"

#include <stdexcept>

#include <unistd.h>
#include <sys/socket.h>

using namespace netlib;

// TODO: More and better error handling

TcpListener::TcpListener(SockAddr _local)
    : local{_local}, sockfd{0}
{ }

TcpListener::TcpListener(IpAddr localAddress, uint16_t port)
    : TcpListener{SockAddr{localAddress, port}}
{ }

TcpListener::TcpListener(const std::string &localAddress, uint16_t port)
    : TcpListener{SockAddr{localAddress, port}}
{ }

TcpListener::TcpListener(const std::string &localAddressPort)
    : TcpListener{SockAddr{localAddressPort}}
{ }

TcpListener::~TcpListener()
{
    // Close the socket to prevent leaking open file descriptors
    if (autoclose) close();
}

TcpListener::TcpListener(TcpListener &&other)
    : local{other.local}, sockfd{other.sockfd}, autoclose{other.autoclose}
{
    // Invalidate other socket
    other.sockfd = 0;
}

TcpListener& TcpListener::operator=(TcpListener &&other)
{
    local = other.local;
    sockfd = other.sockfd;
    autoclose = other.autoclose;

    // Invalidate other socket
    other.sockfd = 0;

    return *this;
}

void TcpListener::listen(int connectionQueue)
{
    if (sockfd > 0)
        throw std::runtime_error("Can't call listen on open socket");

    int af;
    socklen_t sock_len;

    if (local.address.type == IpAddr::Type::V4)
    {
        af = AF_INET;
        sock_len = sizeof(sockaddr_in);
    }
    else if (local.address.type == IpAddr::Type::V6)
    {
        af = AF_INET6;
        sock_len = sizeof(sockaddr_in6);
    }
    else
    {
        throw std::runtime_error("Can't bind to IpAddr::Type::Undef");
    }

    // Create the socket and get the socket file descriptor
    sockfd = socket(af, SOCK_STREAM, 0);

    if (sockfd <= 0)
    {
        throw std::runtime_error("Creating TCP Socket failed");
    }

    // Bind the socket to the local address and port
    if (bind(sockfd, &local.raw_sockaddr.generic, sock_len) != 0)
    {
        close();
        throw std::runtime_error("Binding TCP Socket failed");
    }

    // Start listening on the local address and port with the specified number 
    // queued connections
    if (::listen(sockfd, connectionQueue) != 0)
    {
        close();
        throw std::runtime_error("Listening TCP Socket failed");
    }

}

TcpStream TcpListener::accept()
{
    SockAddr::RawSockAddr remote_raw_saddr = {0};
    socklen_t remote_raw_saddr_len = sizeof(remote_raw_saddr);

    int remote_sockfd = ::accept(sockfd, &remote_raw_saddr.generic, &remote_raw_saddr_len);
    if (remote_sockfd <= 0)
    {
        throw std::runtime_error("Accepting TCP Connection failed");
    }

    // Parse the raw remote sockaddr to a SockAddr
    SockAddr remote_saddr(&remote_raw_saddr.generic, local.address.type);

    // Create a TcpStream and set the remote SockAddr
    TcpStream stream(remote_saddr);
    // Transfer the socket filedescriptor
    stream.sockfd = remote_sockfd;

    // TcpStream can't be copied, so this has to move
    return stream;
}

void TcpListener::close()
{
    if (sockfd != 0)
    {
        ::close(sockfd);
    }

    sockfd = 0;
}

void TcpListener::setAutoclose(bool _autoclose)
{
    autoclose = _autoclose;
}

TcpListener TcpListener::clone()
{
    TcpListener other{local};
    other.sockfd = sockfd;
    other.autoclose = autoclose;

    return other;
}