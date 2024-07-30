#include <cmath>     // std::exp, std::pow, std::tanh
#include <numeric>   // std::iota
#include <stdexcept> // std::invalid_argument
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "agent/RatingStrategy.h"
#include "random/myRandom.h"

RatingStrategy::RatingStrategy(int minRating, int maxRating, const nlohmann::json &parameters)
    : m_ratings{std::vector<int>(maxRating - minRating + 1)},
      m_parameters{parameters},
      m_agentType{AgentType::UNDEFINED}
{
    std::iota(m_ratings.begin(), m_ratings.end(), minRating);
}

RatingStrategy::RatingStrategy(const nlohmann::json &parameters)
    : RatingStrategy(0, 5, parameters)
{
}

RatingStrategy::RatingStrategy(const std::vector<double> &parameters, const std::string &functionType)
    : RatingStrategy(0, 5, to_json(parameters, functionType))
{
}

int RatingStrategy::choseRating(int value) const
{
    return myRandom::choice(m_ratings, computeProbabilities(value));
}

std::vector<double> RatingStrategy::computeProbabilities(int value) const
{
    const std::string functionType{m_parameters["functionType"].get<std::string>()};
    std::vector<double> probabilities(6, 0.);
    if (functionType == "tanh")
    {
        probabilities[0] = functionTanh(m_parameters["p0"], value);
        probabilities[5] = functionTanh(m_parameters["p5"], value);
        const double p1234{(1. - probabilities[0] - probabilities[5]) / 4.};
        probabilities[1] = p1234;
        probabilities[2] = p1234;
        probabilities[3] = p1234;
        probabilities[4] = p1234;
    }
    else if (functionType == "constant")
    {
        probabilities[0] = functionConstant(m_parameters["p0"], value);
        probabilities[5] = functionConstant(m_parameters["p5"], value);
        const double p1234{(1. - probabilities[0] - probabilities[5]) / 4.};
        probabilities[1] = p1234;
        probabilities[2] = p1234;
        probabilities[3] = p1234;
        probabilities[4] = p1234;
    }
    else if (functionType == "linear")
    {
        probabilities[0] = functionLinear(m_parameters["p0"], value);
        probabilities[5] = functionLinear(m_parameters["p5"], value);
        const double p1234{(1. - probabilities[0] - probabilities[5]) / 4.};
        probabilities[1] = p1234;
        probabilities[2] = p1234;
        probabilities[3] = p1234;
        probabilities[4] = p1234;
    }
    else if (functionType == "gaussian")
    {
        probabilities[1] = functionGaussian(m_parameters["p1"], value);
        probabilities[2] = functionGaussian(m_parameters["p2"], value);
        probabilities[3] = functionGaussian(m_parameters["p3"], value);
        probabilities[4] = functionGaussian(m_parameters["p4"], value);
        probabilities[5] = functionGaussian(m_parameters["p5"], value);
        probabilities[0] = 1 - probabilities[1] - probabilities[2] - probabilities[3] - probabilities[4] - probabilities[5];
    }
    else if (functionType == "mns_linear")
    {
        computeProbabilitiesMNSLinear(probabilities, m_parameters["mns"], value);
    }
    else if (functionType == "bot_const")
    {
        for (int i{0}; i < probabilities.size(); ++i)
        {
            probabilities[i] = m_parameters["probas"][i];
        }
    }
    else if (functionType == "bot_tanh")
    {
        probabilities[0] = functionBotTanh(m_parameters["p0"], value);
        probabilities[5] = functionBotTanh(m_parameters["p5"], value);
        const double p1234{(1. - probabilities[0] - probabilities[5]) / 4.};
        probabilities[1] = p1234;
        probabilities[2] = p1234;
        probabilities[3] = p1234;
        probabilities[4] = p1234;
    }
    else
    {
        throw std::invalid_argument("The function type " + functionType + " does not exists.");
    }

    return probabilities;
}

void RatingStrategy::computeProbabilitiesMNSLinear(std::vector<double> &probabilities,
                                                   const std::vector<double> &parameters, int value) const
{
    const double mns{parameters[0] + 5. * parameters[1] * value / 99.};
    if (mns <= 0.)
    {
        probabilities[0] = 1.;
    }
    else if (mns >= 5.)
    {
        probabilities[5] = 1.;
    }
    else
    {
        const int mns_int{static_cast<int>(mns)};
        probabilities[mns_int] = 1 - (mns - mns_int);
        probabilities[mns_int + 1] = mns - mns_int;
    }
}

double RatingStrategy::functionTanh(const std::vector<double> &parameters, int value)
{
    return parameters[0] + parameters[1] * std::tanh((value - parameters[2]) / 99. * parameters[3]);
}

double RatingStrategy::functionBotTanh(const std::vector<double> &parameters, int value)
{
    return functionTanh(std::vector{0.5, 0.5, parameters[0], parameters[1]}, value);
    // return (1 + std::tanh((value - parameters[0]) / 99. * parameters[1])) / 2;
}

double RatingStrategy::functionConstant(const std::vector<double> &parameters, int value)
{
    return parameters[0];
}

double RatingStrategy::functionLinear(const std::vector<double> &parameters, int value)
{
    double result = parameters[0] + parameters[1] * value / 99.;
    result = std::max(0.0, std::min(result, 1.0)); // Clip the result to the range [0, 1]
    return result;
}

double RatingStrategy::functionGaussian(const std::vector<double> &parameters, int value)
{
    double result = parameters[0] * std::exp(-std::pow((value - parameters[1]) / 99. * parameters[2], 2));
    result = std::max(0.0, std::min(result, 1.0)); // Clip the result to the range [0, 1]
    return result;
}

nlohmann::json RatingStrategy::to_json(const std::vector<double> &parameters, const std::string &functionType)
{
    nlohmann::json j;
    j["functionType"] = functionType;
    if (functionType == "tanh")
    {
        j["p0"] = {parameters[0], parameters[1], parameters[2], parameters[3]};
        j["p5"] = {parameters[4], parameters[5], parameters[6], parameters[7]};
    }
    else if (functionType == "gaussian")
    {
        j["p1"] = {parameters[0], parameters[1], parameters[2]};
        j["p2"] = {parameters[3], parameters[4], parameters[5]};
        j["p3"] = {parameters[7], parameters[8], parameters[9]};
        j["p4"] = {parameters[11], parameters[12], parameters[13]};
        j["p5"] = {parameters[14], parameters[15], parameters[16]};
    }
    else if (functionType == "mns_linear")
    {
        j["mns"] = {parameters[0], parameters[1]};
    }
    else
    {
        throw std::invalid_argument("The function type " + functionType + "does not exists.");
    }
    return j;
}

const AgentType &RatingStrategy::getAgentType() const
{
    return m_agentType;
}

std::vector<double> RatingStrategy::getParameters() const
{
    const std::string functionType{m_parameters["functionType"].get<std::string>()};
    if (functionType != "mns_linear")
    {
        throw std::invalid_argument("The function type " + functionType + "does not exists.");
    }

    return m_parameters["mns"];
}
