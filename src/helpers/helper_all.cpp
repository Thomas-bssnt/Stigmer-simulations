#include <fstream>   // std::ifstream
#include <stdexcept> // std::runtime_error
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Agent.h"
#include "game/Game.h"
#include "helpers/helper_all.h"
#include "random/myRandom.h"

std::vector<double> readParameters(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        throw std::runtime_error("The file " + filePath + " could not be opened.");
    }

    int iLine{-1};
    std::string lastLine;
    std::string nextLine;
    do
    {
        ++iLine;
        lastLine = nextLine;
    } while (std::getline(file, nextLine));

    if (iLine == 0)
    {
        throw std::runtime_error("The file " + filePath + " is empty.");
    }

    std::vector<double> parameters;
    int iNumber{0};
    std::string number;
    for (const auto x : lastLine + " ")
    {
        if (x == ' ')
        {
            parameters.push_back(std::stod(number));
            number = "";
            ++iNumber;
        }
        else
        {
            number += x;
        }
    }
    return parameters;
}

std::vector<Agent> initializePlayers(int numberOfAgents,
                                     const std::vector<double> &fractions,
                                     Game &game,
                                     const std::vector<double> &parametersVisits,
                                     const nlohmann::json &parametersStars)
{
    const std::vector<std::string> profiles{"col", "neu", "def"};

    std::vector<Agent> agents;
    agents.reserve(numberOfAgents);
    for (int iAgent{0}; iAgent < numberOfAgents; ++iAgent)
    {
        const std::string profile{myRandom::choice(profiles, fractions)};
        agents.emplace_back(game.getAddress(), parametersVisits, parametersStars[profile]);
    }

    return agents;
}
