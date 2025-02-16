cmake_minimum_required(VERSION 3.10)
project(BoostAsioExample)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)  # Use C++17

# Find Boost library
find_package(Boost REQUIRED COMPONENTS system)

# Add executable for the client
add_executable(client client.cpp)
target_link_libraries(client PRIVATE Boost::system)

# Add executable for the server
add_executable(server server.cpp)
target_link_libraries(server PRIVATE Boost::system)

add_executable(state_client state_client.cpp)
target_link_libraries(state_client PRIVATE Boost::system)
