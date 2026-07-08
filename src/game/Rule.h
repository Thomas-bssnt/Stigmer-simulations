#ifndef GAME_RULE_H
#define GAME_RULE_H

#include <vector>

/**
 * @brief Identifier for the scoring rule used by a game.
 *
 * Each rule defines both the score computation and the maximum number of stars a player may spend
 * during a round.
 */
enum class RuleNumber
{
    /** @brief No score; players may spend up to 15 stars per round. */
    Rule1,
    /** @brief Score is the sum of opened cell values; players may spend up to 15 stars per round. */
    Rule2,
    /** @brief Score is the sum of `value * stars` per opened cell; players may spend up to 8 stars per round. */
    Rule3,
    /** @brief Same as Rule3 with a bonus of 50 points per unspent star; up to 8 stars per round. */
    Rule4,
};

/**
 * @brief Encapsulates the scoring rule of a game.
 */
class Rule
{
public:
    /**
     * @brief Build a rule from its identifier.
     *
     * @param ruleNumber The rule identifier.
     */
    Rule(RuleNumber ruleNumber);

    /**
     * @brief Compute the score contributed by a single round.
     *
     * @param values The values of the cells opened during the round, in order of opening.
     * @param stars The number of stars given to each opened cell, in the same order.
     * @return The score for the round according to the active rule.
     */
    [[nodiscard]] int calculateScore(const std::vector<int> &values, const std::vector<int> &stars) const;

    /**
     * @brief Check if a rating is within the allowed range for this rule.
     *
     * @param rating The rating to check.
     * @return `true` if the rating is in [`getMinRating()`, `getMaxRating()`].
     */
    [[nodiscard]] bool isValidRating(int rating) const;

    /**
     * @brief Get the minimum rating a player may give to a single cell.
     *
     * @return The minimum allowed rating.
     */
    [[nodiscard]] int getMinRating() const;

    /**
     * @brief Get the maximum rating a player may give to a single cell.
     *
     * @return The maximum allowed rating.
     */
    [[nodiscard]] int getMaxRating() const;

    /**
     * @brief Get the maximum total number of stars a player may spend during a round.
     *
     * @return The maximum rating budget per round.
     */
    [[nodiscard]] int getMaxRatingPerRound() const;

private:
    /**
     * @brief Return the maximum rating per round associated with a rule identifier.
     *
     * @param ruleNumber The rule identifier.
     * @return The maximum rating budget per round.
     */
    static int maxRatingPerRoundFor(RuleNumber ruleNumber);

    const RuleNumber m_ruleNumber;
    const int m_minRating;
    const int m_maxRating;
    const int m_maxRatingPerRound;
};

#endif
