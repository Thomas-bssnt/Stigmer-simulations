#include <algorithm> // std::transform
#include <cmath>     // std::sqrt, std::nan
#include <fstream>   // std::ofstream
#include <numeric>   // std::accumulate, std::iota
#include <stdexcept> // std::runtime_error
#include <string>
#include <vector>

#include "agent/Agent.h"
#include "agent/Cell.h"
#include "game/Game.h"
#include "game_analyzer/GameAnalyzer.h"

GameAnalyzer::GameAnalyzer(int numberOfGames, std::vector<int> iAgents)
    : m_numberOfGames{numberOfGames},
      m_iAgents{iAgents},
      m_numberOfPlayersToAnalyze{static_cast<int>(iAgents.size())},
      m_isInitialized{false}
{
}

GameAnalyzer::GameAnalyzer(int numberOfGames, int numberOfPlayers)
    : GameAnalyzer(numberOfGames, std::vector<int>(numberOfPlayers, 0))
{
    std::iota(m_iAgents.begin(), m_iAgents.end(), 0);
}

void GameAnalyzer::analyzeGame(int iGame, const Game &game, const std::vector<Agent> &agents)
{
#pragma omp critical
    if (!m_isInitialized)
    {
        initializeVariables(game);
        m_isInitialized = true;
    }

    computeDistributions(iGame, game);

    int iAgentToAnalyze{0};
    for (auto iAgent : m_iAgents)
    {
        computeValuesBestCells(iGame, agents[iAgent]);
        computeValuesBestCellsSinceStart(iGame, agents[iAgent]);
        computeReplayBestCells(iGame, agents[iAgent]);
        computeFindBestCells(iGame, agents[iAgent]);
        computeScore(iGame, game, iAgent, iAgentToAnalyze);
        computeRank(iGame, game, iAgent, iAgentToAnalyze);
        computeMNS(iGame, game, iAgent);

        ++iAgentToAnalyze;
    }
}

void GameAnalyzer::saveObservables(std::string pathObservables) const
{
    saveObservable(pathObservables + "q_", get_q());
    saveObservable(pathObservables + "Q", get_Q());
    saveObservable(pathObservables + "p_", get_p());
    saveObservable(pathObservables + "P", get_P());
    saveObservable(pathObservables + "IPR_q_", get_IPR_q());
    saveObservable(pathObservables + "IPR_Q", get_IPR_Q());
    saveObservable(pathObservables + "IPR_p_", get_IPR_p());
    saveObservable(pathObservables + "IPR_P", get_IPR_P());
    saveObservable(pathObservables + "F_Q", get_F_Q());
    saveObservable(pathObservables + "F_P", get_F_P());
    saveObservable(pathObservables + "V1", get_V1());
    saveObservable(pathObservables + "V2", get_V2());
    saveObservable(pathObservables + "V3", get_V3());
    saveObservable(pathObservables + "VB1", get_VB1());
    saveObservable(pathObservables + "VB2", get_VB2());
    saveObservable(pathObservables + "VB3", get_VB3());
    saveObservable(pathObservables + "B1", get_B1());
    saveObservable(pathObservables + "B2", get_B2());
    saveObservable(pathObservables + "B3", get_B3());
    saveObservable(pathObservables + "proba_find_99", get_find99());
    saveObservable(pathObservables + "proba_find_86_85_84", get_find80());
    saveObservable(pathObservables + "proba_find_72_71", get_find70());
    saveObservable(pathObservables + "S", get_S());
    saveObservable(pathObservables + "S_group", get_S_group());
    saveObservable(pathObservables + "S_mean", get_S_mean());
    saveObservable(pathObservables + "S_group_mean", get_S_group_mean());
    saveObservable(pathObservables + "rank", get_rank());
    saveObservable(pathObservables + "rank_mean", get_rank_mean());
    saveObservable(pathObservables + "R", get_MNS());
    // saveObservable(pathObservables + "rk_col", get_rk_col());
    // saveObservable(pathObservables + "rk_neu", get_rk_neu());
    // saveObservable(pathObservables + "rk_def", get_rk_def());
}

std::vector<double> GameAnalyzer::get_q() const { return computeAverage(m_q); }
std::vector<double> GameAnalyzer::get_Q() const { return computeAverage(m_Q); }
std::vector<double> GameAnalyzer::get_p() const { return computeAverage(m_p); }
std::vector<double> GameAnalyzer::get_P() const { return computeAverage(m_P); }
std::vector<double> GameAnalyzer::get_IPR_q() const { return computeAverage(m_IPR_q); }
std::vector<double> GameAnalyzer::get_IPR_Q() const { return computeAverage(m_IPR_Q); }
std::vector<double> GameAnalyzer::get_IPR_p() const { return computeAverage(m_IPR_p); }
std::vector<double> GameAnalyzer::get_IPR_P() const { return computeAverage(m_IPR_P); }
std::vector<double> GameAnalyzer::get_F_Q() const { return computeAverage(m_F_Q); }
std::vector<double> GameAnalyzer::get_F_P() const { return computeAverage(m_F_P); }
std::vector<double> GameAnalyzer::get_B1() const { return computeAverage(m_B[0]); }
std::vector<double> GameAnalyzer::get_B2() const { return computeAverage(m_B[1]); }
std::vector<double> GameAnalyzer::get_B3() const { return computeAverage(m_B[2]); }
std::vector<double> GameAnalyzer::get_V1() const { return computeAverage(m_V[0]); }
std::vector<double> GameAnalyzer::get_V2() const { return computeAverage(m_V[1]); }
std::vector<double> GameAnalyzer::get_V3() const { return computeAverage(m_V[2]); }
std::vector<double> GameAnalyzer::get_VB1() const { return computeAverage(m_VB[0]); }
std::vector<double> GameAnalyzer::get_VB2() const { return computeAverage(m_VB[1]); }
std::vector<double> GameAnalyzer::get_VB3() const { return computeAverage(m_VB[2]); }
std::vector<double> GameAnalyzer::get_find99() const { return computeAverage(m_find[0]); }
std::vector<double> GameAnalyzer::get_find80() const { return computeAverage(m_find[1]); }
std::vector<double> GameAnalyzer::get_find70() const { return computeAverage(m_find[2]); }
std::vector<double> GameAnalyzer::get_S() const { return computeDistribution(m_S, 50, 0, 1); }
double GameAnalyzer::get_S_mean() const { return computeAverage(m_S); }
std::vector<double> GameAnalyzer::get_S_group() const { return computeDistribution(m_S_group, 50, 0., 1.); }
double GameAnalyzer::get_S_group_mean() const { return computeAverage(m_S_group); }
std::vector<double> GameAnalyzer::get_rank() const { return computeDistribution(m_rank, 5, 1, 6); }
double GameAnalyzer::get_rank_mean() const { return computeAverage(m_rank); }
std::vector<double> GameAnalyzer::get_MNS() const { return divide(m_MNS_ratings, m_MNS_counts); }

void GameAnalyzer::initializeVariables(const Game &game)
{
    m_numberOfRounds = game.m_numberOfRounds;
    m_numberOfTurns = game.m_numberOfTurns;
    m_numberOfCells = game.m_map.getNumberOfCells();
    //
    m_q = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_Q = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_p = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_P = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_IPR_q = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_IPR_Q = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_IPR_p = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_IPR_P = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_F_Q = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_F_P = std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.));
    m_B = std::vector<std::vector<std::vector<double>>>(m_numberOfTurns, std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.)));
    m_V = std::vector<std::vector<std::vector<double>>>(m_numberOfTurns, std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.)));
    m_VB = std::vector<std::vector<std::vector<double>>>(m_numberOfTurns, std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.)));
    m_find = std::vector<std::vector<std::vector<double>>>(m_numberOfTurns, std::vector<std::vector<double>>(m_numberOfRounds, std::vector<double>(m_numberOfGames, 0.)));
    m_S = std::vector<double>(m_numberOfGames * m_numberOfPlayersToAnalyze, 0.);
    m_S_group = std::vector<double>(m_numberOfGames, 0.);
    m_rank = std::vector<double>(m_numberOfGames * m_numberOfPlayersToAnalyze, 0.);
    m_MNS_ratings = std::vector<int>(100, 0);
    m_MNS_counts = std::vector<int>(100, 0);
    // TODO: change 100 to Vmax1 et calculer Vmax1, VMax2, ... ici
}

void GameAnalyzer::computeDistributions(int iGame, const Game &game)
{
    std::vector<std::vector<int>> oMapInst(m_numberOfRounds, std::vector<int>(m_numberOfCells, 0));
    std::vector<std::vector<int>> rMapInst(m_numberOfRounds, std::vector<int>(m_numberOfCells, 0));
    std::vector<std::vector<int>> oMapCum(m_numberOfRounds, std::vector<int>(m_numberOfCells, 0));
    std::vector<std::vector<int>> rMapCum(m_numberOfRounds, std::vector<int>(m_numberOfCells, 0));
    for (auto iAgent : m_iAgents)
    {
        for (int iRound{0}; iRound < m_numberOfRounds; ++iRound)
        {
            for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
            {
                const int iCell{game.m_iCellOpened[iAgent][iRound][iTurn]};
                const int rCell{game.m_rCellOpened[iAgent][iRound][iTurn]};

                ++oMapInst[iRound][iCell];
                rMapInst[iRound][iCell] += rCell;

                for (int iRound2{iRound}; iRound2 < m_numberOfRounds; ++iRound2)
                {
                    ++oMapCum[iRound2][iCell];
                    rMapCum[iRound2][iCell] += rCell;
                }
            }
        }
    }

    const int Vmax1{99}; // TODO: change that
    const int Vmax2{86}; // TODO: change that
    const int Vmax3{86}; // TODO: change that

    for (int iRound{0}; iRound < game.m_numberOfRounds; ++iRound)
    {
        const std::vector<double> oDistInst{normalize(oMapInst[iRound])};
        const std::vector<double> rDistInst{normalize(rMapInst[iRound])};
        const std::vector<double> oDistCum{normalize(oMapCum[iRound])};
        const std::vector<double> rDistCum{normalize(rMapCum[iRound])};

        m_q[iRound][iGame] = computePerf(oDistInst, game.m_map.getValues(), (Vmax1 + Vmax2 + Vmax3) / 3.);
        m_Q[iRound][iGame] = computePerf(oDistCum, game.m_map.getValues(), (Vmax1 + Vmax2 + Vmax3) / 3.);
        m_p[iRound][iGame] = computePerf(rDistInst, game.m_map.getValues(), Vmax1);
        m_P[iRound][iGame] = computePerf(rDistCum, game.m_map.getValues(), Vmax1);
        m_IPR_q[iRound][iGame] = computeIPR(oDistInst);
        m_IPR_Q[iRound][iGame] = computeIPR(oDistCum);
        m_IPR_p[iRound][iGame] = computeIPR(rDistInst);
        m_IPR_P[iRound][iGame] = computeIPR(rDistCum);
        m_F_Q[iRound][iGame] = computeF(oDistCum, game.m_map.getValues());
        m_F_P[iRound][iGame] = computeF(rDistCum, game.m_map.getValues());
    }
}

void GameAnalyzer::computeValuesBestCells(int iGame, const Agent &agent)
{
    for (int iRound{0}; iRound < m_numberOfRounds; ++iRound)
    {
        for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
        {
            m_V[iTurn][iRound][iGame] += agent.m_bestCells[iRound][iTurn].value / static_cast<double>(m_numberOfPlayersToAnalyze);
        }
    }
}

void GameAnalyzer::computeValuesBestCellsSinceStart(int iGame, const Agent &agent)
{
    std::vector<Cell> bestCells(m_numberOfTurns, {-1, -1});
    for (int iRound{0}; iRound < m_numberOfRounds; ++iRound)
    {
        for (const auto &cellPlayed : agent.m_bestCells[iRound])
        {
            for (int i{0}; i < m_numberOfTurns; ++i)
            {
                if (cellPlayed.index == bestCells[i].index)
                {
                    break;
                }
                if (cellPlayed.value > bestCells[i].value)
                {
                    bestCells.insert(bestCells.begin() + i, cellPlayed);
                    bestCells.pop_back();
                    break;
                }
            }
        }

        for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
        {
            m_VB[iTurn][iRound][iGame] += bestCells[iTurn].value / static_cast<double>(m_numberOfPlayersToAnalyze);
        }
    }
}

void GameAnalyzer::computeReplayBestCells(int iGame, const Agent &agent)
{
    std::vector<std::vector<int>> playBestCellsRound(m_numberOfTurns, std::vector<int>(m_numberOfRounds, 0));
    for (int iRound{1}; iRound < m_numberOfRounds; ++iRound)
    {
        for (auto &cellPlayed : agent.m_bestCells[iRound])
        {
            for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
            {
                if (cellPlayed.index == agent.m_bestCells[iRound - 1][iTurn].index)
                {
                    m_B[iTurn][iRound][iGame] += 1 / static_cast<double>(m_numberOfPlayersToAnalyze);
                }
            }
        }
    }
}

void GameAnalyzer::computeFindBestCells(int iGame, const Agent &agent)
{ // TODO: do not work if the game is not sorted
    std::vector<std::vector<bool>> findings(m_numberOfRounds, std::vector<bool>(m_numberOfCells, false));
    for (int iRound{0}; iRound < m_numberOfRounds; ++iRound)
    {
        for (auto &cell : agent.m_bestCells[iRound])
        {
            if (!findings[iRound][cell.index])
            {
                for (int iRound2{iRound}; iRound2 < m_numberOfRounds; ++iRound2)
                {
                    findings[iRound2][cell.index] = true;
                }
            }
        }
    }

    for (int iRound{0}; iRound < m_numberOfRounds; ++iRound)
    {
        for (int i{0}; i < m_numberOfCells / 225; ++i) // for larger maps
        {
            m_find[0][iRound][iGame] += findings[iRound][i * 225 + 0] /
                                        static_cast<double>(m_numberOfPlayersToAnalyze);
            m_find[1][iRound][iGame] += (findings[iRound][i * 225 + 1] +
                                         findings[iRound][i * 225 + 2] +
                                         findings[iRound][i * 225 + 3] +
                                         findings[iRound][i * 225 + 4]) /
                                        static_cast<double>(4 * m_numberOfPlayersToAnalyze);
            m_find[2][iRound][iGame] += (findings[iRound][i * 225 + 5] +
                                         findings[iRound][i * 225 + 6] +
                                         findings[iRound][i * 225 + 7] +
                                         findings[iRound][i * 225 + 8]) /
                                        static_cast<double>(4 * m_numberOfPlayersToAnalyze);
        }
    }
}

void GameAnalyzer::computeScore(int iGame, const Game &game, int iAgent, int iAgentToAnalyze)
{
    const int Smax{(99 + 86 + 86) * game.m_numberOfRounds}; // TODO: change that

    const double normalizedScore{game.getScoreOfPlayer(iAgent) / static_cast<double>(Smax)};
    m_S[iGame * m_numberOfPlayersToAnalyze + iAgentToAnalyze] = normalizedScore;
    m_S_group[iGame] += normalizedScore / m_numberOfPlayersToAnalyze;
}

void GameAnalyzer::computeRank(int iGame, const Game &game, int iAgent, int iAgentToAnalyze)
{
    m_rank[iGame * m_numberOfPlayersToAnalyze + iAgentToAnalyze] = 1;
    for (int iOtherAgent{0}; iOtherAgent < game.m_numberOfPlayers; ++iOtherAgent)
    {
        if (iOtherAgent != iAgent)
        {
            if (game.getScoreOfPlayer(iOtherAgent) > game.getScoreOfPlayer(iAgent))
            {
                ++m_rank[iGame * m_numberOfPlayersToAnalyze + iAgentToAnalyze];
            }
        }
    }
}

void GameAnalyzer::computeMNS(int iGame, const Game &game, int iAgent)
{
    for (int iRound{0}; iRound < m_numberOfRounds; ++iRound)
    {
        for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
        {
            const int vCell{game.m_vCellOpened[iAgent][iRound][iTurn]};
            const int rCell{game.m_rCellOpened[iAgent][iRound][iTurn]};
            m_MNS_ratings[vCell] += rCell;
            m_MNS_counts[vCell]++;
        }
    }
}

double GameAnalyzer::computePerf(const std::vector<double> &distribution, const std::vector<int> &values, double normalization)
{
    double perf{0.};
    for (int iCell{0}; iCell < distribution.size(); ++iCell)
    {
        perf += distribution[iCell] * values[iCell];
    }
    return perf / normalization;
}

double GameAnalyzer::computeIPR(const std::vector<double> &distribution)
{
    double sumSquared{0.};
    for (const auto &value : distribution)
    {
        sumSquared += value * value;
    }
    return sumSquared == 0. ? 0. : 1. / sumSquared;
}

double GameAnalyzer::computeF(const std::vector<double> &distribution, const std::vector<int> &values)
{
    double sumSqrt{0.};
    int sumValues{0};
    for (int iCell{0}; iCell < distribution.size(); ++iCell)
    {
        sumValues += values[iCell];
        sumSqrt += std::sqrt(distribution[iCell] * values[iCell]);
    }
    return sumSqrt / std::sqrt(sumValues);
}

std::vector<double> GameAnalyzer::normalize(const std::vector<int> &vector)
{
    const double sum{std::accumulate(vector.begin(), vector.end(), 0.)};
    std::vector<double> normalizedVector(vector.size(), 0.);
    if (sum != 0.)
    {
        for (int i{0}; i < vector.size(); ++i)
        {
            normalizedVector[i] = vector[i] / sum;
        }
    }
    return normalizedVector;
}

std::vector<double> GameAnalyzer::divide(const std::vector<int> &numerator, const std::vector<int> &denominator)
{
    std::vector<double> vector(numerator.size(), 0.);
    for (int iCell{0}; iCell < numerator.size(); ++iCell)
    {
        vector[iCell] = denominator[iCell] == 0 ? std::nan("") : numerator[iCell] / static_cast<double>(denominator[iCell]);
    }
    return vector;
}

double GameAnalyzer::computeAverage(const std::vector<double> &vector)
{
    return std::accumulate(vector.begin(), vector.end(), 0.) / vector.size();
}

std::vector<double> GameAnalyzer::computeAverage(const std::vector<std::vector<double>> &vector2d)
{
    std::vector<double> averagedVector(vector2d.size(), 0.);
    std::transform(vector2d.begin(), vector2d.end(), averagedVector.begin(), [](const std::vector<double> &vector1d)
                   { return computeAverage(vector1d); });
    return averagedVector;
}

std::vector<double> GameAnalyzer::computeDistribution(const std::vector<double> &vector, int numberOfDivisions, double min, double max)
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

void GameAnalyzer::saveObservable(const std::string &observablePath, const std::vector<double> &observable) const
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

void GameAnalyzer::saveObservable(const std::string &observablePath, double observable) const
{
    saveObservable(observablePath, std::vector<double>{observable});
}
