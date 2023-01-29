/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#ifndef _TCPSOCKETWRAPPER_HPP
#define _TCPSOCKETWRAPPER_HPP

#include <unistd.h>

#ifdef NETLIB_SSL
#include <openssl/ssl.h>
#endif // NETLIB_SSL

namespace netlib
{

/**
 * @brief Wrapper around a socket file discriptor and an optional SSL pointer. This provides
 * unified read, write and close functions that work with and without an active SSL connection.
 * If this object is dropped (destroyed), the connection will be automatically closed.
 * 
 * @note This is an internal wrapper class and is not indended to be used directly.
 */
class TcpSocketWrapper
{
public:
    int sockfd;

#ifdef NETLIB_SSL

    SSL *ssl = nullptr;

    TcpSocketWrapper(int sockfd, SSL *ssl);

#endif // NETLIB_SSL

    TcpSocketWrapper();
    TcpSocketWrapper(int sockfd);

    ~TcpSocketWrapper();

    TcpSocketWrapper(TcpSocketWrapper &&other);
    TcpSocketWrapper& operator=(TcpSocketWrapper &&other);

    TcpSocketWrapper(const TcpSocketWrapper &other) = delete;
    TcpSocketWrapper& operator=(const TcpSocketWrapper &other) = delete;

    /**
     * @brief Call either write or SSL_write on the underlying connection, depending on whether 
     * the ssl context is set, or not.
     * 
     * @note This does not check if the wrapped socket is valid or not!
     */
    ssize_t write(const void *data, size_t len) const;

    /**
     * @brief Call either read or SSL_read on the underlying connection, depending on whether 
     * the ssl context is set, or not.
     * 
     * @note This does not check if the wrapped socket is valid or not!
     */
    ssize_t read(void *data, size_t len) const;

    /**
     * @brief Returns true if the underlying socketfd is set to something other than 0. This
     * does not check if the connection is broken.
     */
    bool isValid() const;

    /**
     * @brief Close the underlying socket and terminate + free the ssl connection if present.
     */
    void close();

};

}

#endif // _TCPSOCKETWRAPPER_HPP