/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#include "sockcopy.hpp"

namespace netlib
{


SockCopy<TcpStream> wrapCopy(TcpStream & other, bool disableAutoclose)
{
    if (disableAutoclose) other.setAutoclose(false);
    return SockCopy<TcpStream>(other);
}

SockCopy<TcpListener> wrapCopy(TcpListener & other, bool disableAutoclose)
{
    if (disableAutoclose) other.setAutoclose(false);
    return SockCopy<TcpListener>(other);
}

SockCopy<UdpSocket> wrapCopy(UdpSocket & other, bool disableAutoclose)
{
    if (disableAutoclose) other.setAutoclose(false);
    return SockCopy<UdpSocket>(other);
}


SockCopy<TcpStream> wrapCopy(TcpStream && other, bool disableAutoclose)
{
    if (disableAutoclose) other.setAutoclose(false);
    return SockCopy<TcpStream>(std::move(other));
}

SockCopy<TcpListener> wrapCopy(TcpListener && other, bool disableAutoclose)
{
    if (disableAutoclose) other.setAutoclose(false);
    return SockCopy<TcpListener>(std::move(other));
}

SockCopy<UdpSocket> wrapCopy(UdpSocket && other, bool disableAutoclose)
{
    if (disableAutoclose) other.setAutoclose(false);
    return SockCopy<UdpSocket>(std::move(other));
}


} // namespace netlib
