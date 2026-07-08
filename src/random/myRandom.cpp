#include <random>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "random/myRandom.h"

void myRandom::seed(std::uint_fast32_t seed)
{
    detail::globalSeed = seed;
    detail::globalSeedSet = true;

    // Seed every thread in the OpenMP thread pool so that the
    // thread-local engines used later in parallel-for loops are
    // deterministic.  Each thread gets a unique seed derived from the
    // base seed and its thread id.
#pragma omp parallel
    {
#ifdef _OPENMP
        detail::engine.seed(seed + static_cast<std::uint_fast32_t>(omp_get_thread_num()));
#else
        detail::engine.seed(seed);
#endif
    }
}

double myRandom::rand(double low, double high)
{
    return std::uniform_real_distribution(low, high)(myRandom::detail::engine);
}

double myRandom::rand(double high)
{
    return rand(0., high);
}

double myRandom::rand()
{
    return rand(0., 1.);
}

int myRandom::randInt(int low, int high)
{
    return std::uniform_int_distribution(low, high)(myRandom::detail::engine);
}

int myRandom::randInt(int high)
{
    return randInt(0, high);
}

std::size_t myRandom::randIndex(std::size_t low, std::size_t high)
{
    return std::uniform_int_distribution<std::size_t>(low, high - 1)(myRandom::detail::engine);
}

std::size_t myRandom::randIndex(std::size_t high)
{
    return randIndex(0, high);
}

std::size_t myRandom::randIndexWeighted(const std::vector<double> &weights)
{
    return std::discrete_distribution<std::size_t>(weights.begin(), weights.end())(myRandom::detail::engine);
}
