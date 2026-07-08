# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.27)

# Define the project name
project(Stigmer_simulation)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set C++ compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

# Find OpenMP
find_package(OpenMP REQUIRED)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/libs
)

# Include subdirectories
add_subdirectory(src/game)
add_subdirectory(src/random)
add_subdirectory(src/agent)
add_subdirectory(src/game_analyzer)
add_subdirectory(src/helpers)

# List of main source files
set(MAIN_SOURCES
    src/main_MC.cpp
    src/main_obs.cpp
)

# List of libraries to link
set(LIBRARIES
    AgentLibrary
    GameLibrary
    RandomLibrary
    GameAnalyzerLibrary
    HelpersLibrary
    OpenMP::OpenMP_CXX
)

# Create executables for each source file
foreach(MAIN_SOURCE ${MAIN_SOURCES})
    # Extract the target name
    get_filename_component(TARGET_NAME ${MAIN_SOURCE} NAME_WE)

    # Add an executable
    add_executable(${TARGET_NAME} ${MAIN_SOURCE})

    # Link libraries to the executable
    target_link_libraries(${TARGET_NAME} PRIVATE ${LIBRARIES})
endforeach()
