#include <random>

#include "random/myRandom.h"

/*
 * Generates a random double value uniformly distributed in the interval [low, high).
 *
 * @param low: lower bound of the range (inclusive).
 * @param high: upper bound of the range (exclusive).
 * @return: a random double value in the range [low, high).
 */
double myRandom::rand(double low, double high)
{
    return std::uniform_real_distribution(low, high)(myRandom::detail::engine);
}

/*
 * Generates a random double value uniformly distributed in the interval [0., high).
 *
 * @param high: upper bound of the range (exclusive).
 * @return: a random double value in the range [0., high).
 */
double myRandom::rand(double high)
{
    return rand(0., high);
}

/*
 * Generates a random double value uniformly distributed in the interval [0., 1.).
 *
 * @return: a random double value in the range [0., 1.).
 */
double myRandom::rand()
{
    return rand(0., 1.);
}

/*
 * Generates a random integer value uniformly distributed in the interval [low, high].
 *
 * @param low: lower bound of the range (inclusive).
 * @param high: upper bound of the range (inclusive).
 * @return: a random integer in the range [low, high].
 */
int myRandom::randInt(int low, int high)
{
    return std::uniform_int_distribution(low, high)(myRandom::detail::engine);
}

/*
 * Generates a random index value uniformly distributed in the interval [0, high].
 *
 * @param high: upper bound of the range (inclusive).
 * @return: a random index in the range [0, high].
 */
int myRandom::randInt(int high)
{
    return randInt(0, high);
}

/*
 * Generates a random index value uniformly distributed in the interval [low, high).
 *
 * @param low: lower bound of the range (inclusive).
 * @param high: upper bound of the range (exclusive).
 * @return: a random index in the range [low, high).
 */
std::size_t myRandom::randIndex(std::size_t low, std::size_t high)
{
    return std::uniform_int_distribution<std::size_t>(low, high - 1)(myRandom::detail::engine);
}

/*
 * Generates a random index value uniformly distributed in the interval [0, high).
 *
 * @param high: upper bound of the range (exclusive).
 * @return: a random index in the range [0, high).
 */
std::size_t myRandom::randIndex(std::size_t high)
{
    return randIndex(0, high);
}

/*
 * Generates a random index using a weighted probability distribution.
 *
 * @param weights: vector of weights.
 * @return: a random index from the weights vector according to the given weights.
 */
std::size_t myRandom::randIndexWeighted(const std::vector<double> &weights)
{
    return std::discrete_distribution<std::size_t>(weights.begin(), weights.end())(myRandom::detail::engine);
}
