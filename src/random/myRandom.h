#ifndef MY_RANDOM_H
#define MY_RANDOM_H

#include <random>
#include <vector>

namespace myRandom
{
    namespace detail
    {
        thread_local static std::random_device randomDevice;
        thread_local static std::mt19937 engine(randomDevice());

        /// If non-zero, new threads use this as their base seed (combined with thread id).
        inline std::uint_fast32_t globalSeed{0};
        inline bool globalSeedSet{false};
    }

    /**
     * @brief Seed the random number generator for reproducible runs.
     *
     * Must be called before any parallel region. Each OpenMP thread will
     * derive its own seed from `seed + omp_get_thread_num()` so that
     * runs are reproducible yet threads remain independent.
     *
     * @param seed The base seed value.
     */
    void seed(std::uint_fast32_t seed);

    // For reals

    /**
     * @brief Generate a random double uniformly distributed in [0, 1).
     *
     * @return A random double in [0, 1).
     */
    double rand();

    /**
     * @brief Generate a random double uniformly distributed in [0, high).
     *
     * @param high Upper bound of the range (exclusive).
     * @return A random double in [0, high).
     */
    double rand(double high);

    /**
     * @brief Generate a random double uniformly distributed in [low, high).
     *
     * @param low Lower bound of the range (inclusive).
     * @param high Upper bound of the range (exclusive).
     * @return A random double in [low, high).
     */
    double rand(double low, double high);

    // For integers

    /**
     * @brief Generate a random integer uniformly distributed in [low, high].
     *
     * @param low Lower bound of the range (inclusive).
     * @param high Upper bound of the range (inclusive).
     * @return A random integer in [low, high].
     */
    int randInt(int low, int high);

    /**
     * @brief Generate a random integer uniformly distributed in [0, high].
     *
     * @param high Upper bound of the range (inclusive).
     * @return A random integer in [0, high].
     */
    int randInt(int high);

    // For indices

    /**
     * @brief Generate a random index uniformly distributed in [low, high).
     *
     * @param low Lower bound of the range (inclusive).
     * @param high Upper bound of the range (exclusive).
     * @return A random index in [low, high).
     */
    std::size_t randIndex(std::size_t low, std::size_t high);

    /**
     * @brief Generate a random index uniformly distributed in [0, high).
     *
     * @param high Upper bound of the range (exclusive).
     * @return A random index in [0, high).
     */
    std::size_t randIndex(std::size_t high);

    /**
     * @brief Generate a random index using a weighted probability distribution.
     *
     * @param weights Vector of weights.
     * @return A random index drawn according to the given weights.
     */
    std::size_t randIndexWeighted(const std::vector<double> &weights);

    // For sequences

    /**
     * @brief Select a random element from the population vector.
     *
     * @param population Vector of elements to choose from.
     * @return A random element from the population.
     */
    template <typename T>
    T choice(const std::vector<T> &population);

    /**
     * @brief Select `size` random elements from the population vector.
     *
     * @param population Vector of elements to choose from.
     * @param size Number of elements to choose.
     * @return A vector containing the chosen elements.
     */
    template <typename T>
    std::vector<T> choice(const std::vector<T> &population, std::size_t size);

    /**
     * @brief Select a random element from the population vector according to the given weights.
     *
     * @param population Vector of elements to choose from.
     * @param weights Vector of weights associated to the elements of the population.
     * @return A random element from the population drawn according to the given weights.
     */
    template <typename T, typename U>
    T choice(const std::vector<T> &population, const std::vector<U> &weights);

    /**
     * @brief Select `size` random elements from the population vector according to the given weights.
     *
     * @param population Vector of elements to choose from.
     * @param weights Vector of weights associated to the elements of the population.
     * @param size Number of elements to choose.
     * @return A vector containing the chosen elements.
     */
    template <typename T, typename U>
    std::vector<T> choice(const std::vector<T> &population, const std::vector<U> &weights, std::size_t size);

    /**
     * @brief Shuffle the given vector in place.
     *
     * @param vector The vector to shuffle.
     */
    template <typename T>
    void shuffle(std::vector<T> &vector);
}

#include "myRandom.tpp"

#endif
