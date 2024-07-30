#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "game/Game.h"
#include "game_analyzer/GameAnalyzer.h"
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

double computeAverage(const std::vector<double> &vector)
{
    return std::accumulate(vector.begin(), vector.end(), 0.) / vector.size();
}

std::vector<double> computeAverage(const std::vector<std::vector<double>> &vector2d)
{
    std::vector<double> averagedVector(vector2d.size(), 0.);
    std::transform(vector2d.begin(), vector2d.end(), averagedVector.begin(), [](const std::vector<double> &vector1d)
                   { return computeAverage(vector1d); });
    return averagedVector;
}

std::vector<double> computeDistribution(const std::vector<double> &vector, int numberOfDivisions, double min, double max)
{
    // Do the histogram
    std::vector<int> counts(numberOfDivisions, 0);
    for (auto &value : vector)
    {
        int binNumber{value < max
                          ? static_cast<int>((value - min) / (max - min) * numberOfDivisions)
                          : numberOfDivisions - 1};
        counts[binNumber]++;
    }

    // Normalization
    std::vector<double> countsNormalized(numberOfDivisions, 0.);
    for (int iDivisions{0}; iDivisions < numberOfDivisions; ++iDivisions)
    {
        countsNormalized[iDivisions] = static_cast<double>(counts[iDivisions]) / vector.size() * numberOfDivisions / (max - min);
    }
    return countsNormalized;
}

void saveObservable(const std::string &observablePath, const std::vector<double> &observable)
{
    std::ofstream file(observablePath + ".txt");
    if (!file.is_open())
    {
        throw std::runtime_error("The file " + observablePath + " could not be opened.");
    }

    for (const auto &value : observable)
    {
        file << value << "\n";
    }
}

void saveObservable(const std::string &observablePath, double observable)
{
    saveObservable(observablePath, std::vector<double>{observable});
}

int main()
{
    // Parameters of the program
    const int numberOfReps{100000};
    const int numberOfGames{10};
    const std::vector<std::string> gameTypes{"R2_intra/R2_intra/"};

    const std::string pathDataFigures{"../../data_figures/"};

    // Parameters of the game
    const int numberOfRounds{20};
    const int numberOfPlayers{5};

    for (const auto &gameType : gameTypes)
    {
        std::cout << gameType << "\n";

        const std::string pathParametersMimic{pathDataFigures + gameType + "model/parameters/"};
        const std::vector<double> fractionPlayersProfilesMimic{readParameters(pathParametersMimic + "players_profiles.txt")};
        const std::vector<double> parametersOpeningsMimic{readParameters(pathParametersMimic + "cells.txt")};
        const nlohmann::json parametersRatingsMimic{nlohmann::json::parse(std::ifstream(pathParametersMimic + "stars.json"))};

        const std::string pathParametersOpt{pathDataFigures + gameType + "opt/opt_rk/parameters/"};
        const std::vector<double> parametersOpeningsOpt{readParameters(pathParametersOpt + "cells.txt")};
        const std::vector<double> parametersRatingsOpt{readParameters(pathParametersOpt + "stars.txt")};

        GameAnalyzer analyzerOpt(numberOfReps * numberOfGames, std::vector<int>{0});
        GameAnalyzer analyzerMimic(numberOfReps * numberOfGames, std::vector<int>{1, 2, 3, 4});
        GameAnalyzer analyzer(numberOfReps * numberOfGames, 5);
        std::vector<double> ranksOpt(numberOfReps, 1.);

#pragma omp parallel for
        for (int iRep = 0; iRep < numberOfReps; ++iRep)
        {
            std::vector<Agent> agents{};
            agents.emplace_back(parametersOpeningsOpt, RatingStrategy(parametersRatingsOpt, "mns_linear"));
            std::vector<Agent> agentsMimic{initializePlayers(4, fractionPlayersProfilesMimic, parametersOpeningsMimic, parametersRatingsMimic)};
            agents.insert(agents.end(), agentsMimic.begin(), agentsMimic.end());

            std::vector<int> scores(agents.size(), 0);
            for (int iGame{0}; iGame < 10; ++iGame)
            {
                Game game(numberOfRounds, numberOfPlayers);
                for (auto &agent : agents)
                {
                    agent.assignToGame(&game);
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
                    scores[iAgent] += game.getScoreOfPlayer(iAgent);
                }

                analyzerOpt.analyzeGame(iRep * numberOfGames + iGame, game, agents);
                analyzerMimic.analyzeGame(iRep * numberOfGames + iGame, game, agents);
                analyzer.analyzeGame(iRep * numberOfGames + iGame, game, agents);
            }

            for (int iAgent{1}; iAgent < agents.size(); ++iAgent)
            {
                if (scores[0] < scores[iAgent])
                {
                    ++ranksOpt[iRep];
                }
            }
        }

        analyzerOpt.saveObservables(pathDataFigures + gameType + "opt/opt_rk/observables_opt/");
        analyzerMimic.saveObservables(pathDataFigures + gameType + "opt/opt_rk/observables_mimic/");
        analyzer.saveObservables(pathDataFigures + gameType + "opt/opt_rk/observables/");
        saveObservable(pathDataFigures + gameType + "opt/opt_rk/observables_opt/rank_10", computeDistribution(ranksOpt, 5, 1, 6));
        saveObservable(pathDataFigures + gameType + "opt/opt_rk/observables_opt/rank_10_mean", computeAverage(ranksOpt));
    }

    return 0;
}
