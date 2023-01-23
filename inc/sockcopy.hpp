/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#ifndef _SOCKCOPY_HPP
#define _SOCKCOPY_HPP

#include "tcpstream.hpp"
#include "tcplistener.hpp"
#include "udpsocket.hpp"

namespace netlib
{


/**
 * @brief A special wrapper type that can be used to create
 * automatically copyable socket objects. Without wrapping 
 * socket objects in this class, an explicit clone function
 * is needed to copy socket objects.
 */
template <class SockType>
class SockCopy
{
private:
    
    /**
     * @brief The wrapped socket instance.
     */
    SockType socket;

public:

    /**
     * @brief Wrap the given socket so that it is copyable.
     * 
     * @note Autoclose should most likely be set to false on 
     * the either one or both of the sockets to prevent accidentally 
     * closing the socket.
     * 
     * @param socket The socket that will be wrapped.
     */
    SockCopy(const SockType &socket)
        : socket{std::move(socket.clone())}
    { }

    /**
     * @brief Wrap the given socket so that it is copyable.
     * 
     * @param socket The socket that will be wrapped.
     */
    SockCopy(SockType &&socket)
        : socket{std::move(socket)}
    { }

    /**
     * @brief Copy the given wrapped socket.
     * 
     * @note Autoclose should most likely be set to false on 
     * the either one or both of the sockets to prevent accidentally 
     * closing the socket.
     * 
     * @param socket The warpped socket to copy.
     */
    SockCopy(const SockCopy & other)
        : SockCopy{std::move(other.socket.clone())}
    { }

    /**
     * @brief Copy the given wrapped socket.
     * 
     * @note Autoclose should most likely be set to false on 
     * the either one or both of the sockets to prevent accidentally 
     * closing the socket.
     * 
     * @param socket The warpped socket to copy.
     */
    SockCopy & operator=(const SockCopy & other)
    {
        socket = std::move(other.socket.clone());
    }

    /**
     * @brief Get the wrapped socket
     */
    SockType & getSocket()
    {
        return socket;
    }

    /**
     * @brief Get the wrapped socket
     */
    SockType & operator() ()
    {
        return socket;
    }

};


SockCopy<TcpStream> wrapCopy(TcpStream & other, bool disableAutoclose = true);

SockCopy<TcpListener> wrapCopy(TcpListener & other, bool disableAutoclose = true);

SockCopy<UdpSocket> wrapCopy(UdpSocket & other, bool disableAutoclose = true);


SockCopy<TcpStream> wrapCopy(TcpStream && other, bool disableAutoclose = true);

SockCopy<TcpListener> wrapCopy(TcpListener && other, bool disableAutoclose = true);

SockCopy<UdpSocket> wrapCopy(UdpSocket && other, bool disableAutoclose = true);


} // namespace netlib

#endif // _SOCKCOPY_HPP