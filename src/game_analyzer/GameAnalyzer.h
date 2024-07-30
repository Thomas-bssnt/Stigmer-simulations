#ifndef GAME_ANALYZER_H
#define GAME_ANALYZER_H

#include <vector>

#include "agent/Agent.h"
#include "game/Game.h"

class GameAnalyzer
{
public:
    GameAnalyzer(int numberOfGames, std::vector<int> iAgents);
    GameAnalyzer(int numberOfGames, int numberOfPlayers);

    void analyzeGame(int iGame, const Game &game, const std::vector<Agent> &agents);

    void saveObservables(std::string pathObservables) const;

    std::vector<double> get_q() const;
    std::vector<double> get_Q() const;
    std::vector<double> get_p() const;
    std::vector<double> get_P() const;
    std::vector<double> get_IPR_q() const;
    std::vector<double> get_IPR_Q() const;
    std::vector<double> get_IPR_p() const;
    std::vector<double> get_IPR_P() const;
    std::vector<double> get_F_Q() const;
    std::vector<double> get_F_P() const;
    std::vector<double> get_B1() const;
    std::vector<double> get_B2() const;
    std::vector<double> get_B3() const;
    std::vector<double> get_V1() const;
    std::vector<double> get_V2() const;
    std::vector<double> get_V3() const;
    std::vector<double> get_VB1() const;
    std::vector<double> get_VB2() const;
    std::vector<double> get_VB3() const;
    std::vector<double> get_find99() const;
    std::vector<double> get_find80() const;
    std::vector<double> get_find70() const;
    std::vector<double> get_S() const;
    double get_S_mean() const;
    std::vector<double> get_S_group() const;
    double get_S_group_mean() const;
    std::vector<double> get_rank() const;
    double get_rank_mean() const;
    std::vector<double> get_MNS() const;

private:
    void initializeVariables(const Game &game);

    void computeDistributions(int iGame, const Game &game);
    void computeValuesBestCells(int iGame, const Agent &agent);
    void computeValuesBestCellsSinceStart(int iGame, const Agent &agent);
    void computeReplayBestCells(int iGame, const Agent &agent);
    void computeFindBestCells(int iGame, const Agent &agent);
    void computeScore(int iGame, const Game &game, int iAgent, int iAgentToAnalyze);
    void computeRank(int iGame, const Game &game, int iAgent, int iAgentToAnalyze);
    void computeMNS(int iGame, const Game &game, int iAgent);

    static double computePerf(const std::vector<double> &distribution, const std::vector<int> &values, double normalization);
    static double computeIPR(const std::vector<double> &distribution);
    static double computeF(const std::vector<double> &distribution, const std::vector<int> &values);
    static std::vector<double> normalize(const std::vector<int> &vector);
    static std::vector<double> divide(const std::vector<int> &numerator, const std::vector<int> &denominator);

    static double computeAverage(const std::vector<double> &vector);
    static std::vector<double> computeAverage(const std::vector<std::vector<double>> &vector2d);
    static std::vector<double> computeDistribution(const std::vector<double> &vector, int numberOfDivisions, double min, double max);

    void saveObservable(const std::string &observablePath, const std::vector<double> &observable) const;
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
