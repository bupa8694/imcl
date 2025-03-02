cmake_minimum_required(VERSION 3.30)
project(imcl_lib)

set(CMAKE_CXX_STANDARD 23)

# Define options for building dynamic or static libraries
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Include directories
include_directories(include)

# Add source files
set(SOURCES
    src/imcl.cpp
    # Add other source files here
)

# Add library target
if(BUILD_SHARED_LIBS)
    add_library(imcl SHARED ${SOURCES})
    message("Building shared library")
else()
    add_library(imcl STATIC ${SOURCES})
    message("Building static library")
endif()

# Set platform-specific settings
if(WIN32)
    # Windows-specific settings
    target_compile_definitions(imcl PRIVATE PLATFORM_WINDOWS BUILDING_DLL)
    #add_definitions(-DBUILDING_DLL)
    message("Building for Windows")
elseif(APPLE)
    # macOS-specific settings
    target_compile_definitions(imcl PRIVATE PLATFORM_MACOS)
    message("Building for macOS")
elseif(UNIX)
    # Linux-specific settings
    target_compile_definitions(imcl PRIVATE PLATFORM_LINUX)
    message("Building for Linux")
endif()

target_include_directories(imcl PUBLIC include)
target_sources(imcl PRIVATE ${SOURCES})