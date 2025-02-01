/* 
 * grid.c - grid module
 *
 * see grid.h for more information.
 *
 * Colinear, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../libcs50/file.h"
#include "../libcs50/mem.h"
#include "../structures/structures.h"
#include "grid.h"

/************* grid *************/
typedef struct grid {
  char** main_grid;
  int** gold_grid;
  int height;
  int width;
} grid_t;

/**************** grid_new ****************/
/* see grid.h for description */
grid_t*
grid_new(char* mapFile)
{
    // make sure mapFile can be open to be read
    FILE* file = fopen(mapFile, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: unable to open %s\n", mapFile);
        return NULL;
    }
    // obtain map dimensions before copying map into 2D array
    int grid_width = 0;      // number of characters in line
    int grid_height = 0;     // number of lines
    // read the first line to determine the width
    char *line = file_readLine(file);
    if (line != NULL) {
        grid_width = strlen(line);
        grid_height++;
        free(line);
    }
    // Count remaining lines to determine the height
    while ((line = file_readLine(file)) != NULL) {
        grid_height++;
        free(line);
    }
    // reset file pointer to beginning of map file
    rewind(file);
    // copy file contents into 2D char* array (main_grid)
    // allocate memory for 2D array
    char** map = grid_new_blank_array(grid_width, grid_height);
    // Read character by character and copy to the 2D array
    int row = 0;
    int col = 0;
    char ch;
    while ((ch = fgetc(file)) != EOF && row < grid_height) {
        // Check for newline to move to the next row
        if (ch == '\n') {
            // Null-terminate the current line
            map[row][col] = '\0';  
            row++;
            col = 0;
        } else {
            if (col < grid_width) {
                map[row][col] = ch;
                col++;
            }
        }
    }
    fclose(file);
    // null-terminate last line if partially filled
    if (row < grid_height && col > 0) {
        map[row][col] = '\0';
    }
    grid_t* grid = mem_malloc(sizeof(grid_t));
    grid->main_grid = map;
    grid->height = grid_height;
    grid->width = grid_width;

    // inialize gold_grid and set values to 0
    // allocate memory for 2D array
    int** gold_map = mem_malloc(grid_height * sizeof(int*));
    if (gold_map == NULL) {
        fprintf(stderr, "Failed to allocate memory for gold_grid");
        exit(1);
    }
    for (int i = 0; i < grid_height; i++) 
    {
        gold_map[i] = mem_malloc((grid_width)*sizeof(int));  
        if (gold_map[i] == NULL) {
            perror("Failed to allocate memory for row");
            // Free previously allocated rows to avoid memory leaks
            for (int j = 0; j < i; j++) {
                mem_free(gold_map[j]);
            }
            mem_free(gold_map);
            exit(1);
        }
        // Initialize all elements in the current row to 0
        for (int j = 0; j < grid_width; j++) {
            gold_map[i][j] = 0;
        }
    }
    grid->gold_grid = gold_map;
    return grid;
}

/**************** grid_get_symbol ****************/
/* see grid.h for description */
char
grid_get_symbol(grid_t* grid, pos_t* pos)
{
    if (grid == NULL || pos == NULL)
    {
        return '\0';
    }
    if (!grid_is_inside(grid, pos))
    {
        return '\0';
    }
    int x_cord = (int)(get_position_x(pos));
    int y_cord = (int)(get_position_y(pos));
    return grid->main_grid[y_cord][x_cord];
}

/**************** grid_get_gold ****************/
/* see grid.h for description */
int 
grid_get_gold(grid_t* grid, pos_t* pos)
{
    if (grid == NULL || pos == NULL)
    {
        return 0;
    }
    if (!grid_is_inside(grid, pos))
    {
        return 0;
    }
    int x_cord = (int)(get_position_x(pos));
    int y_cord = (int)(get_position_y(pos));
    return grid->gold_grid[y_cord][x_cord];
}

/**************** grid_get_width ****************/
/* see grid.h for description */
int 
grid_get_width(grid_t* grid)
{
    if (grid == NULL)
    {
        return 0; 
    }
    return grid->width+1;   // includes null terminator
}

/**************** grid_get_height ****************/
/* see grid.h for description */
int 
grid_get_height(grid_t* grid)
{
    if (grid == NULL)
    {
        return 0; 
    }
    return grid->height+1;
}

/**************** get_main_grid ****************/
/* see grid.h for description */
char** 
get_main_grid(grid_t* grid)
{
    if (grid == NULL)
    {
        return 0; 
    }
    return grid->main_grid;
}

/**************** get_gold_grid ****************/
/* see grid.h for description */
int** 
get_gold_grid(grid_t* grid)
{
    if (grid == NULL)
    {
        return 0; 
    }
    return grid->gold_grid;
}

/**************** grid_print ****************/
/* see grid.h for description */
void
grid_print(grid_t* grid)
{
    if (grid == NULL) {
        return;
    }
    for (int i = 0; i < grid->height; i++) {
        for (int j = 0; j < grid->width; j++) {
            printf("%c", grid->main_grid[i][j]);
        }
        printf("\n");
    }
}

/**************** grid_delete ****************/
/* see grid.h for description */
void
grid_delete(grid_t* grid)
{
    if (grid == NULL) {
        return; // bad grid
    }

    // Free the main grid memory
    if (grid->main_grid != NULL) {
        for (int i = 0; i < grid->height; i++) {
        mem_free(grid->main_grid[i]);
        }
        mem_free(grid->main_grid);
    }

    // Free the gold grid memory
    if (grid->gold_grid != NULL) {
        for (int i = 0; i < grid->height; i++) {
        mem_free(grid->gold_grid[i]);
        }
        mem_free(grid->gold_grid);
    }
    // Free the grid structure
    mem_free(grid);
}

/**************** grid_set_symbol ****************/
/* see grid.h for description */
void 
grid_set_symbol(grid_t* grid, pos_t* pos, char c)
{
    if (grid == NULL || pos == NULL)
    {
        return;
    }
    if (!grid_is_inside(grid, pos))
    {
        return;
    }
    int x_cord = (int)(get_position_x(pos));
    int y_cord = (int)(get_position_y(pos));
    grid->main_grid[y_cord][x_cord] = c;
}

/**************** grid_remove_gold ****************/
/* see grid.h for description */
void 
grid_remove_gold(grid_t* grid, pos_t* pos)
{
    grid_set_gold(grid, pos, 0);
}

/**************** grid_set_gold ****************/
/* see grid.h for description */
void 
grid_set_gold(grid_t* grid, pos_t* pos, int n)
{
    if (grid == NULL || pos == NULL || n < 0)
    {
        return;
    }
    if (!grid_is_inside(grid, pos))
    {
        return;
    }
    int x_cord = (int)(get_position_x(pos));
    int y_cord = (int)(get_position_y(pos));
    grid->gold_grid[y_cord][x_cord] = n;
}

/**************** grid_is_inside ****************/
/* see grid.h for description */
bool 
grid_is_inside(grid_t* grid, pos_t* pos)
{

    if (grid == NULL || pos == NULL)
    {
        return false; 
    }
    int x_cord = (int)(get_position_x(pos));
    int y_cord = (int)(get_position_y(pos));
    if (x_cord >= grid->width || y_cord >= grid->height || x_cord < 0 || y_cord < 0) {
        // bad position coordinates  
        return false;
    }
    return true;
}

/**************** grid_new_blank_array ****************/
/* see grid.h for description */
char** 
grid_new_blank_array(int width, int height)
{
    if (width < 0 || height < 0)
    {
        return NULL;
    }
        // allocate memory for 2D array
    char** grid = mem_malloc(height * sizeof(char*));
    if (grid == NULL) {
        fprintf(stderr, "Failed to allocate memory for main_grid");
        exit(1);
    }
    for (int i = 0; i < height; i++) 
    {
        // +1 for null terminator
        grid[i] = mem_malloc((width+1)*sizeof(char));
        if (grid[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for row");
            // Free previously allocated rows to avoid memory leaks
            for (int j = 0; j < i; j++) {
                mem_free(grid[j]);
            }
            mem_free(grid);
            exit(1);
        }  
    }
    return grid;
}

/**************** make_grid_blank ****************/
/* see grid.h for description */
grid_t* 
make_grid_blank(int width, int height)
{
    grid_t* blankgrid = mem_malloc(sizeof(grid_t));
    blankgrid->main_grid = grid_new_blank_array(width, height);
    blankgrid->gold_grid = NULL;
    blankgrid->height = height;
    blankgrid->width = width;

    int row = 0;
    int col = 0;
    char ch = ' ';
    while (row < height) {
    // Check for newline to move to the next row
        if (col == width) {
            // Null-terminate the current line
            blankgrid->main_grid[row][col] = '\0';  
            row++;
            col = 0;
        } 
        else {
            if (col < width) {
                blankgrid->main_grid[row][col] = ch;
                col++;
            }
        }
    }
  return blankgrid;
}

/**************** grid_valid_position ****************/
/* see grid.h for description */
bool
grid_valid_position(grid_t* grid, pos_t* pos)
{
  if (grid_is_inside(grid, pos)) {
    char symbol = grid_get_symbol(grid, pos);
    if (symbol == '-' || symbol == '+'  || symbol == ' ' || symbol == '|') {
      return false;
    }
  }
  return true;
}

/**************** grid_in_room ****************/
/* see grid.h for description */
bool
grid_in_room(grid_t* grid, pos_t* pos)
{
  if (grid_is_inside(grid, pos)) {
    char symbol = grid_get_symbol(grid, pos);
    if (isupper(symbol) || symbol == '.' || symbol == '*') {
      return true;
    }
  }
  return false;
}