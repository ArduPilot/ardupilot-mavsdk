cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(actions)

add_executable(actions
    actions.cpp
)

find_package(MAVSDK REQUIRED)

target_link_libraries(actions
    MAVSDK::mavsdk
)

if(NOT MSVC)
    add_compile_options(actions PRIVATE -Wall -Wextra)
else()
    add_compile_options(actions PRIVATE -WX -W2)
endif()
