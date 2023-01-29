/* Copyright 2023 Daniel M
 *
 * Licensed under the MIT license.
 * This file is part of dnlmlr/netlib project.
 */

#ifndef _TCPSTREAM_HPP
#define _TCPSTREAM_HPP

#include <string>
#include <memory>

#include "sockaddr.hpp"
#include "tcpsocketwrapper.hpp"

namespace netlib
{


/**
 * @brief The TcpStream represents tcp a connection with another endpoint and is
 * used to send and receive data through that connection.
 */
class TcpStream
{
private:

    /**
     * @brief The remote SockAddr that the TcpStream will be connected to, or 
     * currently is connected to (depending on wether the connection is open
     * or closed).
     */
    SockAddr remote;

    /**
     * @brief The filedescriptor of the current socket. If this socket wrapper is marked
     * as invalid, the connection is considered closed.
     */
    std::shared_ptr<TcpSocketWrapper> socket;

    /**
     * @brief If set to true, the socket is automatically closed on destruction
     */
    bool autoclose = true;

public:

    /**
     * @brief Create a TcpStream with the remote address set to 0.0.0.0:0 . Since the port 0 is not
     * valid, this TcpStream will fail to connect before changing the remote to a valid SockAddr.
     */
    TcpStream();

    /**
     * @brief Create a TcpStream that will connect to the specified remote 
     * socket address.
     * 
     * @param remote The SockAddress that will be connected to in the connect
     * function.
     */
    TcpStream(SockAddr remote);
    
    /**
     * @brief Same as TcpStream(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    TcpStream(IpAddr remoteAddress, uint16_t port);
    
    /**
     * @brief Same as TcpStream(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    TcpStream(const std::string &remoteAddress, uint16_t port);

    /**
     * @brief Same as TcpStream(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    TcpStream(const std::string &remoteAddressPort);

    /**
     * @brief The socket is automatically closed when the TcpStream is 
     * destroyed.
     */
    ~TcpStream();

    TcpStream(TcpStream &&other);
    TcpStream& operator=(TcpStream &&other);

    /**
     * @brief Copying TcpListener is not allowed. See clone() for explicit copies.
     */
    TcpStream(const TcpStream &other) = delete;

    /**
     * @brief Copying TcpListener is not allowed. See clone() for explicit copies.
     */
    TcpStream& operator=(const TcpStream &other) = delete;

    /**
     * @brief Set the remote address to the specified value. This will fail if the TcpStream is 
     * currently connected.
     */
    void setRemote(SockAddr remote);

    /**
     * @brief Connect to the remote socket address specified in the constructor.
     */
    void connect();

#ifdef NETLIB_SSL
    /**
     * @brief Connect to the remote socket address specified in the constructor using TLS.
     */
    void connect(SSL_CTX *ctx);
#endif // NETLIB_SSL

    /**
     * @brief Close the listening socket. After this, the TcpStream can no 
     * longer be used without reconnecting using connect.
     */
    void close();

    /**
     * @brief Send a maximum of len bytes of data into the tcp connection. It 
     * is possible that less than len bytes are sent, in that case the rest has 
     * to be sent with another call to send.
     * 
     * If sending fails, an exception is thrown.
     * 
     * @param data Pointer to at least len bytes that will be sent over the 
     * tcp connection.
     * @param len The maxiumum number of bytes that will be sent over the tcp 
     * connection.
     *  
     * @return The number of bytes that were actually sent over the connection.
     * This can be less than len.
     */
    ssize_t send(const void *data, size_t len);

    /**
     * @brief Send exactly len bytes of data into the tcp connection.
     * 
     * If sending fails, an exception is thrown.
     * 
     * @param data Pointer to at least len bytes that will be sent over the 
     * tcp connection.
     * @param len The number of bytes that will be sent over the tcp connection.
     *  
     * @return The number of bytes that were actually sent over the connection.
     */
    void sendAll(const void *data, size_t len);

    /**
     * @brief Send the given string data over the tcp connection. The function 
     * send all chars of the given string.
     * 
     * If the sending fails, an exception is thrown.
     * 
     * @param str The string that will be sent.
     */
    void sendAllString(const std::string &str);

    /**
     * @brief Receive a maximum of len bytes of data from the tcp connection. It 
     * is possible that less than len bytes are received. This call will block 
     * until at least byte can be received or the connection terminates.
     * 
     * If receiving fails, an exception is thrown.
     * 
     * @param data Pointer to at least len bytes where the data received over  
     * the tcp connection will be stored.
     * @param len The maxiumum number of bytes that will be received over the 
     * tcp connection.
     *  
     * @return The number of bytes that were actually received over the 
     * connection. This can be less than len.
     */
    ssize_t read(void *data, size_t len);

    /**
     * @brief Read exactly len bytes from the tcp stream. If the connection is
     * closed before len bytes are read, the number of bytes actually read is 
     * returned. This will block until exactly len bytes are read or the 
     * connection is closed.
     * 
     * If receiving failed or the connection is closed before at least one 
     * byte was received, an exception is thrown.
     * 
     * @param data Pointer to at least len bytes where the data received over  
     * the tcp connection will be stored.
     * @param len The number of bytes that will be received over the tcp 
     * connection.
     *  
     * @return The number of bytes that were actually received over the 
     * connection. This can be less than len if the connection is closed.
     */
    ssize_t readAll(void *data, size_t len);

    /**
     * @brief Same as TcpStream::read but with a millisecond timeout. If the 
     * timeout is reached without receiving data, 0 is returned.
     * 
     * If receiving fails, an exception is thrown.
     * 
     * @param data Pointer to at least len bytes where the data received over  
     * the tcp connection will be stored.
     * @param len The maxiumum number of bytes that will be received over the 
     * tcp connection.
     * @param timeoutMs The number of milliseconds before a timeout occurs. 
     * 
     * @return The number of bytes that were actually received over the 
     * connection. This can be less than len. If a timeout occurs 0 is 
     * returned.
     */
    ssize_t readTimeout(void *data, size_t len, int timeoutMs);

    /**
     * @brief Same as TcpStream::readAll but with a millisecond timeout. If 
     * the timeout is reached without receiving any data, 0 is returned. If 
     * the timout occurs after receiving some data, the number of bytes 
     * received before the timeout is returned as NEGATIVE. The timeout 
     * starts from the begining if partial data is received.
     * 
     * If receiving failed an exception is thrown.
     * 
     * @param data Pointer to at least len bytes where the data received over  
     * the tcp connection will be stored.
     * @param len The number of bytes that will be received over the tcp 
     * connection.
     * @param timeoutMs The number of milliseconds before a timeout occurs.
     *  
     * @return The number of bytes that were actually received over the 
     * connection. This can be less than len if the connection is closed.
     * If a timout occurs after some data was already read, the number of 
     * bytes is returned as negative.
     */
    ssize_t readAllTimeout(void *data, size_t len, int timeoutMs);

    /**
     * @brief Get the socket address of the connection target.
     * 
     * @return The connection targets socket address.
     */
    const SockAddr & getRemoteAddr() const;

    /**
     * @brief Check if the socket is closed or open. Open in this case means 
     * connected to the remote.
     * 
     * @return True if the socket was not yet opened, or if it has been closed.
     */
    bool isClosed() const;

    /**
     * @brief Set the behavior for when the TcpStream is destroyed. If autoclose
     * is enabled, the socket is closed on destruct. If autoclose is disabled, the
     * socket will not be closed automatically.
     * 
     * @param autoclose Enable or disable the autoclose functionality.
     */
    void setAutoclose(bool autoclose);

    /**
     * @brief Create a clone of this socket wrapper object. The clone and the 
     * original will share the same underlying socket and file descriptor. 
     * If one of the instances closes the socket, the socket will be closed 
     * for both. The other instance will not be notified about this, but instead
     * socket operations will just fail. Due to this, it might be a good idea to  
     * disabel autoclose and manually close the socket.
     * 
     * @return A clone of this TcpStream that shares the same underlying socket.
     */
    TcpStream clone() const;

    friend class TcpListener;

};


} // namespace netlib

#endif // _TCPSTREAM_HPP