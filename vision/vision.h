/*
 * vision - implements the vision module, as described in Design Spec
 * 
 * Colinear, 2024
 */


#include <stdbool.h>
#include "../grid/grid.h"

/* Calculates the equation of the line running
 * through two given positions
 */
grid_t* calc_grid(grid_t* main_grid, player_t* player);


/* Checks whether a given position is inside
 * the room with respect to the vertical axis
 */
bool is_inside_vert(grid_t* main_grid, pos_t* pos);


/* Checks whether a given position is inside
 * the room with respect to the horizontal axis
 */
bool is_inside_horiz(grid_t* main_grid, pos_t* pos);


/* Checks whether two positions on a grid are 
 * visible from each other. Assumes that both
 * positions are inside the room.
 */
bool check_visible(grid_t* main_grid, pos_t* pos1, pos_t* pos2);


/* Calculates the visible grid
 * for a given player
 */
line_t* calc_line(pos_t* pos1, pos_t* pos2);