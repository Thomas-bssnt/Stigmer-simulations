#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>
#include <vector>

#include "agent/Agent.h"
#include "agent/Cell.h"
#include "agent/OpeningStrategy.h"
#include "agent/RatingStrategy.h"
#include "game/Game.h"
#include "random/myRandom.h"

Agent::Agent(Game *pGame,
             const OpeningStrategy &openingStrategy,
             const RatingStrategy &ratingStrategy)
    : mp_Game{pGame},
      m_iAgent{mp_Game->getPlayerId()},
      m_numberOfTurns{mp_Game->getNumberOfTurns()},
      m_numberOfCells{mp_Game->getNumberOfCells()},
      m_openingStrategy{openingStrategy},
      m_ratingStrategy{ratingStrategy},
      m_round{0},
      m_bestCells{std::vector<std::vector<Cell>>(mp_Game->getNumberOfRounds(), std::vector<Cell>(m_numberOfTurns, {-1, -1}))}
{
}

Agent::Agent(const OpeningStrategy &openingStrategy,
             const RatingStrategy &ratingStrategy)
    : mp_Game{nullptr},
      m_openingStrategy{openingStrategy},
      m_ratingStrategy{ratingStrategy}
{
}

void Agent::assignToGame(Game *pGame)
{
    mp_Game = pGame;
    m_iAgent = mp_Game->getPlayerId();
    m_numberOfTurns = mp_Game->getNumberOfTurns();
    m_numberOfCells = mp_Game->getNumberOfCells();
    m_round = 0;
    m_bestCells = std::vector<std::vector<Cell>>(mp_Game->getNumberOfRounds(), std::vector<Cell>(m_numberOfTurns, {-1, -1}));
}

void Agent::playARound()
{
    m_round = mp_Game->getCurrentRound();
    std::vector<Cell> cellsPlayed;
    for (int iTurn{0}; iTurn < m_numberOfTurns; ++iTurn)
    {
        const int iCell{m_openingStrategy.choseCell(m_round, mp_Game->getColors(), m_bestCells, cellsPlayed)};
        const int vCell{mp_Game->openCell(m_iAgent, iCell)};
        mp_Game->rateCell(m_iAgent, m_ratingStrategy.choseRating(vCell));
        cellsPlayed.push_back({iCell, vCell});
    }
    updateBestCells(cellsPlayed);
}

void Agent::updateBestCells(const std::vector<Cell> &cellsPlayed)
{
    std::vector<Cell> bestCells(m_numberOfTurns, {-1, -1});
    for (const auto &cellPlayed : cellsPlayed)
    {
        for (int i{0}; i < bestCells.size(); ++i)
        {
            if (cellPlayed.value > bestCells[i].value)
            {
                bestCells.insert(bestCells.begin() + i, cellPlayed);
                bestCells.pop_back();
                break;
            }
        }
    }

    m_bestCells[m_round] = bestCells;
}

const AgentType &Agent::getAgentType() const
{
    return m_ratingStrategy.getAgentType();
}
std::vector<double> Agent::getParametersOpeningStrategy() const
{
    return m_openingStrategy.getParameters();
}

std::vector<double> Agent::getParametersRatingStrategy() const
{
    return m_ratingStrategy.getParameters();
}
