#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// Redefine to public for having full access to the data when testing
#define private public
#define protected public

#include "netlib.hpp"

using namespace netlib;

TEST_CASE("Test IpAddr::V4") {

    auto strAddr = "192.168.13.37";

    auto ipv4 = IpAddr::V4(strAddr);

    CHECK( ipv4.type == IpAddr::Type::V4 );
    CHECK( ipv4.isIpv4() == true );
    CHECK( ipv4.isIpv6() == false );

    CHECK( ipv4.str_addr == strAddr );

    CHECK( ipv4.raw_addr.v4.s_addr == 0x250DA8C0);

}

TEST_CASE("Test IpAddr automatic version") {
    
    auto strAddrV4 = "192.168.13.37";

    auto ipv4 = IpAddr(strAddrV4);

    CHECK( ipv4.type == IpAddr::Type::V4 );
    CHECK( ipv4.isIpv4() == true );
    CHECK( ipv4.isIpv6() == false );

    CHECK( ipv4.str_addr == strAddrV4 );

    CHECK( ipv4.raw_addr.v4.s_addr == 0x250DA8C0);

    

    auto strAddrV6 = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    uint8_t addrV6bytes[16] = { 
        0x20, 0x01, 0x0d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
        0x00, 0x00, 0x8a, 0x2e, 0x03, 0x70, 0x73, 0x34
    };

    auto ipv6 = IpAddr(strAddrV6);

    CHECK( ipv6.type == IpAddr::Type::V6 );
    CHECK( ipv6.isIpv4() == false );
    CHECK( ipv6.isIpv6() == true );

    CHECK( ipv6.str_addr == strAddrV6 );

    bool raw_ip_equal = true;
    for (auto i = 0; i < 16; i++)
    {
        if (((uint8_t*)&ipv6.raw_addr.v6)[i] != addrV6bytes[i] )
            raw_ip_equal = false;
    }

    CHECK( raw_ip_equal == true );

}

TEST_CASE("Test IpAddr parsing error") {

    CHECK_THROWS( IpAddr("") );

    CHECK_THROWS( IpAddr("0.0.0.0.0") );
    CHECK_THROWS( IpAddr("256.0.0.0") );
    CHECK_THROWS( IpAddr("-5.0.0.0") );
    CHECK_THROWS( IpAddr("a.0.0.0") );

    CHECK_THROWS( IpAddr("g::0") );
    CHECK_THROWS( IpAddr("10000::0") );
    CHECK_THROWS( IpAddr("1:1:1:1:1:1:1:1:1") );

    CHECK_THROWS( IpAddr::V4("::1") );

    CHECK_THROWS( IpAddr::V4("") );

    CHECK_THROWS( IpAddr::V6("127.0.0.1") );

    CHECK_THROWS( IpAddr::V6("") );

}

TEST_CASE("Test SockAddr from ip:port string") {

    SockAddr sa4("192.168.13.37:1337");

    CHECK( sa4.address.type == IpAddr::Type::V4 );
    CHECK( sa4.getIpAddressString() == "192.168.13.37" );
    CHECK( sa4.port == 1337 );

    SockAddr sa6("[::1]:1337");

    CHECK( sa6.address.type == IpAddr::Type::V6 );
    CHECK( sa6.getIpAddressString() == "::1" );
    CHECK( sa6.port == 1337 );

    // Invalid port
    CHECK_THROWS( SockAddr("127.0.0.1:80808") );
    CHECK_THROWS( SockAddr("127.0.0.1:abc") );
    CHECK_THROWS( SockAddr("127.0.0.1:") );
    CHECK_THROWS( SockAddr("127.0.0.1") );

    CHECK_THROWS( SockAddr("[::1]:80808") );
    CHECK_THROWS( SockAddr("[::1]:abc") );
    CHECK_THROWS( SockAddr("[::1]:") );
    CHECK_THROWS( SockAddr("[::1]") );

    CHECK_THROWS( SockAddr("::1]:8080") );
    CHECK_THROWS( SockAddr("[::1:8080") );
    CHECK_THROWS( SockAddr("[::1.]:8080") );
    CHECK_THROWS( SockAddr("[127.0.0.1]:8080") );

    CHECK_THROWS( SockAddr("") );

}

TEST_CASE("Test SockAddr raw_sockaddr") {
    std::string ip_addr_str = "192.168.13.37";
    uint16_t port = 1337;

    IpAddr ip(ip_addr_str);

    SockAddr sa(ip, port);

    auto raw_addr4 = (sockaddr_in*)&sa.raw_sockaddr;


    CHECK( sa.getIpAddressString() == ip_addr_str );

    CHECK( sa.port == port );

    CHECK( sa.raw_sockaddr.generic.sa_family == AF_INET );

    CHECK( raw_addr4->sin_port == htons(port) );

    CHECK( raw_addr4->sin_addr.s_addr == ip.raw_addr.v4.s_addr );


    ip.type = IpAddr::Type::Undef;

    CHECK_THROWS( SockAddr(ip, port) );

}


TEST_CASE("Test SockAddr from raw_sockaddr") {
    std::string ip_str = "192.168.13.37";
    uint16_t port = 1337;


    sockaddr_in raw;
    memset(&raw, 0, sizeof(raw));

    raw.sin_family = AF_INET;
    raw.sin_port = htons(1337);
    raw.sin_addr.s_addr = 0x250DA8C0; // 192.168.13.37 as int

    SockAddr sa((sockaddr*)&raw, IpAddr::Type::V4);

    CHECK( sa.address.type == IpAddr::Type::V4 );

    CHECK( sa.getIpAddressString() == ip_str );

    CHECK( sa.getPort() == port );

    CHECK_THROWS( SockAddr((sockaddr*)&raw, IpAddr::Type::Undef) );


    std::string ip_str6 = "2001:1db8:85a3::8a2e:1370:7334";
    uint8_t ip6_bytes[16] = { // The ipv6 address from above as bytes
        0x20, 0x01, 0x1d, 0xb8, 0x85, 0xa3, 0x00, 0x00,
        0x00, 0x00, 0x8a, 0x2e, 0x13, 0x70, 0x73, 0x34
    };
    
    sockaddr_in6 raw6;
    memset(&raw6, 0, sizeof(raw6));

    raw6.sin6_family = AF_INET6;
    raw6.sin6_port = htons(1337);
    memcpy(&raw6.sin6_addr, ip6_bytes, 16);

    SockAddr sa6((sockaddr*) &raw6, IpAddr::Type::V6);

    CHECK( sa6.address.type == IpAddr::Type::V6 );

    CHECK( sa6.getIpAddressString() == ip_str6 );

    CHECK( sa6.getPort() == port );


}


// This test requires internet and a working dns config
TEST_CASE("Test Resolver") {

    IpAddr ip4 = Resolver::resolveHostnameIpv4("one.one.one.one");

    CHECK( ip4.type == IpAddr::Type::V4 );

    // The hostname can resolve to either 1.1.1.1 or the fallback 1.0.0.1
    CHECK((
        ip4.getAddressString() == "1.1.1.1" 
        || 
        ip4.getAddressString() == "1.0.0.1"
    ));


    IpAddr ip6 = Resolver::resolveHostnameIpv6("one.one.one.one");

    CHECK( ip6.type == IpAddr::Type::V6 );

    // The hostname can resolve to either 2606:4700:4700::1111 or the fallback 
    // 2606:4700:4700::1001
    CHECK((
        ip6.getAddressString() == "2606:4700:4700::1111" 
        || 
        ip6.getAddressString() == "2606:4700:4700::1001"
    ));


    std::vector<IpAddr> ips = Resolver::resolveHostnameAll("one.one.one.one");

    // Search for 1.1.1.1
    bool found1 = std::find_if(
        ips.begin(), ips.end(), 
        [](const IpAddr &first) {
            return first.str_addr == "1.1.1.1";
        }
    ) != ips.end();

    CHECK( found1 == true );

    // Search for 1.0.0.1
    bool found2 = std::find_if(
        ips.begin(), ips.end(), 
        [](const IpAddr &first) {
            return first.str_addr == "1.0.0.1";
        }
    ) != ips.end();

    CHECK( found2 == true );

    // Search for 2606:4700:4700::1111
    bool found3 = std::find_if(
        ips.begin(), ips.end(), 
        [](const IpAddr &first) {
            return first.str_addr == "2606:4700:4700::1111";
        }
    ) != ips.end();

    CHECK( found3 == true );

    // Search for 2606:4700:4700::1001
    bool found4 = std::find_if(
        ips.begin(), ips.end(), 
        [](const IpAddr &first) {
            return first.str_addr == "2606:4700:4700::1001";
        }
    ) != ips.end();

    CHECK( found4 == true );

}
