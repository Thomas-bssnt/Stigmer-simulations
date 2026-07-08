#include <algorithm> // std::all_of, std::any_of, std::transform
#include <cmath>     // std::pow
#include <numeric>   // std::accumulate
#include <vector>

#include "agent/Cell.h"
#include "agent/OpeningStrategy.h"
#include "random/myRandom.h"

OpeningStrategy::OpeningStrategy(const std::vector<double> &parametersExploration,
                                 const std::vector<std::vector<double>> &parametersReplayCell)
    : m_parametersExploration{parametersExploration},
      m_parametersReplayCell{parametersReplayCell},
      m_exploringProbabilities{},
      m_round{-1}
{
}

// ! Only works for 2 parameters for the exploration and 3 x 2 parameters for the replay cell
OpeningStrategy::OpeningStrategy(const std::vector<double> &parametersOpenings)
    : OpeningStrategy(std::vector<double>(parametersOpenings.begin(), parametersOpenings.begin() + 2),
                      to2d(std::vector<double>(parametersOpenings.begin() + 2, parametersOpenings.end()), 3))
{
}

void OpeningStrategy::updateExploringProbabilities(const std::vector<double> &colors)
{
    const int numberOfCells{static_cast<int>(colors.size())};

    const bool noRatings{std::all_of(colors.begin(), colors.end(), [](double x)
                                     { return x == 0.; })};
    if (noRatings)
    {
        m_exploringProbabilities = std::vector<double>(numberOfCells, 1. / numberOfCells);
    }
    else
    {
        std::vector<double> powers(numberOfCells);
        std::transform(colors.begin(), colors.end(), powers.begin(),
                       [alpha = m_parametersExploration[1]](double x)
                       { return std::pow(x, alpha); });
        const double sumPowers{std::accumulate(powers.begin(), powers.end(), 0.)};

        for (int iCell{0}; iCell < numberOfCells; ++iCell)
        {
            m_exploringProbabilities[iCell] = m_parametersExploration[0] / numberOfCells +
                                              (1 - m_parametersExploration[0]) * powers[iCell] / sumPowers;
        }
    }
}

int OpeningStrategy::choseCell(int round, const std::vector<double> &colors, const std::vector<std::vector<Cell>> &bestCells,
                               const std::vector<Cell> &cellsPlayed)
{
    // If the round has changed, then the probabilities of exploring each cell must be updated
    if (round != m_round)
    {
        m_round = round;
        updateExploringProbabilities(colors);
    }

    const int iTurn = cellsPlayed.size();
    if (round != 0)
    {
        if (shouldReplayCell(bestCells[round - 1][iTurn].value, iTurn))
        {
            return bestCells[round - 1][iTurn].index;
        }
    }
    return chooseACellByExploring(round, m_exploringProbabilities, bestCells, cellsPlayed);
}

bool OpeningStrategy::shouldReplayCell(int value, int iTurn) const
{
    return myRandom::rand() < m_parametersReplayCell[iTurn][1] * (value - m_parametersReplayCell[iTurn][0]) / 99.;
}

int OpeningStrategy::chooseACellByExploring(int round, const std::vector<double> &exploringProbabilities,
                                            const std::vector<std::vector<Cell>> &bestCells,
                                            const std::vector<Cell> &cellsPlayed)
{
    // If the cell has already been played during the round or if it is one of the best cells, then the probability of
    // exploring it is set to 0.
    std::vector<double> exProba{exploringProbabilities};
    if (round > 0)
    {
        for (auto &cell : bestCells[round - 1])
        {
            exProba[cell.index] = 0;
        }
    }
    for (auto &cell : cellsPlayed)
    {
        exProba[cell.index] = 0;
    }
    return myRandom::randIndexWeighted(exProba);
}

std::vector<std::vector<double>> OpeningStrategy::to2d(const std::vector<double> &vector1d, const int nRows)
{
    if (vector1d.size() % nRows != 0)
    {
        throw std::runtime_error("AgentOpeningStrategy::to_2d: The vector size is not a multiple of the number of rows.");
    }

    std::vector<std::vector<double>> vector2d(nRows);
    for (int iRow{0}; iRow < nRows; ++iRow)
    {
        vector2d[iRow] = std::vector<double>(vector1d.begin() + iRow * 2, vector1d.begin() + (iRow + 1) * 2);
    }
    return vector2d;
}

std::vector<double> OpeningStrategy::getParameters() const
{
    std::vector<double> parameters{m_parametersExploration};
    for (const auto &row : m_parametersReplayCell)
    {
        parameters.insert(parameters.end(), row.begin(), row.end());
    }
    return parameters;
}
