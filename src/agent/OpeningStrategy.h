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

    int choseCell(int round, const std::vector<double> &colors, const std::vector<std::vector<Cell>> &bestCells,
                  const std::vector<Cell> &cellsPlayed);

    std::vector<double> getParameters() const;

private:
    std::vector<double> m_parametersExploration;
    std::vector<std::vector<double>> m_parametersReplayCell;
    std::vector<double> m_exploringProbabilities;
    int m_round;

    void updateExploringProbabilities(const std::vector<double> &colors);

    bool shouldReplayCell(int value, int iTurn) const;

    static int chooseACellByExploring(int round, const std::vector<double> &exploringProbabilities,
                                      const std::vector<std::vector<Cell>> &bestCells, const std::vector<Cell> &cellsPlayed);

    static std::vector<std::vector<double>> to2d(const std::vector<double> &vector1d, const int nRows);
};

#endif
