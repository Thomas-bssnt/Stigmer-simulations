#include <ctime>   // std::time_t, std::difftime, std::time
#include <fstream> // std::ifstream, std::ofstream
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "game/Game.h"
#include "game_analyzer/GameAnalyzer.h"
#include "helpers/helper_all.h"
#include "random/myRandom.h"

std::vector<double> readValuesObservable(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "The file " << filePath << " could not be opened.\n";
    }

    std::vector<double> observable;
    std::string line;
    while (std::getline(file, line))
    {
        bool round{true};
        std::string number;
        for (const auto x : line)
        {
            if (round)
            {
                if (x == ' ')
                {
                    round = false;
                }
            }
            else
            {
                if (x == ' ')
                {
                    observable.push_back(std::stod(number));
                    break;
                }
                else
                {
                    number += x;
                }
            }
        }
    }
    return observable;
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

double computeError(const std::vector<double> &experimentalObservable, const std::vector<double> &simulatedObservable)
{
    double numerator{0.};
    double denominator{0.};
    for (int i{0}; i < experimentalObservable.size(); ++i)
    {
        numerator += (experimentalObservable[i] - simulatedObservable[i]) *
                     (experimentalObservable[i] - simulatedObservable[i]);
        denominator += experimentalObservable[i] * experimentalObservable[i];
    }
    return numerator / denominator;
}

double computeTotalError(const std::string &pathObservables, const GameAnalyzer &analyzer)
{
    return computeError(readValuesObservable(pathObservables + "q_.txt"), analyzer.get_q()) +
           computeError(readValuesObservable(pathObservables + "Q.txt"), analyzer.get_Q()) +
           computeError(readValuesObservable(pathObservables + "p_.txt"), analyzer.get_p()) +
           computeError(readValuesObservable(pathObservables + "P.txt"), analyzer.get_P()) +
           computeError(readValuesObservable(pathObservables + "IPR_q_.txt"), analyzer.get_IPR_q()) +
           computeError(readValuesObservable(pathObservables + "IPR_Q.txt"), analyzer.get_IPR_Q()) +
           computeError(readValuesObservable(pathObservables + "IPR_p_.txt"), analyzer.get_IPR_p()) +
           computeError(readValuesObservable(pathObservables + "IPR_P.txt"), analyzer.get_IPR_P()) +
           computeError(readValuesObservable(pathObservables + "F_Q.txt"), analyzer.get_F_Q()) +
           computeError(readValuesObservable(pathObservables + "F_P.txt"), analyzer.get_F_P()) +
           computeError(readValuesObservable(pathObservables + "B1.txt"), analyzer.get_B1()) +
           computeError(readValuesObservable(pathObservables + "B2.txt"), analyzer.get_B2()) +
           computeError(readValuesObservable(pathObservables + "B3.txt"), analyzer.get_B3()) +
           computeError(readValuesObservable(pathObservables + "V1.txt"), analyzer.get_V1()) +
           computeError(readValuesObservable(pathObservables + "V2.txt"), analyzer.get_V2()) +
           computeError(readValuesObservable(pathObservables + "V3.txt"), analyzer.get_V3()) +
           computeError(readValuesObservable(pathObservables + "VB1.txt"), analyzer.get_VB1()) +
           computeError(readValuesObservable(pathObservables + "VB2.txt"), analyzer.get_VB2()) +
           computeError(readValuesObservable(pathObservables + "VB3.txt"), analyzer.get_VB3()) +
           computeError(readValuesObservable(pathObservables + "proba_find_99.txt"), analyzer.get_find99()) +
           computeError(readValuesObservable(pathObservables + "proba_find_86_85_84.txt"), analyzer.get_find80()) +
           computeError(readValuesObservable(pathObservables + "proba_find_72_71.txt"), analyzer.get_find70());
}

double getAverageError(
    const int numberOfGames,
    const int numberOfRounds,
    const int numberOfPlayers,
    const std::vector<double> &parametersOpenings,
    const nlohmann::json &parametersRatings,
    const nlohmann::json &fractionPlayersProfiles,
    const std::string &pathObservables)
{
    GameAnalyzer analyzer(numberOfGames, numberOfPlayers);

#pragma omp parallel for
    for (int iGame = 0; iGame < numberOfGames; ++iGame)
    {
        Game game(numberOfRounds, numberOfPlayers);

        std::vector<Agent> agents{initializePlayers(numberOfPlayers, fractionPlayersProfiles, game, parametersOpenings, parametersRatings)};

        for (int iRound{0}; iRound < numberOfRounds; ++iRound)
        {
            for (auto &agent : agents)
            {
                agent.playARound();
            }
        }

        analyzer.analyzeGame(iGame, game, agents);
    }

    return computeTotalError(pathObservables, analyzer);
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
        epsilon = myRandom::rand(-0.1, 0.1);
        break;
    case 2:
    case 4:
    case 6:
        epsilon = myRandom::rand(-10., 10.);
        break;
    case 3:
    case 5:
    case 7:
        epsilon = myRandom::rand(-0.1, 0.1);
        break;
    default:
        std::cerr << "The parameter " << iParameterToChange << " is not implemented.\n";
        break;
    }
    return epsilon;
}

void printCurrentState(const std::vector<double> &parametersOpenings, double averageError)
{
    std::cerr << "ParametersOpenings: ";
    for (const auto &parameter : parametersOpenings)
    {
        std::cerr << parameter << " ";
    }
    std::cerr << "\n";
    std::cerr << "<err> = " << averageError << "\n\n";
}

void doMonteCarloStep(
    const std::vector<bool> &parametersToChange,
    std::vector<double> &bestParametersOpenings,
    double &bestAverageError,
    const int numberOfGames,
    const int numberOfRounds,
    const int numberOfPlayers,
    const nlohmann::json &parametersRatings,
    const nlohmann::json &fractionPlayersProfiles,
    const std::string &pathObservables,
    const std::string &pathParameters)
{
    std::vector<double> parameters{bestParametersOpenings};
    int iParameterToChange;
    do
    {
        iParameterToChange = myRandom::randIndex(parameters.size());
    } while (!parametersToChange[iParameterToChange]);
    parameters[iParameterToChange] += randomSmallChange(parameters, iParameterToChange);

    double averageError{bestAverageError};
    averageError = getAverageError(numberOfGames, numberOfRounds, numberOfPlayers, parameters,
                                   parametersRatings, fractionPlayersProfiles, pathObservables);

    if (averageError < bestAverageError)
    {
        bestAverageError = averageError;
        bestParametersOpenings = parameters;
        writeBestParameters(pathParameters + "cells.txt", bestParametersOpenings);
    }

    printCurrentState(parameters, averageError);
}

int main()
{
    // Parameters of the program
    const int numberOfGamesInEachStep{100000};
    const std::vector<bool> parametersToChange{true, true, true, true, true, true, true, true};
    // const std::string gameType{"R2_intra/R2_intra/"};
    const std::string gameType{"R1/R1/"};

    // Parameters of the game
    const int numberOfRounds{20};
    const int numberOfPlayers{5};

    // Path of the in and out files
    const std::string pathDataFigures{"../../data_figures/"};
    const std::string pathObservables{pathDataFigures + gameType + "exp/observables/"};

    // Get parameters
    const std::string pathParameters{pathDataFigures + gameType + "model/parameters/"};
    const std::vector<double> fractionPlayersProfiles{readParameters(pathParameters + "players_profiles.txt")};
    const nlohmann::json parametersRatings{nlohmann::json::parse(std::ifstream(pathParameters + "stars.json"))};

    // Initialization
    std::vector<double> bestParametersOpenings{readParameters(pathParameters + "cells.txt")};
    double bestAverageError{getAverageError(numberOfGamesInEachStep, numberOfRounds, numberOfPlayers, bestParametersOpenings,
                                            parametersRatings, fractionPlayersProfiles, pathObservables)};
    printCurrentState(bestParametersOpenings, bestAverageError);

    // MC simulation
    while (true)
    {
        doMonteCarloStep(parametersToChange, bestParametersOpenings, bestAverageError, numberOfGamesInEachStep,
                         numberOfRounds, numberOfPlayers, parametersRatings, fractionPlayersProfiles, pathObservables,
                         pathParameters);
    }

    return 0;
}
