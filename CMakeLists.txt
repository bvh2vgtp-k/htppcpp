set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_COLOR_DIAGNOSTICS ON)

cmake_minimum_required(VERSION 3.30)
project(
	httpsrv
	DESCRIPTION "Simple http 1.0 server implementation in c++"
	LANGUAGES CXX
)


add_compile_options(
  -Wall
  -Wextra
  -Werror
  -Wpedantic
  -Wno-pointer-arith
)

add_executable(httpsrv)
target_sources(httpsrv
	PRIVATE
	src/main.cpp
)

target_link_libraries(httpsrv
	net
	err
	http
)


add_library(net STATIC)
add_library(http STATIC)
add_library(err INTERFACE)

target_sources(net
	PRIVATE
	  src/net/net.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
	  src/net/net.hpp
)

target_sources(http
	PRIVATE
	  src/http/http.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
	  src/http/http.hpp
)


target_sources(err
    INTERFACE
    FILE_SET HEADERS
    FILES
      src/error/error.hpp
)

target_include_directories(httpsrv PUBLIC src)
target_include_directories(net PUBLIC src)
target_include_directories(http PUBLIC src)
target_include_directories(err INTERFACE src)


