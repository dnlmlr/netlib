#include <iostream>
#include <string>
#include <cstring>

// Include the all-in-one headerfile. This will include all other headers
#include "netlib.hpp"

using namespace netlib;

void example_IpAddr()
{
    // IpAddr takes an address as string and automatically detects if it is  
    // Ipv4 or Ipv6

    // Autodetected Ipv4 example
    IpAddr ip1("192.168.13.37");
    std::cout << "IP1\n---\n" 
        << "Address=" << ip1.getAddressString() << "\n"
        << "IsIpv4=" << ip1.isIpv4() << "\n" 
        << "IsIpv6=" << ip1.isIpv6() << "\n\n";

    // Autodetected Ipv6 example
    IpAddr ip2("dead:beef::1");
    std::cout << "IP2\n---\n" 
        << "Address=" << ip2.getAddressString() << "\n"
        << "IsIpv4=" << ip2.isIpv4() << "\n" 
        << "IsIpv6=" << ip2.isIpv6() << "\n\n";

    // Ipv4 or Ipv6 addresses can also be created explicitely by using 
    // IpAddr::V4(addr) and IpAddr::V6(addr). If the provided address string is 
    // not a valid address of the type, an exception will be thrown

    // Explicit Ipv6 example
    ip1 = IpAddr::V6("dead:beef::1");
    std::cout << "IP1 after change\n----------------\n" 
        << "Address=" << ip1.getAddressString() << "\n"
        << "IsIpv4=" << ip1.isIpv4() << "\n" 
        << "IsIpv6=" << ip1.isIpv6() << "\n\n";

    // Explicit Ipv4 example
    ip2 = IpAddr::V4("192.168.13.37");
    std::cout << "IP2 after change\n----------------\n" 
        << "Address=" << ip2.getAddressString() << "\n"
        << "IsIpv4=" << ip2.isIpv4() << "\n" 
        << "IsIpv6=" << ip2.isIpv6() << "\n\n";


    // Specifying an Ipv6 address with the explicit Ipv4 will throw an exception
    try
    {
        auto ip = IpAddr::V4("::1");
        std::cout << "If this code is executed, something went very wrong\n\n";
    }
    catch (const std::exception & e)
    {
        std::cout << "This exception is intended: \n"
            << e.what() << "\n\n";
    }

    // Specifying an Ipv4 address with the explicit Ipv6 will throw an exception
    try
    {
        auto ip = IpAddr::V6("192.168.13.37");
        std::cout << "If this code is executed, something went very wrong\n\n";
    }
    catch (const std::exception & e)
    {
        std::cout << "This exception is intended: \n"
            << e.what() << "\n\n";
    }
}


void example_SockAddr()
{
    // SockAddr is the combination of an IpAddress and a port. The IpAddr will 
    // handle the Ipv4/Ipv6 type and the port is an unsigned 16 bit integer.
    // There are multiple ways to create a SockAddr.


    // Create a SockAddr by passing an IpAddr and a port
    IpAddr ip1("192.168.13.37");
    SockAddr sa1(ip1, 1337);
    std::cout << "SockAddr1\n--------\n"
        << "IpAddr=" << sa1.getIpAddressString() << "\n"
        << "Port=" << sa1.getPort() << "\n\n";


    // Create a SockAddr by passing an IpAddress as string and a port
    // The the ip string the same rules apply as for the IpAddr string 
    // constructor. That means the type (Ipv4/Ipv6) is determined automatically.
    SockAddr sa2("10.10.0.69", 48879);
    std::cout << "SockAddr2\n--------\n"
        << "IpAddr=" << sa2.getIpAddressString() << "\n"
        << "Port=" << sa2.getPort() << "\n\n";


    // Create a SockAddr by passing a "ipaddr:port" string with an Ipv4 address.
    // If the port is not a unsigned 16 bit integer, or the IpAddress is invalid,
    // an exception will be thrown.
    SockAddr sa3("10.11.12.13:443");
    std::cout << "SockAddr3\n--------\n"
        << "IpAddr=" << sa3.getIpAddressString() << "\n"
        << "Port=" << sa3.getPort() << "\n\n";
    
    
    // Create a SockAddr by passing a "ipaddr:port" string with an Ipv6 address.
    // For Ipv6 addresses, the address must be placed inside []. Otherwise it 
    // behaves the same as with Ipv4 addresses. The [] can not be used with 
    // Ipv4 addresses.
    SockAddr sa4("[dead:beef::1337]:8080");
    std::cout << "SockAddr4\n--------\n"
        << "IpAddr=" << sa4.getIpAddressString() << "\n"
        << "Port=" << sa4.getPort() << "\n\n";

}

void example_resolver()
{
    // The Resolver helps with resolving hostnames to IpAddresses. It can 
    // resolve either just the first address (which is wanted most of the time),
    // or it can fetch all associated IpAddresses for a given hostname. Both 
    // Ipv4 and Ipv6 addresses are supported.

    std::string hostname = "one.one.one.one";

    // Fetch Ipv4 address for the given hostname
    IpAddr ip4 = Resolver::resolveHostnameIpv4(hostname);
    std::cout << "Resolve Ipv4 for " << hostname << "\n" 
        << " => " << ip4.getAddressString() << "\n\n";

    
    // Fetch Ipv6 address for the given hostname
    IpAddr ip6 = Resolver::resolveHostnameIpv6(hostname);
    std::cout << "Resolve Ipv6 for " << hostname << "\n" 
        << " => " << ip6.getAddressString() << "\n\n";


    // Fetch all associated Ipv4 addresses for the given hostname
    auto ip4s = Resolver::resolveHostnameAllIpv4(hostname);
    std::cout << "Resolve all Ipv4 addresses for " << hostname << "\n";
    for (auto ip : ip4s) std::cout << " => " << ip.getAddressString() << "\n";
    std::cout << "\n";


    // Fetch all associated Ipv6 addresses for the given hostname
    auto ip6s = Resolver::resolveHostnameAllIpv6(hostname);
    std::cout << "Resolve all Ipv6 addresses for " << hostname << "\n";
    for (auto ip : ip6s) std::cout << " => " << ip.getAddressString() << "\n";
    std::cout << "\n";


    // Fetch all associated Ipv4 and Ipv6 addresses for the given hostname
    auto ip46s = Resolver::resolveHostnameAll(hostname);
    std::cout << "Resolve all Ipv4 & Ipv6 addresses for " << hostname << "\n";
    for (auto ip : ip46s) 
    {
        std::cout << " => " << "isIpV4=" << ip.isIpv4() 
            << "  " << ip.getAddressString() << "\n";
    }
    std::cout << "\n";

    
    // When a given hostname could not be resolved, an exception is thrown
    try
    {
        Resolver::resolveHostnameAll("thisisaninvalidhostname-6iesw5rb7fwa54r.com");
        std::cout << "If this code is executed, something went very wrong\n\n";
    }
    catch (const std::exception & e)
    {
        std::cout << "This exception is intended: \n"
            << e.what() << "\n\n";
    }
    
}

int main(int argc, char **argv)
{
    
    example_IpAddr();

    example_SockAddr();

    example_resolver();

    return 0;
}
