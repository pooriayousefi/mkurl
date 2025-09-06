cmake_minimum_required(VERSION 3.20)
project(MkURL
    VERSION 1.0.0
    DESCRIPTION "Cross-platform URL shortcut creator"
    HOMEPAGE_URL "https://github.com/pooriayousefi/mkurl"
    LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable modern CMake policies
cmake_policy(SET CMP0077 NEW)

# Include standard modules
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set default build type
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Create the executable
add_executable(mkurl mkurl.cpp)

# Set target properties
set_target_properties(mkurl PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    DEBUG_POSTFIX "_debug"
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(mkurl PRIVATE _WIN32_WINNT=0x0601)
elseif(APPLE)
    target_compile_definitions(mkurl PRIVATE __APPLE__)
else()
    target_compile_definitions(mkurl PRIVATE __linux__)
endif()

# Compiler-specific options for cross-platform support
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(mkurl PRIVATE
        $<$<CONFIG:Debug>:-g -O0 -Wall -Wextra -Wpedantic -Wshadow>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(mkurl PRIVATE -fsanitize=address,undefined)
        target_link_options(mkurl PRIVATE -fsanitize=address,undefined)
    endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(mkurl PRIVATE
        $<$<CONFIG:Debug>:/Zi /Od /W4 /permissive->
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(mkurl PRIVATE /MT)
    endif()
endif()

# Installation
install(TARGETS mkurl
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT Runtime
)

install(FILES README.md LICENSE.txt
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT Documentation
)

# CPack configuration for cross-platform packaging
set(CPACK_PACKAGE_NAME "MkURL")
set(CPACK_PACKAGE_VENDOR "Pooria Yousefi")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Cross-platform URL shortcut creator")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")

if(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop;TGZ")
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Pooria Yousefi")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
endif()

include(CPack)

# Configuration summary
message(STATUS "MkURL Configuration Summary:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
