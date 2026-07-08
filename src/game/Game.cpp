#include <algorithm> // std::all_of, std::any_of, std::fill, std::transform
#include <cmath>     // std::isnan
#include <fstream>   // std::ofstream
#include <numeric>   // std::accumulate
#include <vector>    // std::vector

#include "game/Game.h"
#include "game/Map.h"
#include "game/Rule.h"

namespace
{
    // TODO: Change this legend parce que ce n'est pas vraiment la norme L1
    /**
     * @brief Normalize a vector by dividing each element by the sum of its elements.
     *
     * @note All elements of the vector must be non-negative.
     *
     * @param vector The vector to normalize.
     * @return The normalized vector. If the sum of the input is 0, a vector of zeros of the same size is
     *         returned.
     */
    template <typename T>
    std::vector<double> normalize(const std::vector<T> &vector)
    {
        const double sum{std::accumulate(vector.begin(), vector.end(), 0.)};

        if (sum == 0.)
        {
            return std::vector<double>(vector.size(), 0.);
        }

        std::vector<double> normalizedVector(vector.size());
        std::transform(vector.begin(), vector.end(), normalizedVector.begin(),
                       [sum](const T &value)
                       { return static_cast<double>(value) / sum; });
        return normalizedVector;
    }
} // namespace

Game::Game(int numberOfRounds,
           int numberOfTurns,
           int numberOfPlayers,
           const Rule rule,
           const Map map,
           double tauEvaporation)
    : //
      m_numberOfRounds{numberOfRounds},
      m_numberOfTurns{numberOfTurns},
      m_numberOfPlayers{numberOfPlayers},
      m_rule{rule},
      m_map{map},
      m_tauEvaporation{tauEvaporation},
      //
      m_playerCount{0},
      //
      m_iRound{0},
      m_rMap(m_map.getNumberOfCells(), 0.),
      m_colors(m_map.getNumberOfCells(), 0.),
      m_scores(m_map.getNumberOfCells(), 0),
      m_iCellOpened{std::vector<std::vector<std::vector<int>>>(
          m_numberOfPlayers, std::vector<std::vector<int>>(m_numberOfRounds, std::vector<int>(numberOfTurns, 0)))},
      m_vCellOpened{std::vector<std::vector<std::vector<int>>>(
          m_numberOfPlayers, std::vector<std::vector<int>>(m_numberOfRounds, std::vector<int>(numberOfTurns, 0)))},
      m_rCellOpened{std::vector<std::vector<std::vector<int>>>(
          m_numberOfPlayers, std::vector<std::vector<int>>(m_numberOfRounds, std::vector<int>(numberOfTurns, 0)))},
      m_iTurn{std::vector<int>(numberOfPlayers, 0)},
      m_hasOpenedACell{std::vector<bool>(numberOfPlayers, false)},
      m_numberOfRatingsRemaining{std::vector<int>(numberOfPlayers, m_rule.getMaxRatingPerRound())}
{
}

Game::Game(int numberOfRounds, int numberOfPlayers, const Rule &rule, double tauEvaporation)
    : Game(numberOfRounds, 3, numberOfPlayers, rule, Map(225, false), tauEvaporation)
{
}

Game::Game(int numberOfRounds, int numberOfPlayers, const Rule &rule)
    : Game(numberOfRounds, 3, numberOfPlayers, rule, Map(225, false), std::nan(""))
{
}

Game::Game(int numberOfRounds, int numberOfPlayers)
    : Game(numberOfRounds, 3, numberOfPlayers, Rule(RuleNumber::Rule2), Map(225, false), std::nan(""))
{
}

int Game::openCell(int iPlayer, int iCell)
{
    if (m_iRound >= m_numberOfRounds)
    {
        throw GameException("Game::openCell: The game is over.");
    }
    if (m_iTurn[iPlayer] >= m_numberOfTurns)
    {
        throw GameException("Game::openCell: Player " + std::to_string(iPlayer) + " already played " +
                            std::to_string(m_numberOfTurns) + " times during the round.");
    }
    if (m_hasOpenedACell[iPlayer])
    {
        throw GameException("Game::openCell: Player " + std::to_string(iPlayer) +
                            " already opened a cell and must rate it before opening another cell.");
    }
    if (!m_map.isValidIndex(iCell))
    {
        throw GameException("Game::openCell: The cell number " + std::to_string(iCell) + " does not exist.");
    }
    if (hasThePlayerOpenedTheCellDuringTheRound(iPlayer, iCell))
    {
        throw GameException("Game::openCell: The player " + std::to_string(iPlayer) + " already opened the cell " +
                            std::to_string(iCell) + " during the round.");
    }

    m_hasOpenedACell[iPlayer] = true;

    const int vCell{m_map.getValue(iCell)};

    m_iCellOpened[iPlayer][m_iRound][m_iTurn[iPlayer]] = iCell;
    m_vCellOpened[iPlayer][m_iRound][m_iTurn[iPlayer]] = vCell;
    return vCell;
}

void Game::rateCell(int iPlayer, int rating)
{
    if (m_iRound >= m_numberOfRounds)
    {
        throw GameException("Game::rateCell: The game is over.");
    }
    if (m_iTurn[iPlayer] >= m_numberOfTurns)
    {
        throw GameException("Game::rateCell: Player " + std::to_string(iPlayer) + " already played " +
                            std::to_string(m_numberOfTurns) + " times during the round.");
    }
    if (!m_hasOpenedACell[iPlayer])
    {
        throw GameException("Game::rateCell: Player " + std::to_string(iPlayer) +
                            " must open a cell before rating it.");
    }
    if (!m_rule.isValidRating(rating))
    {
        throw GameException("Game::rateCell: Player " + std::to_string(iPlayer) +
                            " entered an invalid rating (" + std::to_string(rating) + " not in [" +
                            std::to_string(m_rule.getMinRating()) + ", " +
                            std::to_string(m_rule.getMaxRating()) + "]).");
    }
    if (rating > m_numberOfRatingsRemaining[iPlayer])
    {
        throw GameException("Game::rateCell: Player " + std::to_string(iPlayer) + " only has " +
                            std::to_string(m_numberOfRatingsRemaining[iPlayer]) + " ratings remaining.");
    }

    m_rCellOpened[iPlayer][m_iRound][m_iTurn[iPlayer]] = rating;

    m_numberOfRatingsRemaining[iPlayer] -= rating;
    m_hasOpenedACell[iPlayer] = false;
    ++m_iTurn[iPlayer];

    if (haveAllPlayersPlayedTheirTurns())
    {
        changeRound();
    }
}

void Game::changeRound()
{
    updateColors();
    updateScores();

    std::fill(m_iTurn.begin(), m_iTurn.end(), 0);
    std::fill(m_numberOfRatingsRemaining.begin(), m_numberOfRatingsRemaining.end(), m_rule.getMaxRatingPerRound());

    ++m_iRound;
}

void Game::updateColors()
{
    if (!std::isnan(m_tauEvaporation))
    {
        const double evaporationFactor{1. - 1. / m_tauEvaporation};
        for (double &value : m_rMap)
        {
            value *= evaporationFactor;
        }
    }

    for (int iPlayer{0}; iPlayer < m_numberOfPlayers; ++iPlayer)
    {
        for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
        {
            const int iCell{m_iCellOpened[iPlayer][m_iRound][iTurn]};
            const int rating{m_rCellOpened[iPlayer][m_iRound][iTurn]};
            m_rMap[iCell] += rating;
        }
    }
    m_colors = normalize(m_rMap);
}

void Game::updateScores()
{
    for (int iPlayer{0}; iPlayer < m_numberOfPlayers; ++iPlayer)
    {
        m_scores[iPlayer] += m_rule.calculateScore(m_vCellOpened[iPlayer][m_iRound], m_rCellOpened[iPlayer][m_iRound]);
    }
}

bool Game::hasThePlayerOpenedTheCellDuringTheRound(int iPlayer, int iCell) const
{
    return std::any_of(m_iCellOpened[iPlayer][m_iRound].begin(),
                       m_iCellOpened[iPlayer][m_iRound].begin() + m_iTurn[iPlayer],
                       [iCell](int iCellOpened)
                       { return iCellOpened == iCell; });
}

bool Game::haveAllPlayersPlayedTheirTurns() const
{
    return std::all_of(m_iTurn.begin(),
                       m_iTurn.end(),
                       [&numberOfTurns = m_numberOfTurns](int iTurn)
                       { return iTurn == numberOfTurns; });
}

Game *Game::getAddress()
{
    return this;
}

int Game::registerPlayer()
{
    return m_playerCount++;
}

const std::vector<double> &Game::getColors() const
{
    return m_colors;
}

int Game::getScoreOfPlayer(int iPlayer) const
{
    return m_scores[iPlayer];
}

int Game::getCurrentRound() const
{
    return m_iRound;
}

int Game::getNumberOfRounds() const
{
    return m_numberOfRounds;
}

int Game::getNumberOfTurns() const
{
    return m_numberOfTurns;
}

int Game::getNumberOfCells() const
{
    return m_map.getNumberOfCells();
}
