#include <fstream> // std::ifstream
#include <string>  // std::string
#include <vector>  // std::vector

#include <nlohmann/json.hpp> // nlohmann::json, nlohmann::json::parse

#include "agent/Agent.h"                // Agent
#include "game/Game.h"                  // Game
#include "game_analyzer/GameAnalyzer.h" // GameAnalyzer
#include "helpers/helper_all.h"         // readParameters, initializePlayers

int main()
{
    // Parameters of the simulation
    const int numberOfGames{100000};
    const int numberOfRounds{20};
    const int numberOfPlayers{5};

    const std::string pathData{"./data/example/"};

    // Read the parameters of the agents
    const std::string pathParameters{pathData + "model/parameters/"};
    const std::vector<double> fractionPlayersProfiles{readParameters(pathParameters + "players_profiles.txt")};
    const std::vector<double> parametersOpenings{readParameters(pathParameters + "cells.txt")};
    const nlohmann::json parametersRatings{nlohmann::json::parse(std::ifstream(pathParameters + "stars.json"))};

    // Initialize the analyzer
    GameAnalyzer analyzer(numberOfGames, numberOfPlayers);

    // Loop over all repetitions of the game
#pragma omp parallel for
    for (int iGame = 0; iGame < numberOfGames; ++iGame)
    {
        // Initialize the game
        Game game(numberOfRounds, numberOfPlayers);

        // Initialize the agents
        std::vector<Agent> agents{initializePlayers(numberOfPlayers, fractionPlayersProfiles, game, parametersOpenings, parametersRatings)};

        // Play the game
        for (int iRound{0}; iRound < numberOfRounds; ++iRound)
        {
            for (auto &agent : agents)
            {
                agent.playARound();
            }
        }

        // Analyze the game
        analyzer.analyzeGame(iGame, game, agents);
    }

    // Average the observables over all repetition and save them
    analyzer.saveObservables(pathData + "model/observables/");

    return 0;
}
