# CMake configuration for the random directory

# List source files for the random directory
set(RANDOM_SOURCES
    myRandom.cpp
    myRandom.tpp
)

# List header files for the random directory
set(RANDOM_HEADERS
    myRandom.h
)

# Create a library for the random sources
add_library(RandomLibrary ${RANDOM_SOURCES} ${RANDOM_HEADERS})
