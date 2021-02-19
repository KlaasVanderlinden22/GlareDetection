cmake_minimum_required(VERSION 2.8.3)

project(GlareDetection)

# Include directories
include_directories(include)

message("Include directories")

# Add sources
file(GLOB SOURCES "src/*.c")

# Make executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

message("Making executable")

# Linking libraries
target_link_libraries(${CMAKE_PROJECT_NAME} m)

message("Linking libraries")
