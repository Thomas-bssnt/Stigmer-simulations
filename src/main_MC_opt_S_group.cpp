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
                    round = true;
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

double getAverageScore(
    const int numberOfGames,
    const int numberOfRounds,
    const int numberOfPlayers,
    const std::vector<double> &parametersOpenings,
    const nlohmann::json &parametersRatings)
{
    int S_tot{0};

#pragma omp parallel for reduction(+ : S_tot)
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

        for (int iAgent{0}; iAgent < agents.size(); ++iAgent)
        {
            S_tot += game.getScoreOfPlayer(iAgent);
        }
    }

    return S_tot / static_cast<double>(numberOfGames * numberOfPlayers) / 5420;
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
    averageScore = getAverageScore(numberOfReps, numberOfRounds, numberOfPlayers,
                                   parametersOpenings, parametersRatings);

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
    const int numberOfGamesInEachStep{20000};
    const std::vector<bool> parametersToChange{true, true, false, false, false, false, false, false,
                                               true, true};
    const std::string gameType{"R2_intra/R2_intra/"};
    const std::string pathDataFigures{"../../data_figures/"};

    // Parameters of the game
    const int numberOfRounds{20};
    const int numberOfPlayers{5};

    // Get parameters Opt
    const std::string pathParametersOpt{pathDataFigures + gameType + "opt/opt_S_group/parameters/"};
    std::vector<double> bestParametersOpeningsOpt{readParameters(pathParametersOpt + "cells.txt")};
    std::vector<double> bestParametersRatingsOpt{readParameters(pathParametersOpt + "stars.txt")};

    // Initialization
    double bestAverageScore{getAverageScore(numberOfGamesInEachStep, numberOfRounds, numberOfPlayers,
                                            bestParametersOpeningsOpt, bestParametersRatingsOpt)};
    printCurrentState(bestParametersOpeningsOpt, bestParametersRatingsOpt, bestAverageScore);

    // MC simulation
    while (true)
    {
        doMonteCarloStep(parametersToChange,
                         numberOfGamesInEachStep, numberOfRounds, numberOfPlayers,
                         bestAverageScore, bestParametersOpeningsOpt, bestParametersRatingsOpt,
                         pathParametersOpt);
    }

    return 0;
}
