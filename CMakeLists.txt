# Minimum required version of CMake
cmake_minimum_required(VERSION 3.17)

# Prevent in-source builds
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(
    FATAL_ERROR
      "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source."
  )
endif()

# Define the project and its properties
project(
  wspserver
  VERSION 0.1.0
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

# Append the project's custom CMake module path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Find the spdlog library
find_package(spdlog REQUIRED)
# Define a preprocessor macro to use the external fmt library
add_compile_definitions(SPDLOG_FMT_EXTERNAL)
# Find the Drogon library
find_package(Drogon CONFIG REQUIRED)

find_package(GTest REQUIRED)
include(GoogleTest)

find_package(msgpack-cxx CONFIG REQUIRED)
add_compile_definitions(MSGPACK_USE_BOOST=0 MSGPACK_CXX20=1)

# Include the project's main directory
include_directories(${PROJECT_SOURCE_DIR})

# Function to add CMake subdirectories if they exist
function(add_cmake_subdirectories)
  # Gather all directories
  file(
    GLOB cmake_directories
    RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *)
  # For each directory
  foreach(dir ${cmake_directories})
    # If the directory contains a CMakeLists.txt, add it to the build
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir}
       AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/CMakeLists.txt)
      add_subdirectory(${dir})
    endif()
  endforeach()
endfunction()

# Function to set the output directory of a target
function(set_output_dir target_name output_dir)
  set_target_properties(
    ${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                              ${CMAKE_BINARY_DIR}/${output_dir})
endfunction()

# Call the function to add CMake subdirectories
add_cmake_subdirectories()
