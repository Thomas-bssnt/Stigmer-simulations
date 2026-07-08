#ifndef RATING_STRATEGY_H
#define RATING_STRATEGY_H

#include <vector>

#include <nlohmann/json.hpp>

#include "agent/Cell.h"

/**
 * @brief Behavioural profile of an agent.
 */
enum class AgentType
{
    /** @brief Type not set. */
    UNDEFINED,
    /** @brief Cooperates with the group by rating cells informatively. */
    collaborator,
    /** @brief Rates cells without a clear cooperative or deceptive intent. */
    neutral,
    /** @brief Deliberately misrates cells to mislead the group. */
    defector,
    /** @brief Uses parameters fitted by optimization. */
    optimized,
    /** @brief Follows a scripted rating policy. */
    bot,
};

/**
 * @brief Decides how many stars to give to a cell given its value.
 *
 * The rating is drawn from a discrete distribution whose probabilities are computed by one of
 * several parametric functions (tanh, constant, linear, gaussian, mns_linear, bot_const, bot_tanh)
 * selected via the `functionType` entry of the parameters JSON.
 */
class RatingStrategy
{
public:
    /**
     * @brief Build a rating strategy from a JSON parameter object with an explicit rating range.
     *
     * @param minRating Minimum rating (inclusive).
     * @param maxRating Maximum rating (inclusive).
     * @param parameters JSON parameters describing the probability function to use.
     */
    RatingStrategy(int minRating, int maxRating, const nlohmann::json &parameters);

    /**
     * @brief Build a rating strategy with the default rating range [0, 5].
     *
     * @param parameters JSON parameters describing the probability function to use.
     */
    RatingStrategy(const nlohmann::json &parameters);

    /**
     * @brief Build a rating strategy from a flat parameter vector and a function-type name.
     *
     * @param parameters Flat vector of parameters interpreted according to `functionType`.
     * @param functionType Name of the probability function (e.g. "tanh", "gaussian", "mns_linear").
     */
    RatingStrategy(const std::vector<double> &parameters, const std::string &functionType);

    /**
     * @brief Pick a rating for a cell with the given value.
     *
     * @param value The value of the cell being rated.
     * @return The rating drawn from the probability distribution.
     */
    int choseRating(int value) const;

    /**
     * @brief Get the agent type associated with the strategy.
     *
     * @return A reference to the agent type.
     */
    const AgentType &getAgentType() const;

    /**
     * @brief Get the parameters of the strategy as a flat vector.
     *
     * @return The parameters of the underlying probability function.
     */
    std::vector<double> getParameters() const;

private:
    nlohmann::json m_parameters;
    std::vector<int> m_ratings;
    AgentType m_agentType;

    /**
     * @brief Compute the rating probabilities for a cell of the given value.
     *
     * @param value The value of the cell being rated.
     * @return The probability of each rating in `m_ratings`.
     */
    std::vector<double> computeProbabilities(int value) const;

    /**
     * @brief Fill `probabilities` for the "mns_linear" function.
     *
     * @param probabilities Output vector to be filled.
     * @param parameters Parameters of the mns_linear function.
     * @param value The value of the cell being rated.
     */
    void computeProbabilitiesMNSLinear(std::vector<double> &probabilities,
                                       const std::vector<double> &parameters, int value) const;

    /**
     * @brief `p0 + p1 * tanh((value - p2) / 99 * p3)`.
     *
     * @param parameters The four function parameters `[p0, p1, p2, p3]`.
     * @param value The cell value.
     * @return The resulting probability.
     */
    static double functionTanh(const std::vector<double> &parameters, int value);

    /**
     * @brief Bot-flavoured tanh: equivalent to `functionTanh({0.5, 0.5, p0, p1}, value)`.
     *
     * @param parameters The two function parameters `[p0, p1]`.
     * @param value The cell value.
     * @return The resulting probability.
     */
    static double functionBotTanh(const std::vector<double> &parameters, int value);

    /**
     * @brief Constant function: returns `parameters[0]`.
     *
     * @param parameters The single function parameter.
     * @param value Unused.
     * @return The constant probability.
     */
    static double functionConstant(const std::vector<double> &parameters, int value);

    /**
     * @brief `clip(p0 + p1 * value / 99, 0, 1)`.
     *
     * @param parameters The two function parameters `[p0, p1]`.
     * @param value The cell value.
     * @return The resulting probability, clamped to [0, 1].
     */
    static double functionLinear(const std::vector<double> &parameters, int value);

    /**
     * @brief `clip(p0 * exp(-((value - p1) / 99 * p2)^2), 0, 1)`.
     *
     * @param parameters The three function parameters `[p0, p1, p2]`.
     * @param value The cell value.
     * @return The resulting probability, clamped to [0, 1].
     */
    static double functionGaussian(const std::vector<double> &parameters, int value);

    /**
     * @brief Convert a flat parameter vector into the JSON form expected by the constructor.
     *
     * @param parametersStars Flat vector of parameters.
     * @param functionType Name of the probability function.
     * @return The JSON representation of the parameters.
     */
    static nlohmann::json to_json(const std::vector<double> &parametersStars, const std::string &functionType);
};

#endif
