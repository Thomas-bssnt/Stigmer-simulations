#include <algorithm> // std::all_of, std::any_of, std::transform
#include <cmath>     // std::isnan
#include <fstream>   // std::ofstream
#include <numeric>   // std::accumulate
#include <vector>    // std::vector

#include "game/Game.h"
#include "game/Map.h"
#include "game/Rule.h"

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
      m_rMap{std::vector<double>(m_map.getNumberOfCells(), 0.)},
      m_colors{std::vector<double>(m_map.getNumberOfCells(), 0.)},
      m_scores{std::vector<int>(m_map.getNumberOfCells(), 0)},
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

/*
 * Open a cell.
 * @param iPlayer the player number
 * @param iCell the cell number
 * @return the value of the cell
 */
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

/*
 * Rate a cell.
 * @param iPlayer the player number
 * @param rating the number of stars
 */
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

/*
 * Change the round and update the distributions and scores.
 */
void Game::changeRound()
{
    updateColors();
    updateScores();

    m_iTurn = std::vector<int>(m_numberOfPlayers, 0);
    m_numberOfRatingsRemaining = std::vector<int>(m_numberOfPlayers, m_rule.getMaxRatingPerRound());

    ++m_iRound;
}

/*
 * Update the color map used by players.
 */
void Game::updateColors()
{
    if (!std::isnan(m_tauEvaporation))
    {
        const double evaporationFactor{1. - 1. / m_tauEvaporation};
        for (int iCell{0}; iCell < m_rMap.size(); ++iCell)
        {
            m_rMap[iCell] *= evaporationFactor;
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

/*
 * Update the scores of the players.
 */
void Game::updateScores()
{
    for (int iPlayer{0}; iPlayer < m_numberOfPlayers; ++iPlayer)
    {
        m_scores[iPlayer] += m_rule.calculateScore(m_vCellOpened[iPlayer][m_iRound], m_rCellOpened[iPlayer][m_iRound]);
    }
}

/*
 * Check if a player has already opened a cell during the round.
 *
 * @param iPlayer the player number
 * @param iCell the cell number
 * @return true if the player has already opened the cell during the round.
 */
bool Game::hasThePlayerOpenedTheCellDuringTheRound(int iPlayer, int iCell) const
{
    return std::any_of(m_iCellOpened[iPlayer][m_iRound].begin(),
                       m_iCellOpened[iPlayer][m_iRound].begin() + m_iTurn[iPlayer],
                       [iCell](int iCellOpened)
                       { return iCellOpened == iCell; });
}

/*
 * Check if all players have played their turns.
 *
 * @return true if all players have played their turns.
 */
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

/*
 * Assigns a unique playerId to a player. This method should only be used once per player.
 *
 * @return the assigned playerId.
 */
int Game::getPlayerId()
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

// TODO: Change this legend parce que ce n'est pas vraiment la norme L1
/*
 * Calculate the L1 norm of a vector.
 *
 * @param vector the vector to normalize. All elements of the vector must be non-negative.
 * @return the L1 norm of the input vector. If the sum of the values in the vector is 0., then a vector full of 0.
 * is returned.
 */
template <typename T>
std::vector<double> Game::normalize(const std::vector<T> &vector)
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
