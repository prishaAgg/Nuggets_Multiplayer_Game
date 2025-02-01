/*
 * visiontest - unit testing for the vision module
 * 
 * Colinear, 2024
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "vision.h"


/**************** Static constants ****************/
static const int numMoves = 8; // number of moves for test_calc_grid()


/*********** Function prototypes ***********/
void test_calc_line();
void test_is_inside_vert();
void test_is_inside_horiz();
void test_check_visible();
void test_calc_grid();


/**************** main ****************/
/* Runs the tests chronologically
 */
int main() {
    printf("Running tests for vision functions...\n");

    test_calc_line();
    test_is_inside_vert();
    test_is_inside_horiz();
    test_check_visible();
    test_calc_grid();

    printf("All tests passed.\n");
    return 0;
}


/**************** test_calc_line ****************/
/* Tests calc_line */

void test_calc_line() {
  // initializes data structures
  pos_t* pos1 = position_new(1,1);
  pos_t* pos2 = position_new(3,3);
  line_t* line = calc_line(pos1, pos2);

  // asserts
  assert(get_line_slope(line) == 1.0);
  assert(get_line_intercept(line) == 0.0);

  // cleans up
  position_delete(pos1);
  position_delete(pos2);
  line_delete(line);
  printf("test_calc_line passed.\n");
}


/**************** test_is_inside_vert ****************/
/* Tests is_inside_vert */

void test_is_inside_vert() {
  // initializes data structures
  grid_t* grid = grid_new("../maps/main.txt");
  pos_t* pos_inside = position_new(9, 2.2);
  pos_t* pos_outside = position_new(2,2.5);

  // asserts
  assert(is_inside_vert(grid, pos_inside) == true);
  assert(is_inside_vert(grid, pos_outside) == false);
  
  // cleans up
  position_delete(pos_inside);
  position_delete(pos_outside);
  grid_delete(grid);
  printf("test_is_inside_vert passed.\n");
}


/**************** test_is_inside_horiz ****************/
/* Tests is_inside_horiz */

void test_is_inside_horiz() {
  // initializes data structures
  grid_t* grid = grid_new("../maps/main.txt");
  pos_t* pos_inside = position_new(62, 15.5);
  pos_t* pos_outside = position_new(4.4, 4);

  // asserts
  assert(is_inside_horiz(grid, pos_outside) == false);
  assert(is_inside_horiz(grid, pos_inside) == true);

  // cleans up
  position_delete(pos_inside);
  position_delete(pos_outside);
  grid_delete(grid);
  printf("test_is_inside_horiz passed.\n");
  
}


/**************** test_check_visible ****************/
/* Tests check_visible */

void test_check_visible() {
  // initializes data structures
  grid_t* grid = grid_new("../maps/main.txt");
  pos_t* pos1 = position_new(5.0,1.0);
  pos_t* pos2 = position_new(11.0,1.0);
  pos_t* pos3 = position_new(5,10);

  // asserts
  assert(check_visible(grid, pos1, pos2) == true);
  assert(check_visible(grid, pos1, pos3) == false);

  position_delete(pos1);
  position_delete(pos2);
  position_delete(pos3);
  grid_delete(grid);
  printf("test_check_visible passed.\n");
}


/**************** test_calc_grid ****************/
/* Tests calc_grid by using the wasd keys to move
 * a test player in main.txt for numMoves number 
 * of moves and repeatedly prints that grid. Input 
 * comes from stdin and enter must be pressed
 * after each letter is added. Note that the user
 * CAN move the player to an outside-room position.
 */

void test_calc_grid() {
  // initializes data structures
  grid_t* main_grid = grid_new("../maps/main.txt");
  player_t* player = player_new("testplayer", 'T');
  pos_t* player_pos = get_player_position(player);
  // sets player position to (12,3) which is inside a room
  set_position_x(player_pos, 12);
  set_position_y(player_pos, 3);

  // calculates and displays 
  grid_t* perspective_grid = calc_grid(main_grid, player);
  grid_print(perspective_grid);

  // updates player position and prints the perspective grid
  char ch;
  for (int i = 0; i < numMoves * 2 ; i++) {
      ch = getchar();

      if (ch == 'w') {
          set_position_y(player_pos, get_position_y(player_pos) - 1);
      } else if (ch == 's') {
          set_position_y(player_pos, get_position_y(player_pos) + 1);
      } else if (ch == 'a') {
          set_position_x(player_pos, get_position_x(player_pos) - 1);
      } else if (ch == 'd') {
          set_position_x(player_pos, get_position_x(player_pos) + 1);
      } else if (ch != '\n') {
          printf("Invalid input. Use w, a, s, or d.\n");
      }

      grid_delete(perspective_grid);
      perspective_grid = calc_grid(main_grid, player);
      grid_print(perspective_grid);
  }

  // asserts that the player position has symbol '@'
  assert(grid_get_symbol(perspective_grid, player_pos) == '@');

  // cleans up
  player_delete(player);
  grid_delete(perspective_grid);
  grid_delete(main_grid);
  printf("test_calc_grid passed.\n");
}