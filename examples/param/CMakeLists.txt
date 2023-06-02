cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(param)

add_executable(param
    param.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(param
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(param PRIVATE -Wall -Wextra)
else()
    add_compile_options(param PRIVATE -WX -W2)
endif()
