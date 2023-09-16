
cmake_minimum_required(VERSION 3.5)

project(Account VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(MSVC)
    string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127") # C4127 n/a for C++11
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic")
endif()

add_subdirectory(accounttooldata)
add_subdirectory(accounttool)
add_subdirectory(help)


include(setup/install.cmake)

if(MSVC)
    include(setup/install-qt.cmake)
endif()

include(setup/install-help.cmake)
include(CPack)

