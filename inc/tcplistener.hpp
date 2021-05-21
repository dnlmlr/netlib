#ifndef _TCPLISTENER_HPP
#define _TCPLISTENER_HPP

#include "sockaddr.hpp"
#include "tcpstream.hpp"

#include <filesystem>

namespace netlib
{


/**
 * @brief Listen to a local ip address + port and accept incomming connections 
 * as TcpStreams.
 */
class TcpListener
{
private:

    /**
     * @brief The local SockAddr that is used to bind to and listen.
     */
    SockAddr local;

    /**
     * @brief The filedescriptor of the current socket. If this is 0, the socket
     * is closed.
     */
    int sockfd;

    /**
     * @brief If set to true, the socket is automatically closed on destruction
     */
    bool autoclose = false;

public:

    /**
     * @brief Create a TcpListener that will listen to the local address and 
     * port that is specified in the SockAddr.
     * 
     * @param local The SockAddr that will be listened to.
     */
    TcpListener(SockAddr local);
    
    /**
     * @brief Same as TcpListener(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    TcpListener(IpAddr localAddress, uint16_t port);
    
    /**
     * @brief Same as TcpListener(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    TcpListener(const std::string &localAddress, uint16_t port);

    /**
     * @brief Same as TcpListener(SockAddr) and the parameters are passed to 
     * SockAddr constructor.
     * 
     * @see SockAddr
     */
    TcpListener(const std::string &localAddressPort);

    /**
     * @brief The socket is automatically closed when the TcpListener is 
     * destroyed.
     */
    ~TcpListener();

    TcpListener(TcpListener &&other);
    TcpListener& operator=(TcpListener &&other);

    /**
     * @brief Copying TcpListener is not allowed.
     */
    TcpListener(const TcpListener &other) = delete;

    /**
     * @brief Copying TcpListener is not allowed.
     */
    TcpListener& operator=(const TcpListener &other) = delete;


    /**
     * @brief Start listening on the address and port specified at the creation.
     * This does not yet block and accept clients.
     * 
     * This will create the socket, bind it to the socket address and listen.
     * 
     * @param connectionQueue The number of connections that will be queued 
     * before refusing new connections.
     */
    void listen(int connectionQueue = 10);

    /**
     * @brief Block until a tcp connection is accepted. 
     * 
     * @return The TcpStream associated with the accepted connection.
     */
    TcpStream accept();

    /**
     * @brief Check if the listener socket is closed or open. Open in this case 
     * means bound and listening.
     * 
     * @return True if the socket was not yet opened, or if it has been closed.
     */
    bool isClosed() const;

    /**
     * @brief Close the listening socket. After this, the TcpListener can no 
     * longer be used without calling listen again.
     */
    void close();

};


} // namespace netlib

#endif // _TCPLISTENER_HPP