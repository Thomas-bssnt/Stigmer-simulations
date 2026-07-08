#ifndef GAME_ANALYZER_H
#define GAME_ANALYZER_H

#include <vector>

#include "agent/Agent.h"
#include "game/Game.h"

/**
 * @brief Aggregates observables computed over many games.
 *
 * A `GameAnalyzer` is initialized with the expected number of games and, optionally, the specific
 * agent indices to analyze. For each game, `analyzeGame()` records per-game observables (visit and
 * rating distributions, best-cell values, discovery times, scores, ranks, MNS, ...). Getters return
 * the observables averaged over games; `saveObservables()` writes them to disk.
 */
class GameAnalyzer
{
public:
    /**
     * @brief Build an analyzer that tracks specific agents.
     *
     * @param numberOfGames Number of games that will be analyzed.
     * @param iAgents Indices of the agents to analyze in each game.
     */
    GameAnalyzer(int numberOfGames, std::vector<int> iAgents);

    /**
     * @brief Build an analyzer that tracks all `numberOfPlayers` agents (indices `0..numberOfPlayers-1`).
     *
     * @param numberOfGames Number of games that will be analyzed.
     * @param numberOfPlayers Number of agents per game.
     */
    GameAnalyzer(int numberOfGames, int numberOfPlayers);

    /**
     * @brief Record the observables of a single game.
     *
     * On the first call, the analyzer sizes its internal buffers using the game's parameters.
     *
     * @param iGame Index of the game (must be in [0, numberOfGames)).
     * @param game The finished game to analyze.
     * @param agents The agents that played the game.
     */
    void analyzeGame(int iGame, const Game &game, const std::vector<Agent> &agents);

    /**
     * @brief Write all averaged observables to files under the given directory.
     *
     * @param pathObservables Directory path (with trailing slash) where observables are written.
     */
    void saveObservables(std::string pathObservables) const;

    /** @brief Per-round instantaneous visit distribution performance, averaged over games. */
    std::vector<double> get_q() const;
    /** @brief Per-round cumulative visit distribution performance, averaged over games. */
    std::vector<double> get_Q() const;
    /** @brief Per-round instantaneous rating distribution performance, averaged over games. */
    std::vector<double> get_p() const;
    /** @brief Per-round cumulative rating distribution performance, averaged over games. */
    std::vector<double> get_P() const;
    /** @brief Inverse participation ratio of the instantaneous visit distribution, per round. */
    std::vector<double> get_IPR_q() const;
    /** @brief Inverse participation ratio of the cumulative visit distribution, per round. */
    std::vector<double> get_IPR_Q() const;
    /** @brief Inverse participation ratio of the instantaneous rating distribution, per round. */
    std::vector<double> get_IPR_p() const;
    /** @brief Inverse participation ratio of the cumulative rating distribution, per round. */
    std::vector<double> get_IPR_P() const;
    /** @brief Fidelity F of the cumulative visit distribution, per round. */
    std::vector<double> get_F_Q() const;
    /** @brief Fidelity F of the cumulative rating distribution, per round. */
    std::vector<double> get_F_P() const;
    /** @brief Probability that the best cell of round r-1 is replayed at turn 1 of round r. */
    std::vector<double> get_B1() const;
    /** @brief Probability that the 2nd best cell of round r-1 is replayed at turn 2 of round r. */
    std::vector<double> get_B2() const;
    /** @brief Probability that the 3rd best cell of round r-1 is replayed at turn 3 of round r. */
    std::vector<double> get_B3() const;
    /** @brief Mean value of the best cell played at turn 1, per round. */
    std::vector<double> get_V1() const;
    /** @brief Mean value of the best cell played at turn 2, per round. */
    std::vector<double> get_V2() const;
    /** @brief Mean value of the best cell played at turn 3, per round. */
    std::vector<double> get_V3() const;
    /** @brief Mean value of the best cell found since the start, turn 1, per round. */
    std::vector<double> get_VB1() const;
    /** @brief Mean value of the best cell found since the start, turn 2, per round. */
    std::vector<double> get_VB2() const;
    /** @brief Mean value of the best cell found since the start, turn 3, per round. */
    std::vector<double> get_VB3() const;
    /** @brief Probability of having found the value-99 cell, per round. */
    std::vector<double> get_find99() const;
    /** @brief Probability of having found one of the top second-tier cells (86/85/84), per round. */
    std::vector<double> get_find80() const;
    /** @brief Probability of having found one of the top third-tier cells (72/71), per round. */
    std::vector<double> get_find70() const;
    /** @brief Distribution of normalized individual scores. */
    std::vector<double> get_S() const;
    /** @brief Mean normalized individual score. */
    double get_S_mean() const;
    /** @brief Distribution of normalized group scores. */
    std::vector<double> get_S_group() const;
    /** @brief Mean normalized group score. */
    double get_S_group_mean() const;
    /** @brief Distribution of ranks (1 = best). */
    std::vector<double> get_rank() const;
    /** @brief Mean rank. */
    double get_rank_mean() const;
    /** @brief Mean number of stars given per opened cell value (MNS profile). */
    std::vector<double> get_MNS() const;

private:
    /**
     * @brief Allocate the internal per-game buffers using the game's parameters.
     *
     * @param game A game whose parameters seed the buffer sizes.
     */
    void initializeVariables(const Game &game);

    /** @brief Compute visit and rating distributions (instantaneous and cumulative) for a game. */
    void computeDistributions(int iGame, const Game &game);
    /** @brief Accumulate the values of the best cells played each turn/round for an agent. */
    void computeValuesBestCells(int iGame, const Agent &agent);
    /** @brief Accumulate the values of the best cells found since the start for an agent. */
    void computeValuesBestCellsSinceStart(int iGame, const Agent &agent);
    /** @brief Accumulate the replay indicator: did the agent replay the best cells of the previous round? */
    void computeReplayBestCells(int iGame, const Agent &agent);
    /** @brief Accumulate the discovery indicators for the top-tier cells of the map. */
    void computeFindBestCells(int iGame, const Agent &agent);
    /** @brief Record the normalized individual and group scores for one agent of one game. */
    void computeScore(int iGame, const Game &game, int iAgent, int iAgentToAnalyze);
    /** @brief Record the rank (1 = best) of one agent within one game. */
    void computeRank(int iGame, const Game &game, int iAgent, int iAgentToAnalyze);
    /** @brief Accumulate the mean-number-of-stars histogram for one agent of one game. */
    void computeMNS(int iGame, const Game &game, int iAgent);

    /**
     * @brief Compute a normalized performance from a distribution and the underlying cell values.
     *
     * @param distribution Probability of visiting/rating each cell.
     * @param values The value of each cell.
     * @param normalization Divisor applied to the raw expected value.
     * @return The normalized performance.
     */
    static double computePerf(const std::vector<double> &distribution, const std::vector<int> &values, double normalization);

    /**
     * @brief Compute the inverse participation ratio of a distribution.
     *
     * @param distribution Probability of visiting/rating each cell.
     * @return `1 / sum(p^2)`, or 0 if the distribution is null.
     */
    static double computeIPR(const std::vector<double> &distribution);

    /**
     * @brief Compute the fidelity F of a distribution relative to the map values.
     *
     * @param distribution Probability of visiting/rating each cell.
     * @param values The value of each cell.
     * @return `sum(sqrt(p*v)) / sqrt(sum(v))`.
     */
    static double computeF(const std::vector<double> &distribution, const std::vector<int> &values);

    /**
     * @brief Normalize an integer vector so that its entries sum to 1.
     *
     * @param vector The vector to normalize.
     * @return The normalized vector, or a vector of zeros if the input sums to 0.
     */
    static std::vector<double> normalize(const std::vector<int> &vector);

    /**
     * @brief Element-wise division of two integer vectors, returning NaN where the denominator is 0.
     *
     * @param numerator Numerator vector.
     * @param denominator Denominator vector, same size as `numerator`.
     * @return The element-wise quotient.
     */
    static std::vector<double> divide(const std::vector<int> &numerator, const std::vector<int> &denominator);

    /**
     * @brief Compute the arithmetic mean of a vector.
     *
     * @param vector The vector to average.
     * @return The mean of `vector`.
     */
    static double computeAverage(const std::vector<double> &vector);

    /**
     * @brief Compute the row-wise arithmetic mean of a 2D vector.
     *
     * @param vector2d The 2D vector to average.
     * @return A vector containing the mean of each row.
     */
    static std::vector<double> computeAverage(const std::vector<std::vector<double>> &vector2d);

    /**
     * @brief Compute a histogram of the values in a vector, in `numberOfDivisions` equal bins over `[min, max]`.
     *
     * @param vector The samples to bin.
     * @param numberOfDivisions Number of histogram bins.
     * @param min Lower edge of the histogram range.
     * @param max Upper edge of the histogram range.
     * @return The normalized histogram.
     */
    static std::vector<double> computeDistribution(const std::vector<double> &vector, int numberOfDivisions, double min, double max);

    /**
     * @brief Write a vector observable to a file.
     *
     * @param observablePath Path of the output file.
     * @param observable The observable values, written one per line.
     */
    void saveObservable(const std::string &observablePath, const std::vector<double> &observable) const;

    /**
     * @brief Write a scalar observable to a file.
     *
     * @param observablePath Path of the output file.
     * @param observable The observable value.
     */
    void saveObservable(const std::string &observablePath, double observable) const;

    //
    const int m_numberOfGames;
    std::vector<int> m_iAgents;
    const int m_numberOfPlayersToAnalyze;
    bool m_isInitialized;

    //
    int m_numberOfRounds;
    int m_numberOfTurns;
    int m_numberOfCells;

    //
    std::vector<std::vector<double>> m_q;
    std::vector<std::vector<double>> m_Q;
    std::vector<std::vector<double>> m_p;
    std::vector<std::vector<double>> m_P;
    std::vector<std::vector<double>> m_IPR_q;
    std::vector<std::vector<double>> m_IPR_Q;
    std::vector<std::vector<double>> m_IPR_p;
    std::vector<std::vector<double>> m_IPR_P;
    std::vector<std::vector<double>> m_F_Q;
    std::vector<std::vector<double>> m_F_P;
    std::vector<std::vector<std::vector<double>>> m_B;
    std::vector<std::vector<std::vector<double>>> m_V;
    std::vector<std::vector<std::vector<double>>> m_VB;
    std::vector<std::vector<std::vector<double>>> m_find;
    std::vector<double> m_S;
    std::vector<double> m_S_group;
    std::vector<double> m_rank;
    std::vector<int> m_MNS_ratings;
    std::vector<int> m_MNS_counts;
};

#endif
