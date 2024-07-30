# CMake configuration for the game_analyzer directory

# List source files for the game_analyzer directory
set(GAME_ANALYZER_SOURCES
    GameAnalyzer.cpp
)

# List header files for the game_analyzer directory
set(GAME_ANALYZER_HEADERS
    GameAnalyzer.h
)

# Create a library for the game_analyzer sources
add_library(GameAnalyzerLibrary ${GAME_ANALYZER_SOURCES} ${GAME_ANALYZER_HEADERS})
