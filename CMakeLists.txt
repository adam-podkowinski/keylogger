cmake_minimum_required(VERSION 3.10)

project(WindowsProfiler VERSION 1.0)

set(SOURCE_FILES WindowsProfiler.cpp)

# add_executable(WindowsProfiler ${SOURCE_FILES})
add_executable(WindowsProfiler WIN32 ${SOURCE_FILES})

set_property(TARGET WindowsProfiler PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_include_directories(WindowsProfiler PUBLIC C:/chilkat/include)
target_link_libraries(WindowsProfiler -static-libgcc -static-libstdc++ C:/chilkat/libchilkat-9.5.0.a wsock32 ws2_32 -static winpthread)
target_compile_options(WindowsProfiler PUBLIC -DUNICODE)


message(${CMAKE_BUILD_TYPE})
message(${CMAKE_CXX_COMPILER_ID})