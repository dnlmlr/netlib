#ifndef _UDPSOCKET_HPP
#define _UDPSOCKET_HPP

#include "sockaddr.hpp"

namespace netlib
{


/**
 * @brief The UdpSocket can be used to receive UDP packets from and send UDP
 * packets to any target address.
 */
class UdpSocket
{
private:
    /**
     * @brief The local SockAddr that is used to bind to and listen.
     */
    SockAddr local;

    /**
     * @brief The filedescriptor of the current socket. If this is 0, the socket
     * is closed (unbound).
     */
    int sockfd;

    /**
     * @brief The length in bytes of the raw sockaddr for either sockaddr_in 
     * (Ipv4) or sockaddr_in6 (Ipv6) depending on what address type is in use.
     */
    socklen_t raw_socklen;

    /**
     * @brief The address family (address type) should either be AF_INET for 
     * Ipv4 or AF_INET6 for Ipv6.
     */
    int address_family;

    /**
     * @brief If set to true, the socket is automatically closed on destruction
     */
    bool autoclose = false;

public:

    /**
     * @brief Create a UdpSocket that will be bound to Ipv4 0.0.0.0 and the 
     * port 0. This will use a random free port and is useful for sending 
     * only.
     */
    UdpSocket();

    /**
     * @brief Create a UdpSocket that will be bound to the local address and 
     * port that is specified in the SockAddr. 
     * If the socket is only used to send UDP packets, the "any" address 
     * (0.0.0.0 for Ipv4, ::0 for Ipv6) should be used with the port 0. That 
     * will use a random free port.
     * If the socket is used to receive packets, the right port must be used.
     * 
     * @param local The SockAddr that will be listened to.
     */
    UdpSocket(SockAddr local);

    /**
     * @brief Same as UdpSocket(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    UdpSocket(IpAddr localAddress, uint16_t port);

    /**
     * @brief Same as UdpSocket(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    UdpSocket(const std::string &localAddress, uint16_t port);

    /**
     * @brief Same as UdpSocket(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    UdpSocket(const std::string &localAddressPort);

    /**
     * @brief The socket is automatically closed when the UdpSocket is 
     * destroyed.
     */
    ~UdpSocket();

    UdpSocket(UdpSocket &&other);

    UdpSocket& operator=(UdpSocket &&other);

    /**
     * @brief Copying UdpSocket is not allowed. See clone() for explicit copies.
     */
    UdpSocket(const UdpSocket &other) = delete;

    /**
     * @brief Copying UdpSocket is not allowed. See clone() for explicit copies.
     */
    UdpSocket& operator=(const UdpSocket &other) = delete;

    /**
     * @brief Create the socket and bind it to the socket address specified in 
     * the constructor.
     * This has to be called before the socket can be used to send or receive 
     * data.
     */
    void bind();

    /**
     * @brief Send a number of len bytes as UDP packet to the provided remote 
     * socket address.
     * 
     * @param remote The target address and port to which the UDP packet will 
     * be sent.
     * @param data Pointer to at least len bytes that will be sent as UDP packet.
     * @param len The number of bytes that will be sent.
     *  
     * @return The number of bytes that were actually sent.
     */
    ssize_t sendTo(const SockAddr &remote, const void *data, size_t len);

    /**
     * @brief Same as sendTo(SockAddr, data, len) but the SockAddr is created 
     * from the provided parameters.
     * 
     * @see SockAddr
     */
    ssize_t sendTo(const std::string &remoteAddr, uint16_t port, const void *data, size_t len);

    /**
     * @brief Same as sendTo(SockAddr, data, len) but the SockAddr is created 
     * from the provided parameters.
     * 
     * @see SockAddr
     */
    ssize_t sendTo(const std::string &remoteAddrPort, const void *data, size_t len);

    /**
     * @brief Receive a UDP packet and copy a maximum number of len bytes from 
     * the packet payload into data. Store the senders origin socket address 
     * into remote.
     * 
     * @param data Pointer to at least len bytes of data in which the payload 
     * will be copied.
     * @param len The maximum number of bytes that can be copied into data.
     * @param remote A reference to a SockAddr that is used to store the origin 
     * of the UDP packet.
     * 
     * @return The number of bytes that were actually copied.
     */
    ssize_t receive(void *data, size_t len, SockAddr &remote);

    /**
     * @brief Receive a UDP packet and copy a maximum number of len bytes from 
     * the packet payload into data. The packets origin is not stored.
     * 
     * @param data Pointer to at least len bytes of data in which the payload 
     * will be copied.
     * @param len The maximum number of bytes that can be copied into data.
     * 
     * @return The number of bytes that were actually copied.
     */
    ssize_t receive(void *data, size_t len);

    /**
     * @brief Receive a UDP packet and copy a maximum number of len bytes from 
     * the packet payload into data. Store the senders origin socket address 
     * into remote. If the timeout occurs, 0 is returned. 
     * 
     * @param data Pointer to at least len bytes of data in which the payload 
     * will be copied.
     * @param len The maximum number of bytes that can be copied into data.
     * @param remote A reference to a SockAddr that is used to store the origin 
     * of the UDP packet.
     * @param timeoutMs The number of milliseconds before a timeout occurs. 
     * 
     * @return The number of bytes that were actually copied, or 0 if a timeout
     * occured.
     */
    ssize_t receiveTimeout(void *data, size_t len, SockAddr &remote, int timeoutMs);

    /**
     * @brief Receive a UDP packet and copy a maximum number of len bytes from 
     * the packet payload into data. The packets origin is not stored. If the 
     * timeout occurs, 0 is returned. 
     * 
     * @param data Pointer to at least len bytes of data in which the payload 
     * will be copied.
     * @param len The maximum number of bytes that can be copied into data.
     * @param timeoutMs The number of milliseconds before a timeout occurs. 
     * 
     * @return The number of bytes that were actually copied, or 0 if a timeout
     * occured.
     */
    ssize_t receiveTimeout(void *data, size_t len, int timeoutMs);

    /**
     * @brief Check if the socket is closed or open. Open in this case means 
     * bound and ready to send / receive.
     * 
     * @return True if the socket was not yet opened, or if it has been closed.
     */
    bool isClosed() const;

    /**
     * @brief Close the listening socket. After this, the UdpSocket can no 
     * longer be used to send or receive packets without calling bind again.
     */
    void close();

    /**
     * @brief Set the behavior for when the UdpSocket is destroyed. If autoclose
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
     * @return A clone of this UdpSocket that shares the same underlying socket.
     */
    UdpSocket clone();

};


} // namespace netlib

#endif // _UDPSOCKET_HPP