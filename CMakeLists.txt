cmake_minimum_required(VERSION 3.1)

project(net_playground)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(Util)

find_or_get(Boost)
find_package(Threads)
set(CXX_STANDARD_REQUIRED ON)

add_executable(server server.cpp)
target_include_directories(server PUBLIC 
    ${PROJECT_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS})
target_link_libraries(server Threads::Threads)
target_precompile_headers(server PUBLIC ${PROJECT_SOURCE_DIR}/pch.h)
set_property(TARGET server PROPERTY CXX_STANDARD 17)