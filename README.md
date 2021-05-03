# Netlib
## General

This project provides a simple c++ wrapper around the annoying unix-c network 
functions. The focus lies especially on removing the need to work with the 
`sockaddr`, `sockaddr_in` and `sockaddr_in6` structs directly.

This is done by the classes `IpAddr` and `SockAddr` that hide all the low level 
structs and the evil type punning, as well as the network byte order and 
raw address / string conversions. 

Instead Ip-Addresses are simply specified as `string` and ports as `uint16_t` 
(in host byte order). All the conversions and Ipv4/Ipv6 differences are handled 
behind the scenes.

## Usage

This library can be used in many different ways. The most simple of which would 
be to just copy the content of the `src` directory in another project. The 
better way would be to include it in the buildsystem used by the other project.
The provided `Makefile` compiles this library to a static library that can be 
statically linked.

### Include this in a cmake project CMake

1. Create subdirectory for the external library: `mkdir libs`
2. Go into the subdirectory: `cd libs`
3. Add the library as a git submodule: `git submodule add https://code.fbi.h-da.de/istddmue2/netlib.git`
4. Configure `CMakeLists.txt` to include the library

```
# Add the library as subdirectory
add_subdirectory(libs/netlib)
# Add the include dir for header files
include_directories(libs/netlib/inc)

# Link the main executable with the library. (replace example_prog by the real name)
target_link_libraries(example_prog netlib)
```