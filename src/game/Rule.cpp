#include <numeric>   // std::accumulate, std::inner_product
#include <stdexcept> // std::invalid_argument
#include <string>
#include <vector>

#include "game/Rule.h"

Rule::Rule(const RuleNumber &ruleNumber)
    : m_ruleNumber{ruleNumber}
{
    m_minRating = 0;
    m_maxRating = 5;
    switch (m_ruleNumber)
    {
    case RuleNumber::Rule1:
    case RuleNumber::Rule2:
        m_maxRatingPerRound = 15;
        break;
    case RuleNumber::Rule3:
    case RuleNumber::Rule4:
        m_maxRatingPerRound = 8;
        break;
    default:
        throw std::invalid_argument{"Invalid rule number"};
    }
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
        return std::inner_product(values.begin(), values.end(), stars.begin(), 0) +
               std::accumulate(stars.begin(), stars.end(), m_maxRatingPerRound, std::minus<>()) * 50;
    }
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
