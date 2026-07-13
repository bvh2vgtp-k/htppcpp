cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(FATAL_ERROR "OS not supported")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to source directory"
    )
endif()


project(
	httpsrv
    DESCRIPTION "Simple HTTP/1.1 server implementation in C++"
	LANGUAGES CXX
)

add_library(compiler_flags INTERFACE)

target_compile_options(compiler_flags INTERFACE
    -Wall
    -Wextra
    -Wshadow
    -Werror
    -Wpedantic
    -Wno-pointer-arith
)

target_compile_options(compiler_flags INTERFACE
    $<$<CONFIG:Debug>:
        -g
        -fsanitize=undefined,address
    >
)

target_link_options(compiler_flags INTERFACE
    $<$<CONFIG:Debug>:
        -fsanitize=undefined,address
    >
)

target_compile_options(compiler_flags INTERFACE
    $<$<CONFIG:Release>:
        -O3
    >
)

add_executable(httpsrv)
target_sources(httpsrv
	PRIVATE
	src/main.cpp
)

target_link_libraries(httpsrv
    PRIVATE
        compiler_flags
        http
        listener
        acceptor
        err
)


add_library(listener STATIC)
add_library(acceptor STATIC)
add_library(response STATIC)
add_library(http STATIC)
add_library(err INTERFACE)

target_sources(listener
	PRIVATE
	  src/net/listener.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
	  src/net/listener.hpp
)
target_link_libraries(listener PRIVATE compiler_flags)


target_sources(acceptor
	PRIVATE
	  src/net/acceptor.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
	  src/net/acceptor.hpp
)
target_link_libraries(acceptor PRIVATE compiler_flags)

target_sources(response
    PRIVATE
      src/http/response/Response.cpp
    PUBLIC
    FILE_SET HEADERS
    FILES
      src/http/response/Response.hpp
)
target_link_libraries(response PRIVATE compiler_flags)

target_sources(http
	PRIVATE
	  src/http/http.cpp
	PUBLIC
	FILE_SET HEADERS
	FILES
	  src/http/http.hpp
)
target_link_libraries(http PUBLIC response PRIVATE compiler_flags)


target_sources(err
    INTERFACE
    FILE_SET HEADERS
    FILES
      src/error/error.hpp
)
target_link_libraries(err INTERFACE compiler_flags)

target_include_directories(httpsrv PUBLIC src)
target_include_directories(listener PUBLIC src)
target_include_directories(acceptor PUBLIC src)
target_include_directories(response PUBLIC src)
target_include_directories(http PUBLIC src)
target_include_directories(err INTERFACE src)
