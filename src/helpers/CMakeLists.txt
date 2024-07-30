# CMake configuration for the helpers directory

# List source files for the helpers directory
set(HELPERS_SOURCES
    helper_all.cpp
)

# List header files for the helpers directory
set(HELPERS_HEADERS
    helper_all.h
)

# Create a library for the helpers sources
add_library(HelpersLibrary ${HELPERS_SOURCES} ${HELPERS_HEADERS})
