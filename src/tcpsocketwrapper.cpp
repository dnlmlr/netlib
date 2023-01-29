#include "tcpsocketwrapper.hpp"

using namespace netlib;

TcpSocketWrapper::TcpSocketWrapper() : sockfd{0}
{ }

TcpSocketWrapper::TcpSocketWrapper(int sockfd) : sockfd{sockfd}
{ }

#ifdef NETLIB_SSL

TcpSocketWrapper::TcpSocketWrapper(int sockfd, SSL *ssl) : sockfd{sockfd}, ssl{ssl}
{
}

#endif // NETLIB_SSL

TcpSocketWrapper::~TcpSocketWrapper()
{
    close();
}

TcpSocketWrapper::TcpSocketWrapper(TcpSocketWrapper &&other)
{
    sockfd = other.sockfd;
    other.sockfd = 0;

#ifdef NETLIB_SSL
    ssl = other.ssl;
    other.ssl = nullptr;
#endif // NETLIB_SSL
}

TcpSocketWrapper& TcpSocketWrapper::operator=(TcpSocketWrapper &&other)
{
    // If the left hand side of the assignement (this) has an open connection,
    // it should be closed to prevent leaking filedescriptors and SSL objects.
    close();

    sockfd = other.sockfd;
    other.sockfd = 0;
#ifdef NETLIB_SSL
    ssl = other.ssl;
    other.ssl = nullptr;
#endif // NETLIB_SSL
    return *this;
}

ssize_t TcpSocketWrapper::read(void *data, size_t len) const
{
#ifdef NETLIB_SSL
    if (ssl != nullptr)
        return SSL_read(ssl, data, len);
#endif // NETLIB_SSL

    return ::read(sockfd, data, len);
}

ssize_t TcpSocketWrapper::write(const void *data, size_t len) const
{
#ifdef NETLIB_SSL
    if (ssl != nullptr)
        return SSL_write(ssl, data, len);
#endif // NETLIB_SSL

    return ::write(sockfd, data, len);
}

void TcpSocketWrapper::close()
{
#ifdef NETLIB_SSL
    if (ssl != nullptr)
    {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        ssl = nullptr;
    }
#endif // NETLIB_SSL

    if (sockfd != 0)
    {
        ::close(sockfd);
        sockfd = 0;
    }
}

bool TcpSocketWrapper::isValid() const
{
    return sockfd != 0;
}
