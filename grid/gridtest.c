/* 
 * gridtest.c - code to test the grid module
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grid.h"

// tester
int
main(const int argc, char* argv[])
{
    char* map = "../maps/main.txt";
    grid_t* grid = grid_new(map);
    grid_print(grid);
    printf("\n");
    //int** gold = grid_getGoldArray(grid);
    //for (int i = 0; i < grid_getHeight(grid); i++) {
    //    for (int j = 0; j < grid_getWidth(grid); j++) {
    //        printf("%d ", gold[i][j]);
    //    }
    //    printf("\n");
    //}
    grid_delete(grid);
}