cmake_minimum_required(VERSION 3.10)

project(netlib)

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB HEADER_FILES "inc/*.hpp")

include_directories(inc)

add_library(netlib STATIC ${SRC_FILES} ${HEADER_FILES})
set_target_properties(netlib PROPERTIES PREFIX "")

option(NETLIB_SSL "Enable OpenSSL support" OFF)

if(NETLIB_SSL)
    add_definitions(-DNETLIB_SSL=1)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(netlib OpenSSL::SSL)
endif()

add_executable(example.run EXCLUDE_FROM_ALL example/example.cpp)
target_link_libraries(example.run netlib)
add_custom_target(example example.run)


add_executable(test.run EXCLUDE_FROM_ALL test/test.cpp)
target_link_libraries(test.run netlib)
add_custom_target(test test.run)

target_compile_options(netlib PRIVATE -Wall -Wextra -Wpedantic)