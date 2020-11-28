cmake_minimum_required(VERSION 3.10)

project(WindowsProfiler VERSION 1.0)

set(SOURCE_FILES WindowsProfiler.cpp md5.cpp base64.cpp CSmtp.cpp)

# set(OPENSSL_USE_STATIC_LIBS TRUE)
# set(OPENSSL_ROOT_DIR "C:/Program\ Files\ (x86)/OpenSSL-Win32")
message(OPENSSL_ROOT_DIR)

find_package(OpenSSL)

add_executable(WindowsProfiler ${SOURCE_FILES})
target_link_libraries(WindowsProfiler OpenSSL::SSL wsock32 ws2_32)

