#ifndef AGENT_H
#define AGENT_H

#include <vector>

#include "agent/Cell.h"
#include "agent/OpeningStrategy.h"
#include "agent/RatingStrategy.h"
#include "game/Game.h"

class GameAnalyzer;

/**
 * @brief An autonomous player of a `Game`.
 *
 * An agent combines an `OpeningStrategy`, which decides which cell to open, with a `RatingStrategy`,
 * which decides how many stars to spend on the opened cell. Once assigned to a game, the agent plays
 * one round at a time via `playARound()` and keeps track of the best cells discovered so far.
 */
class Agent
{
    friend class GameAnalyzer;

public:
    /**
     * @brief Build an agent that immediately joins a game.
     *
     * @param pGame Pointer to the game the agent will play. Must not be null.
     * @param openingStrategy Strategy used to choose which cell to open.
     * @param ratingStrategy Strategy used to choose how many stars to give.
     */
    Agent(Game *pGame,
          const OpeningStrategy &openingStrategy,
          const RatingStrategy &ratingStrategy);

    /**
     * @brief Build an agent that is not yet assigned to a game.
     *
     * The agent must be attached to a game with `assignToGame` before it can play.
     *
     * @param openingStrategy Strategy used to choose which cell to open.
     * @param ratingStrategy Strategy used to choose how many stars to give.
     */
    Agent(const OpeningStrategy &openingStrategy,
          const RatingStrategy &ratingStrategy);

    /**
     * @brief Assign the agent to a game and reset its per-game state.
     *
     * @param pGame Pointer to the game the agent will play. Must not be null.
     */
    void assignToGame(Game *pGame);

    /**
     * @brief Play a full round: open a cell and rate it, once per turn, then update the best cells.
     */
    void playARound();

    /**
     * @brief Get the agent type (defined by the rating strategy).
     *
     * @return A reference to the agent's type.
     */
    const AgentType &getAgentType() const;

    /**
     * @brief Get the parameters of the opening strategy.
     *
     * @return A flat vector of the opening-strategy parameters.
     */
    std::vector<double> getParametersOpeningStrategy() const;

    /**
     * @brief Get the parameters of the rating strategy.
     *
     * @return A flat vector of the rating-strategy parameters.
     */
    std::vector<double> getParametersRatingStrategy() const;

private:
    /**
     * @brief Update the top-`m_numberOfTurns` best cells opened during the current round.
     *
     * @param cellsPlayed The cells opened during the round.
     */
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
