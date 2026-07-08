#ifndef CELL_H
#define CELL_H

/**
 * @brief A cell of the game map, identified by its index and holding a value.
 */
struct Cell
{
    /** @brief Index of the cell in the map. */
    int index;
    /** @brief Value of the cell. */
    int value;
};

#endif
