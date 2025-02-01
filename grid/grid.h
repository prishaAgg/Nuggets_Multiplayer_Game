/* 
 * grid.h - header file for grid module
 *
 * A *grid* is the in-memory representation of the game's map as specified by map.txt
 *
 * Colinear, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../structures/structures.h"

/**************** global types ****************/
typedef struct grid grid_t;     // opaque to users of the module

/**************** functions ****************/

/**************** grid_new ****************/
/* Creates a new grid based on a map file provided as input.
 * Reads the map file to determine the dimensions, allocates memory for
 * the 2D array representation of the map and initializes the gold grid.
 */
grid_t* grid_new(char* mapFile);

/**************** grid_delete ****************/
/* Frees the memory associated with the grid.
 * Releases memory for both the main grid and the gold grid.
 */
void grid_delete(grid_t* grid);

/**************** grid_print ****************/
/* Prints the grid to standard output.
 * Outputs each row of the main grid, line by line.
 */
void grid_print(grid_t* grid);

/**************** grid_get_symbol ****************/
/* Retrieves the symbol at a specified position in the grid.
 * Returns the character located at the given position, or '\0' if
 * the position is invalid or outside the grid boundaries.
 */
char grid_get_symbol(grid_t* grid, pos_t* pos);

/**************** grid_get_gold ****************/
/* Retrieves the amount of gold at a specified position in the grid.
 * Returns the number of gold nuggets at the position or 0 if the position is invalid.
 */
int grid_get_gold(grid_t* grid, pos_t* pos);

/**************** grid_get_width ****************/
/* Retrieves the width of the grid.
 * Returns the width plus one for the full size including the null terminator.
 */
int grid_get_width(grid_t* grid);

/**************** grid_get_height ****************/
/* Retrieves the height of the grid.
 * Returns the height plus one for the full size including the null terminator.
 */
int grid_get_height(grid_t* grid);

/**************** get_main_grid ****************/
/* Retrieves the main 2D character array of the grid.
 * Returns a pointer to the main grid array or NULL if the grid is invalid.
 */
char** get_main_grid(grid_t* grid);

/**************** get_gold_grid ****************/
/* Retrieves the gold grid (2D array of integers) of the grid.
 * Returns a pointer to the gold grid array or NULL if the grid is invalid.
 */
int** get_gold_grid(grid_t* grid);

/**************** grid_is_inside ****************/
/* Checks if a position is inside the grid boundaries.
 * Returns true if the position is valid; otherwise, returns false.
 */
bool grid_is_inside(grid_t* grid, pos_t* pos);

/**************** grid_set_symbol ****************/
/* Sets a specific symbol at a given position in the grid.
 * Does nothing if the position is outside the grid boundaries or if the input is invalid.
 */
void grid_set_symbol(grid_t* grid, pos_t* pos, char c);

/**************** grid_set_gold ****************/
/* Sets the amount of gold at a specific position in the grid.
 * Does nothing if the input is invalid or if the amount is negative.
 */
void grid_set_gold(grid_t* grid, pos_t* pos, int n);

/**************** grid_remove_gold ****************/
/* Removes gold from the specified position by setting its value to 0.
 */
void grid_remove_gold(grid_t* grid, pos_t* pos);

/**************** grid_new_blank_array ****************/
/* Creates a new blank 2D array with the given width and height.
 * Allocates memory for the array and returns a pointer to it.
 */
char** grid_new_blank_array(int width, int height);

/**************** make_grid_blank ****************/
/* Creates a blank grid with the given width and height.
 * Initializes all cells with a space character.
 */
grid_t* make_grid_blank(int width, int height);

/**************** grid_valid_position ****************/
/* Checks if a position is valid for gameplay (i.e., not a wall or boundary).
 * Returns true if the position is valid; otherwise, returns false.
 */
bool grid_valid_position(grid_t* grid, pos_t* pos);

/**************** grid_in_room ****************/
/* Checks if a position is within a room ('.', '*', or an uppercase letter).
 * Returns true if the position is inside a room; otherwise, returns false.
 */
bool grid_in_room(grid_t* grid, pos_t* pos);