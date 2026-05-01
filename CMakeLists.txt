set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

#subdirecotry лучше не использовать чё у мя два файла...

cmake_minimum_required(VERSION 3.30)
project(
	httpsrv
	DESCRIPTION "Simple http 1.0 server implementation in c++"
	LANGUAGES CXX
)

add_executable(httpsrv)
target_sources(httpsrv
	PRIVATE
	src/main.cpp
)

target_link_libraries(httpsrv
	net
	err
	#http
)


add_library(net STATIC)
add_library(err INTERFACE)

target_sources(net
	PRIVATE
	  src/net/net.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
	  src/net/net.hpp
)

target_sources(err
    INTERFACE
    FILE_SET HEADERS
    FILES
      src/error/error.hpp
)

target_include_directories(httpsrv PUBLIC src)
target_include_directories(net PUBLIC src)
target_include_directories(err INTERFACE src)

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -Wno-pointer-arith
)

