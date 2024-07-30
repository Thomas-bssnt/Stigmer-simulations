#ifndef GAME_RULE_H
#define GAME_RULE_H

#include <vector>

enum class RuleNumber
{
    Rule1,
    Rule2,
    Rule3,
    Rule4,
};

class Rule
{
public:
    Rule(const RuleNumber &ruleNumber);

    int calculateScore(const std::vector<int> &values, const std::vector<int> &stars) const;

    bool isValidRating(int rating) const;

    int getMinRating() const;

    int getMaxRating() const;

    int getMaxRatingPerRound() const;

protected:
    const RuleNumber m_ruleNumber;
    int m_minRating;
    int m_maxRating;
    int m_maxRatingPerRound;
};

#endif
