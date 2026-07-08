#ifndef GAME_H
#define GAME_H

#include <stdexcept>
#include <string>
#include <vector>

#include "game/Map.h"
#include "game/Rule.h"

class GameAnalyzer;

class Game
{
    friend class GameAnalyzer;

public:
    Game(int numberOfRounds, int numberOfTurns, int numberOfPlayers, const Rule rule, const Map map,
         double tauEvaporation);

    Game(int numberOfRounds, int numberOfPlayers, const Rule &rule, double tauEvaporation);

    Game(int numberOfRounds, int numberOfPlayers, const Rule &rule);

    Game(int numberOfRounds, int numberOfPlayers);

    /**
     * @brief Open a cell for a given player.
     *
     * @param playerId The identifier of the player.
     * @param iCell The index of the cell to open.
     * @return The value of the cell.
     */
    int openCell(int playerId, int iCell);

    /**
     * @brief Rate the cell that was just opened by the given player.
     *
     * @param playerId The identifier of the player.
     * @param numberOfRatings The number of stars to assign to the cell.
     */
    void rateCell(int playerId, int numberOfRatings);

    Game *getAddress();

    /**
     * @brief Assign a unique identifier to a player.
     *
     * This method should be called once per player.
     *
     * @return The assigned player identifier.
     */
    int registerPlayer();

    [[nodiscard]] const std::vector<double> &getColors() const;

    [[nodiscard]] int getScoreOfPlayer(int playerId) const;

    [[nodiscard]] int getCurrentRound() const;

    [[nodiscard]] int getNumberOfRounds() const;

    [[nodiscard]] int getNumberOfTurns() const;

    [[nodiscard]] int getNumberOfCells() const;

private:
    /**
     * @brief Advance to the next round and update the distributions and scores.
     */
    void changeRound();

    /**
     * @brief Update the color map used by players.
     */
    void updateColors();

    /**
     * @brief Update the scores of the players.
     */
    void updateScores();

    /**
     * @brief Check if a player has already opened a given cell during the current round.
     *
     * @param playerId The identifier of the player.
     * @param iCell The index of the cell.
     * @return `true` if the player has already opened the cell during the round.
     */
    bool hasThePlayerOpenedTheCellDuringTheRound(int playerId, int iCell) const;

    /**
     * @brief Check if all players have played their turns for the current round.
     *
     * @return `true` if all players have played their turns.
     */
    bool haveAllPlayersPlayedTheirTurns() const;

    // Constant variables
    const int m_numberOfRounds;
    const int m_numberOfTurns;
    const int m_numberOfPlayers;
    const Rule m_rule;
    const Map m_map;
    const double m_tauEvaporation;
    // Player count
    int m_playerCount;
    // Variables updated at each round
    int m_iRound;
    std::vector<double> m_rMap;
    std::vector<double> m_colors;
    std::vector<int> m_scores;
    std::vector<std::vector<std::vector<int>>> m_iCellOpened;
    std::vector<std::vector<std::vector<int>>> m_vCellOpened;
    std::vector<std::vector<std::vector<int>>> m_rCellOpened;
    std::vector<int> m_iTurn;
    std::vector<bool> m_hasOpenedACell;
    std::vector<int> m_numberOfRatingsRemaining;
};

class GameException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

#endif
