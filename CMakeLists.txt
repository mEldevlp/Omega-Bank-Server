# Copyright (c) 2025 Daniel Kondrashoff

cmake_minimum_required(VERSION 3.31.5)
project(Omega-Bank-Server VERSION 1.0.0)

cmake_policy(SET CMP0144 NEW)
cmake_policy(SET CMP0167 NEW)

set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "" FORCE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Отключает нестандартные расширения компилятора

list(APPEND CMAKE_PREFIX_PATH "C:/Program Files/boost/boost_1_87_0/stage/lib/cmake/Boost-1.87.0")
list(APPEND CMAKE_PREFIX_PATH "C:/Program Files/boost/boost_1_87_0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

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

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
	"${SOURCE_DIR}/*.h"
)

add_executable(Omega-Bank-Server ${SOURCES})

target_precompile_headers(Omega-Bank-Server PRIVATE "${SOURCE_DIR}/pch.h")

set_target_properties(Omega-Bank-Server PROPERTIES WIN32_EXECUTABLE OFF)

target_include_directories(Omega-Bank-Server PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(Omega-Bank-Server PUBLIC ${SOURCE_DIR} ${INCLUDE_DIR})

target_link_libraries(Omega-Bank-Server PRIVATE 
	spdlog::spdlog
	Boost::system
	Boost::thread
)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${SOURCES})

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Omega-Bank-Server)