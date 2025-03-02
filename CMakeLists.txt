cmake_minimum_required(VERSION 3.30)
project(imclx)

set(CMAKE_CXX_STANDARD 23)

# Set the IMCL variable to the imcl_lib folder path
set(IMCL ${CMAKE_SOURCE_DIR}/imcl_lib)

include_directories(${IMCL}/include)

# Get the output directory for runtime executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set the output directory for the executable based on the build mode
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)

add_executable(imclx main.cpp)

# Include the imcl_lib CMakeLists.txt
add_subdirectory(${IMCL})

# Link the imcl_lib library to the imclx executable
target_link_libraries(imclx imcl)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)