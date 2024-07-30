#ifndef AGENT_H
#define AGENT_H

#include <vector>

#include "agent/Cell.h"
#include "agent/OpeningStrategy.h"
#include "agent/RatingStrategy.h"
#include "game/Game.h"

class GameAnalyzer;

class Agent
{
    friend class GameAnalyzer;

public:
    Agent(Game *pGame,
          const OpeningStrategy &openingStrategy,
          const RatingStrategy &ratingStrategy);

    Agent(const OpeningStrategy &openingStrategy,
          const RatingStrategy &ratingStrategy);

    void assignToGame(Game *pGame);

    void playARound();

    const AgentType &getAgentType() const;

    std::vector<double> getParametersOpeningStrategy() const;

    std::vector<double> getParametersRatingStrategy() const;

private:
    void updateBestCells(const std::vector<Cell> &cellsPlayed);

    // Game variables
    Game *mp_Game;
    int m_iAgent;
    int m_numberOfTurns;
    int m_numberOfCells;
    // Strategy variables
    OpeningStrategy m_openingStrategy;
    RatingStrategy m_ratingStrategy;
    //
    int m_round;
    std::vector<std::vector<Cell>> m_bestCells;
};

#endif
