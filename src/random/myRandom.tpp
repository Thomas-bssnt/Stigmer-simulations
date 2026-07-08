#ifndef MY_RANDOM_TPP
#define MY_RANDOM_TPP

#ifndef MY_RANDOM_H
#error __FILE__ should only be included from myRandom.h.
#endif

#include <algorithm> // std::shuffle
#include <stdexcept> // std::invalid_argument

template <typename T>
T myRandom::choice(const std::vector<T> &population)
{
    return population[myRandom::randIndex(population.size())];
}

template <typename T>
std::vector<T> myRandom::choice(const std::vector<T> &population, std::size_t size)
{
    std::vector<T> newPopulation{};
    newPopulation.reserve(size);
    for (std::size_t i = 0; i < size; ++i)
    {
        newPopulation.emplace_back(myRandom::choice(population));
    }
    return newPopulation;
}

template <typename T, typename U>
T myRandom::choice(const std::vector<T> &population, const std::vector<U> &weights)
{
    if (population.size() != weights.size())
    {
        throw std::invalid_argument("Population and weights vectors must have the same size.");
    }

    return population[myRandom::randIndexWeighted(weights)];
}

template <typename T, typename U>
std::vector<T> myRandom::choice(const std::vector<T> &population,
                                const std::vector<U> &weights,
                                std::size_t size)
{
    std::vector<T> newPopulation{};
    newPopulation.reserve(size);
    for (std::size_t i = 0; i < size; ++i)
    {
        newPopulation.emplace_back(myRandom::choice(population, weights));
    }
    return newPopulation;
}

template <typename T>
void myRandom::shuffle(std::vector<T> &vector)
{
    std::shuffle(vector.begin(), vector.end(), myRandom::detail::engine);
}

#endif
