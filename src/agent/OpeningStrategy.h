#ifndef OPENING_STRATEGY_H
#define OPENING_STRATEGY_H

#include <vector>

#include "agent/Cell.h"

class OpeningStrategy
{
public:
    OpeningStrategy(const std::vector<double> &parametersExploration,
                    const std::vector<std::vector<double>> &parametersReplayCell);

    OpeningStrategy(const std::vector<double> &parametersOpenings);

    /**
     * @brief Choose a cell by replaying the best cell played during a previous round or by exploring.
     *
     * @param round The current round of the game.
     * @param colors The percentage of ratings in each cell.
     * @param bestCells The best cells played, indexed by round.
     * @param cellsPlayed The cells already played during the current round.
     * @return The index of the cell chosen.
     */
    int chooseCell(int round, const std::vector<double> &colors, const std::vector<std::vector<Cell>> &bestCells,
                   const std::vector<Cell> &cellsPlayed);

    std::vector<double> getParameters() const;

private:
    std::vector<double> m_parametersExploration;
    std::vector<std::vector<double>> m_parametersReplayCell;
    std::vector<double> m_exploringProbabilities;
    int m_round;

    /**
     * @brief Compute the probability of exploring each cell.
     *
     * If there is no rating in the map, then all cells have the same probability of being explored.
     *
     * @param colors The percentage of ratings in each cell.
     */
    void updateExploringProbabilities(const std::vector<double> &colors);

    /**
     * @brief Check if the cell should be replayed.
     *
     * @param value The value of the cell.
     * @param iTurn The turn of the round.
     * @return `true` if the cell should be replayed, `false` otherwise.
     */
    bool shouldReplayCell(int value, int iTurn) const;

    /**
     * @brief Choose a cell by exploring.
     *
     * Cells that have already been played during the round or that are among the best cells have their
     * exploration probability set to 0.
     *
     * @param round The current round of the game.
     * @param exploringProbabilities The probability of exploring each cell.
     * @param bestCells The best cells played, indexed by round.
     * @param cellsPlayed The cells already played during the current round.
     * @return The index of the cell chosen by exploring.
     */
    static int chooseACellByExploring(int round, const std::vector<double> &exploringProbabilities,
                                      const std::vector<std::vector<Cell>> &bestCells, const std::vector<Cell> &cellsPlayed);

    /**
     * @brief Convert a 1D vector to a 2D vector.
     *
     * @param vector1d The input 1D vector. Its size must be a multiple of `nRows`.
     * @param nRows The number of rows of the 2D vector.
     * @return The resulting 2D vector.
     */
    static std::vector<std::vector<double>> to2d(const std::vector<double> &vector1d, const int nRows);
};

#endif
