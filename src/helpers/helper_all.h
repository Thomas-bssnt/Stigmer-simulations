#ifndef HELPER_ALL_H
#define HELPER_ALL_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "game/Game.h"

/**
 * @brief Read space-separated numeric parameters from the last non-empty line of a file.
 *
 * @param filePath Path to the file to read.
 * @return The parameters parsed from the last line of the file.
 */
std::vector<double> readParameters(const std::string &filePath);

/**
 * @brief Create a set of agents assigned to the given game with randomly drawn profiles.
 *
 * The profile of each agent is drawn from `{"col", "neu", "def"}` according to `fractions`, and its
 * rating strategy is taken from the JSON entry named after the drawn profile.
 *
 * @param numberOfAgents Number of agents to create.
 * @param fractions Sampling weights for the three profiles, in the order col/neu/def.
 * @param game The game the agents will be assigned to.
 * @param parametersVisits Parameters of the opening strategy shared by all agents.
 * @param parametersStars JSON object containing one rating-strategy entry per profile.
 * @return The vector of initialized agents.
 */
std::vector<Agent> initializePlayers(int numberOfAgents,
                                     const std::vector<double> &fractions,
                                     Game &game,
                                     const std::vector<double> &parametersVisits,
                                     const nlohmann::json &parametersStars);

#endif
