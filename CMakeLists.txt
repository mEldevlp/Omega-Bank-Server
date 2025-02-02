cmake_minimum_required(VERSION 3.21)
project(Omega-Bank-Server VERSION 1.0.0)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(UI_DIR "${CMAKE_SOURCE_DIR}/ui")

# Find modules Qt6
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)

include_directories(${SOURCE_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${UI_DIR})

file(GLOB_RECURSE SOURCES
    "${SOURCE_DIR}/*.cpp"
    "${INCLUDE_DIR}/*.h"
)

add_executable(Omega-Bank-Server ${SOURCES})
set_target_properties(Omega-Bank-Server PROPERTIES WIN32_EXECUTABLE OFF)

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>
)

target_include_directories(Omega-Bank-Server PUBLIC ${SOURCE_DIR})
target_include_directories(Omega-Bank-Server PUBLIC ${INCLUDE_DIR})
target_include_directories(Omega-Bank-Server PUBLIC ${UI_DIR})

target_link_libraries(Omega-Bank-Server PRIVATE 
	Qt6::Core 
	Qt6::Widgets 
	Qt6::Network
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Omega-Bank-Server)