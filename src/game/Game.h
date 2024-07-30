#ifndef GAME_H
#define GAME_H

#include <exception>
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

    int openCell(int playerId, int iCell);

    void rateCell(int playerId, int numberOfRatings);

    Game *getAddress();

    int getPlayerId();

    const std::vector<double> &getColors() const;

    int getScoreOfPlayer(int playerId) const;

    int getCurrentRound() const;

    int getNumberOfRounds() const;

    int getNumberOfTurns() const;

    int getNumberOfCells() const;

private:
    void changeRound();

    void updateColors();

    void updateScores();

    bool hasThePlayerOpenedTheCellDuringTheRound(int playerId, int iCell) const;

    bool haveAllPlayersPlayedTheirTurns() const;

    template <typename T>
    static std::vector<double> normalize(const std::vector<T> &vector);

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

class GameException : public std::exception
{
public:
    GameException(const std::string &error) : m_error{error} {}

    const char *what() const noexcept override { return m_error.c_str(); }

private:
    std::string m_error;
};

#endif
