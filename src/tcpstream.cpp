/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#include "tcpstream.hpp"

#include <stdexcept>

#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>

using namespace netlib;

TcpStream::TcpStream()
    : remote{SockAddr{IpAddr::V4("0.0.0.0"), 0}}, socket{nullptr}
{ }

TcpStream::TcpStream(SockAddr _remote)
    : remote{_remote}, socket{nullptr}
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
    if (autoclose) close();
}

TcpStream::TcpStream(TcpStream &&other)
    : remote{other.remote}, socket{std::move(other.socket)}, autoclose{other.autoclose}
{ }

TcpStream& TcpStream::operator=(TcpStream &&other)
{
    remote = other.remote;
    socket = std::move(other.socket);
    autoclose = other.autoclose;

    return *this;
}

void TcpStream::setRemote(SockAddr remote)
{
    if (isSocketValid())
        throw std::runtime_error("Can't change remote while having an open socket");
    this->remote = remote;
}

bool TcpStream::isSocketValid() const
{
    return (socket != nullptr) && socket->isValid();
}

void TcpStream::connect()
{
    if (isSocketValid())
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
    int sockfd = ::socket(af, SOCK_STREAM, 0);
    if (sockfd <= 0)
    {
        throw std::runtime_error("Creating TCP Socket failed");
    }

    socket = std::make_shared<TcpSocketWrapper>(TcpSocketWrapper{sockfd});

    if (::connect(sockfd, &remote.raw_sockaddr.generic, sock_len) != 0)
    {
        close();
        throw std::runtime_error("Connecting TCP Socket failed");
    }

}


#ifdef NETLIB_SSL
void TcpStream::connect(SSL_CTX *ctx)
{
    this->connect();

    SSL *ssl = SSL_new(ctx);
    if (ssl == nullptr)
    {
        close();
        throw std::runtime_error("Creating SSL structure failed");
    }

    socket->ssl = ssl;

    if (SSL_set_fd(ssl, socket->sockfd) != 1)
    {
        close();
        throw std::runtime_error("Binding SSL to socket failed");
    }

    if (SSL_connect(ssl) != 1)
    {
        close();
        throw std::runtime_error("SSL handshake failed");
    }
}
#endif // NETLIB_SSL

void TcpStream::close()
{
    if (socket == nullptr) return;

    if (isSocketValid())
    {
        socket->close();
    }
}

ssize_t TcpStream::send(const void *data, size_t len)
{
    if (!isSocketValid())
        throw std::runtime_error("Can't write to closed socket");
    
    ssize_t bytes_sent = socket->write(data, len);

    if (bytes_sent < 0)
    {
        close();
        throw std::runtime_error("Error while writing to socket");
    }

    return bytes_sent;
}

void TcpStream::sendAll(const void *data, size_t len)
{
    if (!isSocketValid())
        throw std::runtime_error("Can't write to closed socket");
    
    size_t bytesSentTotal = 0;

    while (bytesSentTotal < len)
    {
        ssize_t bytesSent = socket->write((uint8_t*)data + bytesSentTotal, len-bytesSentTotal);

        if (bytesSent < 0)
        {
            close();
            throw std::runtime_error("Error while writing to socket");
        }

        bytesSentTotal += bytesSent;
    }
}

void TcpStream::sendAllString(const std::string &str)
{
    sendAll(str.c_str(), str.size());
}

ssize_t TcpStream::read(void *data, size_t len)
{
    if (!isSocketValid())
        throw std::runtime_error("Can't read from closed socket");

    ssize_t bytes_read = socket->read(data, len);
    if (bytes_read < 0)
    {
        close();
        throw std::runtime_error("Error while reading from socket");
    }
    return bytes_read;
}

ssize_t TcpStream::readAll(void *data, size_t len)
{
    if (!isSocketValid())
        throw std::runtime_error("Can't read from closed socket");
    
    size_t bytesReadTotal = 0;
    while (true)
    {
        ssize_t bytesRead = socket->read((uint8_t*)data + bytesReadTotal, len-bytesReadTotal);

        if (bytesRead == 0) break;
        if (bytesRead < 0)
        {
            close();
            throw std::runtime_error("Error while reading from socket");
        }

        bytesReadTotal += bytesRead;
    }
    return bytesReadTotal;
}

ssize_t TcpStream::readTimeout(void *data, size_t len, int timeoutMs)
{
    if (timeoutMs <= 0) return read(data, len);

    if (!isSocketValid())
        throw std::runtime_error("Can't read from closed socket");

    pollfd pfd;
    std::memset(&pfd, 0, sizeof(pollfd));

    pfd.fd = socket->sockfd;
    pfd.events = POLLIN;
    
    // block until data is available or the timeout is reached
    int res = poll(&pfd, 1, timeoutMs);

    // a timout occured
    if (res == 0) return 0;

    // a poll error occured
    if (res < 0)
    {
        close();
        throw std::runtime_error("Error while reading from socket");
    }

    ssize_t bytes_read = socket->read(data, len);
    if (bytes_read < 0)
    {
        close();
        throw std::runtime_error("Error while reading from socket");
    }
    return bytes_read;
}

ssize_t TcpStream::readAllTimeout(void *data, size_t len, int timeoutMs)
{
    if (timeoutMs <= 0) return readAll(data, len);

    if (!isSocketValid())
        throw std::runtime_error("Can't read from closed socket");
    
    pollfd pfd;
    std::memset(&pfd, 0, sizeof(pollfd));

    pfd.fd = socket->sockfd;
    pfd.events = POLLIN;
    
    
    size_t bytesReadTotal = 0;
    while (true)
    {
        // block until data is available or the timeout is reached
        int res = poll(&pfd, 1, timeoutMs);

        // a timout occured
        if (res == 0) return -1 * bytesReadTotal;

        // a poll error occured
        if (res < 0)
        {
            close();
            throw std::runtime_error("Error while reading from socket");
        }

        ssize_t bytesRead = socket->read((uint8_t*)data + bytesReadTotal, len-bytesReadTotal);

        if (bytesRead == 0) break;
        if (bytesRead < 0)
        {
            close();
            throw std::runtime_error("Error while reading from socket");
        }

        bytesReadTotal += bytesRead;
    }
    return bytesReadTotal;
}

const SockAddr & TcpStream::getRemoteAddr() const
{
    return remote;
}

#ifdef NETLIB_SSL

const SSL *TcpStream::getSSL() const
{
    if (!isSocketValid())
    {
        return nullptr;
    }
    return socket->ssl;
}
#endif // NETLIB_SSL

bool TcpStream::isClosed() const
{
    return !isSocketValid();
}

void TcpStream::setAutoclose(bool _autoclose)
{
    autoclose = _autoclose;
}

TcpStream TcpStream::clone() const
{
    TcpStream other{remote};
    other.socket = socket;
    other.autoclose = autoclose;

    return other;
}