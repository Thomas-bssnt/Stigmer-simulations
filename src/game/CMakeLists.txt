# CMake configuration for the game directory

# List source files for the game directory
set(GAME_SOURCES
    Game.cpp
    Map.cpp
    Rule.cpp
)

# List header files for the game directory
set(GAME_HEADERS
    Game.h
    Map.cpp
    Rule.cpp
)

# Create a library for the game sources
add_library(GameLibrary ${GAME_SOURCES} ${GAME_HEADERS})
