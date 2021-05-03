#ifndef _TCPSTREAM_HPP
#define _TCPSTREAM_HPP

#include "sockaddr.hpp"

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
     * @brief The filedescriptor of the current socket. If this is 0, the socket
     * is closed.
     */
    int sockfd;

public:

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

    TcpStream(TcpStream &&other) = default;
    TcpStream& operator=(TcpStream &&other) = default;

    /**
     * @brief Copying TcpListener is not allowed.
     */
    TcpStream(const TcpStream &other) = delete;

    /**
     * @brief Copying TcpListener is not allowed.
     */
    TcpStream& operator=(const TcpStream &other) = delete;

    /**
     * @brief Connect to the remote socket address specified in the constructor.
     */
    void connect();

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

    friend class TcpListener;

};


#endif // _TCPSTREAM_HPP