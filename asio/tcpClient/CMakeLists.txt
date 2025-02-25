cmake_minimum_required(VERSION 3.10)
project(TcpClient)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)  # Use C++17

# Find Boost library
find_package(Boost REQUIRED COMPONENTS system )
find_package(Threads REQUIRED)

# Add executable for the client
add_executable(timer timer.cpp)
target_link_libraries(timer PRIVATE Boost::system ${CMAKE_THREAD_LIBS_INIT})

