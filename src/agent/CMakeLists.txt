# CMake configuration for the agent directory

# List source files for the agent directory
set(AGENT_SOURCES
    Agent.cpp
    OpeningStrategy.cpp
    RatingStrategy.cpp
)

# List header files for the agent directory
set(AGENT_HEADERS
    Agent.h
    OpeningStrategy.h
    RatingStrategy.h
)

# Create a library for the agent sources
add_library(AgentLibrary ${AGENT_SOURCES} ${AGENT_HEADERS})
