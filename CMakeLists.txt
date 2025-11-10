cmake_minimum_required(VERSION 3.10)
project(CyberpunkCannonShooter VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find SFML (version 3.0 or later)
# Note: Graphics automatically includes System and Window
# Audio automatically includes System
find_package(SFML 3.0 COMPONENTS Graphics Audio REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories (using modern target-based approach)
# Must be after add_executable
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link SFML libraries
# Graphics automatically includes System and Window
# Audio automatically includes System
target_link_libraries(${PROJECT_NAME}
    SFML::Graphics
    SFML::Audio
)

# Copy assets to build directory (for development)
# Note: This will be enabled when we have actual assets to copy
# For now, we'll skip this since directories are empty
# if(EXISTS ${CMAKE_SOURCE_DIR}/assets)
#     file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
# endif()
# if(EXISTS ${CMAKE_SOURCE_DIR}/config)
#     file(COPY ${CMAKE_SOURCE_DIR}/config DESTINATION ${CMAKE_BINARY_DIR})
# endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX-)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Enable testing
enable_testing()

# Test subdirectory (will be added later)
# add_subdirectory(tests)

message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(SFML_VERSION)
    message(STATUS "SFML Version: ${SFML_VERSION}")
endif()
if(SFML_INCLUDE_DIR)
    message(STATUS "SFML Include: ${SFML_INCLUDE_DIR}")
endif()

