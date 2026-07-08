#include <numeric>   // std::accumulate, std::inner_product
#include <stdexcept> // std::invalid_argument
#include <string>
#include <vector>

#include "game/Rule.h"

Rule::Rule(RuleNumber ruleNumber)
    : m_ruleNumber{ruleNumber},
      m_minRating{0},
      m_maxRating{5},
      m_maxRatingPerRound{maxRatingPerRoundFor(ruleNumber)}
{
}

int Rule::maxRatingPerRoundFor(RuleNumber ruleNumber)
{
    switch (ruleNumber)
    {
    case RuleNumber::Rule1:
    case RuleNumber::Rule2:
        return 15;
    case RuleNumber::Rule3:
    case RuleNumber::Rule4:
        return 8;
    }
    throw std::invalid_argument{"Invalid rule number"};
}

int Rule::calculateScore(const std::vector<int> &values, const std::vector<int> &stars) const
{
    switch (m_ruleNumber)
    {
    case RuleNumber::Rule1:
        return 0;

    case RuleNumber::Rule2:
        return std::accumulate(values.begin(), values.end(), 0);

    case RuleNumber::Rule3:
        return std::inner_product(values.begin(), values.end(), stars.begin(), 0);

    case RuleNumber::Rule4:
    {
        const int used = std::accumulate(stars.begin(), stars.end(), 0);
        const int leftover = m_maxRatingPerRound - used;
        return std::inner_product(values.begin(), values.end(), stars.begin(), 0) + leftover * 50;
    }
    }
    throw std::invalid_argument{"Invalid rule number"};
}

bool Rule::isValidRating(int rating) const
{
    return m_minRating <= rating && rating <= m_maxRating;
}

int Rule::getMinRating() const
{
    return m_minRating;
}

int Rule::getMaxRating() const
{
    return m_maxRating;
}

int Rule::getMaxRatingPerRound() const
{
    return m_maxRatingPerRound;
}
