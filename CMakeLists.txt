# Copyright (c) 2025 Daniel Kondrashoff

cmake_minimum_required(VERSION 3.21)
project(Omega-Bank-Server VERSION 1.0.0)

cmake_policy(SET CMP0144 NEW)
cmake_policy(SET CMP0167 NEW)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

list(APPEND CMAKE_PREFIX_PATH "C:/boost-1.87.0/stage/lib/cmake")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")

# Find modules Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# Ищем нужные компоненты Boost
find_package(Boost REQUIRED COMPONENTS system thread)

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
else()
    message(FATAL_ERROR "Boost not found!")
endif()

set(PostgreSQL_ROOT "C:/Program Files/PostgreSQL/17")
find_package(PostgreSQL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.15.1  # ltst
)

FetchContent_MakeAvailable(spdlog)

include_directories(${SOURCE_DIR})
include_directories(${INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${INCLUDE_DIR}/*.h"
)

add_executable(Omega-Bank-Server ${SOURCES})
set_target_properties(Omega-Bank-Server PROPERTIES WIN32_EXECUTABLE OFF)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

target_include_directories(Omega-Bank-Server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(Omega-Bank-Server PUBLIC ${SOURCE_DIR} ${INCLUDE_DIR})

target_link_libraries(Omega-Bank-Server PRIVATE 
	Qt6::Core 
	Qt6::Widgets 
	Qt6::Network
	spdlog::spdlog
	Boost::system
	Boost::thread
)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Omega-Bank-Server)