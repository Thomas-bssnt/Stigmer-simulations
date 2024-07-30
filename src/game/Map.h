#ifndef MAP_H
#define MAP_H

#include <vector>

class Map
{
public:
    Map(int numberOfCells, bool random);

    bool isValidIndex(int index) const;

    int getNumberOfCells() const;

    int getValue(int index) const;

    const std::vector<int> &getValues() const;

private:
    const int m_numberOfCells;
    const std::vector<int> m_values;

    static std::vector<int> generateValues(int numberOfCells, bool random);
};

#endif
