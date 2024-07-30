#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "game/Game.h"
#include "game_analyzer/GameAnalyzer.h"
#include "helpers/helper_all.h"

int main()
{
    // Parameters of the program
    const int numberOfGames{100000};
    const std::string gameType{"R2_intra/R2_intra/"};

    const std::string pathDataFigures{"../../data_figures/"};

    // Parameters of the game
    const int numberOfRounds{20};
    const int numberOfPlayers{5};

    const std::string pathParameters{pathDataFigures + gameType + "opt/opt_S_group/parameters/"};
    const std::vector<double> parametersOpenings{readParameters(pathParameters + "cells.txt")};
    const std::vector<double> parametersRatings{readParameters(pathParameters + "stars.txt")};

    GameAnalyzer analyzer(numberOfGames, numberOfPlayers);

#pragma omp parallel for
    for (int iGame = 0; iGame < numberOfGames; ++iGame)
    {
        Game game(numberOfRounds, numberOfPlayers);

        std::vector<Agent> agents{};
        for (int iAgent{0}; iAgent < numberOfPlayers; ++iAgent)
        {
            agents.emplace_back(game.getAddress(), parametersOpenings, RatingStrategy(parametersRatings, "mns_linear"));
        }

        for (int iRound{0}; iRound < numberOfRounds; ++iRound)
        {
            for (auto &agent : agents)
            {
                agent.playARound();
            }
        }

        analyzer.analyzeGame(iGame, game, agents);
    }

    analyzer.saveObservables(pathDataFigures + gameType + "opt/opt_S/observables/");

    return 0;
}
