# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.27)

# Define the project name
project(Stigmer_simulation)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set C++ compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Xpreprocessor -fopenmp -O2")

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
    src/main_MC_opt_rk.cpp
    src/main_MC_opt_S_group.cpp
    src/main_MC_opt_S_solo.cpp
    src/main_obs.cpp
    src/main_obs_pred_20_players.cpp
    src/main_obs_pred_100_rounds.cpp
    src/main_obs_opt_rk.cpp
    src/main_obs_opt_S_group.cpp
    src/main_obs_opt_S_solo.cpp
)

# List of libraries to link
set(LIBRARIES
    AgentLibrary
    GameLibrary
    RandomLibrary
    GameAnalyzerLibrary
    HelpersLibrary
    -L/opt/homebrew/Cellar/libomp/17.0.6/lib -lomp
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
