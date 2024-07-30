#ifndef RATING_STRATEGY_H
#define RATING_STRATEGY_H

#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Cell.h"

enum class AgentType
{
    UNDEFINED,
    collaborator,
    neutral,
    defector,
    optimized,
    bot,
};

class RatingStrategy
{
public:
    RatingStrategy(int minRating, int maxRating, const nlohmann::json &parameters);

    RatingStrategy(const nlohmann::json &parameters);

    RatingStrategy(const std::vector<double> &parameters, const std::string &functionType);

    int choseRating(int value) const;

    const AgentType &getAgentType() const;

    std::vector<double> getParameters() const;

private:
    nlohmann::json m_parameters;
    std::vector<int> m_ratings;
    AgentType m_agentType;

    std::vector<double> computeProbabilities(int value) const;

    void computeProbabilitiesMNSLinear(std::vector<double> &probabilities,
                                       const std::vector<double> &parameters, int value) const;

    static double functionTanh(const std::vector<double> &parameters, int value);

    static double functionBotTanh(const std::vector<double> &parameters, int value);

    static double functionConstant(const std::vector<double> &parameters, int value);

    static double functionLinear(const std::vector<double> &parameters, int value);

    static double functionGaussian(const std::vector<double> &parameters, int value);

    static nlohmann::json to_json(const std::vector<double> &parametersStars, const std::string &functionType);
};

#endif
