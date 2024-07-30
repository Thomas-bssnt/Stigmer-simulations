#ifndef HELPER_ALL_H
#define HELPER_ALL_H

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "game/Game.h"

std::vector<double> readParameters(const std::string &filePath);

std::vector<Agent> initializePlayers(int numberOfAgents,
                                     const std::vector<double> &fractions,
                                     Game &game,
                                     const std::vector<double> &parametersVisits,
                                     const nlohmann::json &parametersStars);

#endif
