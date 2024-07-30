#include <ctime>   // std::time_t, std::difftime, std::time
#include <fstream> // std::ifstream, std::ofstream
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "agent/RatingStrategy.h"
#include "game/Game.h"
#include "helpers/helper_all.h"
#include "random/myRandom.h"

std::vector<Agent> initializePlayers(int numberOfAgents,
                                     const std::vector<double> &fractions,
                                     const std::vector<double> &parametersVisits,
                                     const nlohmann::json &parametersStars)
{
    const std::vector<std::string> profiles{"col", "neu", "def"};

    std::vector<Agent> agents;
    agents.reserve(numberOfAgents);
    for (int iAgent{0}; iAgent < numberOfAgents; ++iAgent)
    {
        const std::string profile{myRandom::choice(profiles, fractions)};
        agents.emplace_back(parametersVisits, parametersStars[profile]);
    }

    return agents;
}

void writeBestParameters(const std::string &filePath, const std::vector<double> &bestParameters)
{
    std::ofstream file(filePath, std::ios::app);
    if (!file.is_open())
    {
        std::cerr << "The file " << filePath << " could not be opened.\n";
    }

    file << bestParameters[0];
    for (int iParameter{1}; iParameter < bestParameters.size(); ++iParameter)
    {
        file << " " << bestParameters[iParameter];
    }
    file << "\n";
}

double getAverageRank(
    const int numberOfGames,
    const int numberOfRounds,
    const int numberOfPlayers,
    const std::vector<double> &parametersOpeningsOpt,
    const nlohmann::json &parametersRatingsOpt,
    const std::vector<double> &parametersOpeningsMimic,
    const nlohmann::json &parametersRatingsMimic,
    const nlohmann::json &fractionPlayersProfilesMimic)
{
    int S_tot{0};

#pragma omp parallel for reduction(+ : S_tot)
    for (int iGame = 0; iGame < numberOfGames; ++iGame)
    {
        Game game(numberOfRounds, numberOfPlayers);

        std::vector<Agent> agents{};
        agents.emplace_back(parametersOpeningsOpt, RatingStrategy(parametersRatingsOpt, "mns_linear"));
        std::vector<Agent> agentsMimic{initializePlayers(4, fractionPlayersProfilesMimic, parametersOpeningsMimic, parametersRatingsMimic)};
        agents.insert(agents.end(), agentsMimic.begin(), agentsMimic.end());

        for (int iRound{0}; iRound < numberOfRounds; ++iRound)
        {
            for (auto &agent : agents)
            {
                agent.playARound();
            }
        }

        S_tot += game.getScoreOfPlayer(0);
    }

    return S_tot / static_cast<double>(numberOfGames) / 5420;
}

double randomSmallChange(const std::vector<double> &parameters, const int iParameterToChange)
{
    double epsilon{0.};
    switch (iParameterToChange)
    {
    case 0:
        do
        {
            epsilon = myRandom::rand(-0.1, 0.1);
        } while (parameters[0] - epsilon < 0 ||
                 parameters[0] + epsilon < 0 ||
                 parameters[0] - epsilon > 1 ||
                 parameters[0] + epsilon > 1);
        break;
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 9:
        epsilon = myRandom::rand(-0.1, 0.1);
        break;
    case 2:
    case 4:
    case 6:
        epsilon = myRandom::rand(-10., 10.);
        break;
    default:
        std::cerr << "The parameter " << iParameterToChange << " is not implemented.\n";
        break;
    }
    return epsilon;
}

void printCurrentState(const std::vector<double> &parametersOpeningsOpt, const std::vector<double> &parametersRatingsOpt, double averageScore)
{
    std::cerr << "ParametersOpenings: ";
    for (const auto &parameter : parametersOpeningsOpt)
    {
        std::cerr << parameter << " ";
    }
    std::cerr << "\n";
    std::cerr << "ParametersRatings: ";
    for (const auto &parameter : parametersRatingsOpt)
    {
        std::cerr << parameter << " ";
    }
    std::cerr << "\n";
    std::cerr << "<S> = " << averageScore << "\n\n";
}

void doMonteCarloStep(
    const std::vector<bool> &parametersToChange,
    const int numberOfReps,
    const int numberOfRounds,
    const int numberOfPlayers,
    double &bestAverageScore,
    std::vector<double> &bestParametersOpeningsOpt,
    std::vector<double> &bestParametersRatingsOpt,
    const std::vector<double> &parametersOpeningsMimic,
    const nlohmann::json &parametersRatingsMimic,
    const nlohmann::json &fractionPlayersProfilesMimic,
    const std::string &pathParametersOpt)
{
    std::vector<double> parameters{bestParametersOpeningsOpt};
    parameters.insert(parameters.end(), bestParametersRatingsOpt.begin(), bestParametersRatingsOpt.end());

    int iParameterToChange;
    do
    {
        iParameterToChange = myRandom::randIndex(parameters.size());
    } while (!parametersToChange[iParameterToChange]);
    parameters[iParameterToChange] += randomSmallChange(parameters, iParameterToChange);

    std::vector<double> parametersOpenings(parameters.begin(), parameters.begin() + 8);
    std::vector<double> parametersRatings(parameters.begin() + 8, parameters.end());

    double averageScore{bestAverageScore};
    averageScore = getAverageRank(numberOfReps, numberOfRounds, numberOfPlayers,
                                  parametersOpenings, parametersRatings,
                                  parametersOpeningsMimic, parametersRatingsMimic, fractionPlayersProfilesMimic);

    if (averageScore > bestAverageScore)
    {
        bestAverageScore = averageScore;
        bestParametersOpeningsOpt = parametersOpenings;
        bestParametersRatingsOpt = parametersRatings;
        writeBestParameters(pathParametersOpt + "cells.txt", bestParametersOpeningsOpt);
        writeBestParameters(pathParametersOpt + "stars.txt", bestParametersRatingsOpt);
    }

    printCurrentState(parametersOpenings, parametersRatings, averageScore);
}

int main()
{
    // Parameters of the program
    const int numberOfGamesInEachStep{50000};
    const std::vector<bool> parametersToChange{true, true, true, true, true, true, true, true,
                                               true, true};
    const std::string gameType{"R2_intra/R2_intra/"};
    const std::string pathDataFigures{"../../data_figures/"};

    // Parameters of the game
    const int numberOfRounds{20};
    const int numberOfPlayers{5};

    // Get parameters Mimic
    const std::string pathParametersMimic{pathDataFigures + gameType + "model/parameters/"};
    const std::vector<double> fractionPlayersProfilesMimic{readParameters(pathParametersMimic + "players_profiles.txt")};
    const std::vector<double> parametersOpeningsMimic{readParameters(pathParametersMimic + "cells.txt")};
    const nlohmann::json parametersRatingsMimic{nlohmann::json::parse(std::ifstream(pathParametersMimic + "stars.json"))};

    // Get parameters Opt
    const std::string pathParametersOpt{pathDataFigures + gameType + "opt/opt_S_solo/parameters/"};
    std::vector<double> bestParametersOpeningsOpt{readParameters(pathParametersOpt + "cells.txt")};
    std::vector<double> bestParametersRatingsOpt{readParameters(pathParametersOpt + "stars.txt")};

    // Initialization
    double bestAverageScore{getAverageRank(numberOfGamesInEachStep, numberOfRounds, numberOfPlayers,
                                           bestParametersOpeningsOpt, bestParametersRatingsOpt,
                                           parametersOpeningsMimic, parametersRatingsMimic, fractionPlayersProfilesMimic)};
    printCurrentState(bestParametersOpeningsOpt, bestParametersRatingsOpt, bestAverageScore);

    // MC simulation
    while (true)
    {
        doMonteCarloStep(parametersToChange,
                         numberOfGamesInEachStep, numberOfRounds, numberOfPlayers,
                         bestAverageScore, bestParametersOpeningsOpt, bestParametersRatingsOpt,
                         parametersOpeningsMimic, parametersRatingsMimic, fractionPlayersProfilesMimic,
                         pathParametersOpt);
    }

    return 0;
}
