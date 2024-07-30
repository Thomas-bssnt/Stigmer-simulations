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
    }

    // For reals

    double rand();

    double rand(double high);

    double rand(double low, double high);

    // For integers

    int randInt(int low, int high);

    int randInt(int high);

    // For indices

    std::size_t randIndex(std::size_t low, std::size_t high);

    std::size_t randIndex(std::size_t high);

    std::size_t randIndexWeighted(const std::vector<double> &weights);

    // For sequences

    template <typename T>
    T choice(const std::vector<T> &population);

    template <typename T>
    std::vector<T> choice(const std::vector<T> &population, std::size_t size);

    template <typename T, typename U>
    T choice(const std::vector<T> &population, const std::vector<U> &weights);

    template <typename T, typename U>
    std::vector<T> choice(const std::vector<T> &population, const std::vector<U> &weights, std::size_t size);

    template <typename T>
    void shuffle(std::vector<T> &vector);
}

#include "myRandom.tpp"

#endif
