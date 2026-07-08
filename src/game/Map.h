#ifndef MAP_H
#define MAP_H

#include <vector>

/**
 * @brief A game map that assigns a value to every cell.
 *
 * The map is generated from a fixed base distribution of 225 values. Larger maps are built by
 * repeating that distribution and, optionally, shuffling the resulting values.
 */
class Map
{
public:
    /**
     * @brief Build a map with the given number of cells.
     *
     * @param numberOfCells Number of cells in the map. Must be a multiple of 225.
     * @param random If `true`, the cell values are shuffled; otherwise they follow the base ordering.
     */
    Map(int numberOfCells, bool random);

    /**
     * @brief Check whether an index refers to a valid cell.
     *
     * @param index The cell index.
     * @return `true` if the index is in [0, `getNumberOfCells()`).
     */
    [[nodiscard]] bool isValidIndex(int index) const;

    /**
     * @brief Get the number of cells in the map.
     *
     * @return The number of cells.
     */
    [[nodiscard]] int getNumberOfCells() const;

    /**
     * @brief Get the value of a given cell.
     *
     * @param index The cell index.
     * @return The value of the cell at the given index.
     */
    [[nodiscard]] int getValue(int index) const;

    /**
     * @brief Get the values of all cells.
     *
     * @return A reference to the internal vector of cell values.
     */
    [[nodiscard]] const std::vector<int> &getValues() const;

private:
    const int m_numberOfCells;
    const std::vector<int> m_values;

    /**
     * @brief Generate the vector of cell values from the base distribution.
     *
     * @param numberOfCells Number of cells in the map. Must be a multiple of 225.
     * @param random If `true`, the cell values are shuffled.
     * @return The generated vector of cell values.
     */
    static std::vector<int> generateValues(int numberOfCells, bool random);
};

#endif
