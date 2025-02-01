/*
 * vision - implements the vision module, as described in Design Spec
 * 
 * Colinear, 2024
 */


#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "../libcs50/set.h"
#include "vision.h"


/**************** Global Variables ****************/
const int radius = 5;

/**************** calc_line ****************/
/* see vision.h for description */

line_t* calc_line(pos_t* pos1, pos_t* pos2)
{
  double m = (get_position_y(pos2) - get_position_y(pos1) )/(get_position_x(pos2)  - get_position_x(pos1));
  double c = (get_position_y(pos1) - m * get_position_x(pos1));

  line_t* line = line_new();
  set_line_slope(line, m);
  set_line_intercept(line, c);
  return line;
}


/**************** is_inside_vert ****************/
/* see vision.h for description */

bool is_inside_vert(grid_t* main_grid, pos_t* pos)
{
  // if the position has integer y value
  if ((int)get_position_y(pos) == get_position_y(pos)) {
    return grid_in_room(main_grid, pos);
  }

  // calculates the integer y positions directly above and below
  double y_ceil = ceil(get_position_y(pos));
  double y_floor = floor(get_position_y(pos));
  pos_t* up = position_new(get_position_x(pos), y_ceil);
  pos_t* down = position_new(get_position_x(pos), y_floor);

  // evaluates and cleans up
  bool inside = grid_in_room(main_grid, up) || grid_in_room(main_grid, down);
  position_delete(up);
  position_delete(down);

  return inside;
}


/**************** is_inside_horiz ****************/
/* see vision.h for description */

bool is_inside_horiz(grid_t* main_grid, pos_t* pos)
{
  // if the position has integer x value
  if ((int)get_position_x(pos) == get_position_x(pos)) {
    return grid_in_room(main_grid, pos);
  }

  // calculates the integer x positions directly left and right
  double x_ceil = ceil(get_position_x(pos));
  double x_floor = floor(get_position_x(pos));
  pos_t* right = position_new(x_ceil, get_position_y(pos));
  pos_t* left = position_new(x_floor, get_position_y(pos));

  // evaluates and cleans up
  bool inside = grid_in_room(main_grid, right) || grid_in_room(main_grid, left);
  position_delete(right);
  position_delete(left);

  return inside;
}


/**************** check_visible ****************/
/* see vision.h for description */

bool check_visible(grid_t* main_grid, pos_t* pos1, pos_t* pos2)
{
  // initializes data structures
  double x;
  double y;
  pos_t* left;
  pos_t* right;
  pos_t* up;
  pos_t* down;
  line_t* line = calc_line(pos1, pos2);

  // ensures passage corners cannot be seen 
  if (grid_get_symbol(main_grid, pos2) == '#' && get_line_slope(line) != 0 && isfinite(get_line_slope(line))) {
    line_delete(line);
    return false;
  }

  // checking the x intersects
  if (get_position_x(pos1) != get_position_x(pos2)) {
    // finds the left and right positions
    if (get_position_x(pos1) < get_position_x(pos2)) {
      left = pos1;
      right = pos2;
    } else {
      left = pos2;
      right = pos1;
    }

    // loops over the x positions
    for (x = (get_position_x(left)) + 1; x < get_position_x(right); x++) {
      y = get_line_slope(line) * x + get_line_intercept(line);
      pos_t* pos = position_new(x, y);

      // if the line of vision runs outside the room
      if (!is_inside_vert(main_grid, pos)) {
        position_delete(pos);
        line_delete(line);
        return false;
      }
      position_delete(pos);
    }
  }

  // checking the y intersects
  if (get_position_y(pos1) != get_position_y(pos2)) {
    // finds the up and down positions
    if (get_position_y(pos1) < get_position_y(pos2)) {
      up = pos1;
      down = pos2;
    } else {
      up = pos2;
      down = pos1;
    }

    // loops over the y positions
    for (y = (get_position_y(up)) + 1; y < get_position_y(down); y++) {

      // handles the case when the slope is vertical
      if (isinf(get_line_slope(line))) {
        x = get_position_x(up);
      } else {
        x = (y - get_line_intercept(line)) / get_line_slope(line);
      }
      pos_t* pos = position_new(x, y);

      // if the line of vision runs outside the room
      if (!is_inside_horiz(main_grid, pos)) {
        position_delete(pos);
        line_delete(line);
        return false;
      }
      position_delete(pos);
    }
  }
  // clean up and return
  line_delete(line);
  return true;
  }


/**************** calc_grid ****************/
/* see vision.h for description */

grid_t* calc_grid(grid_t* main_grid, player_t* player)
{
  // initializes data structures
  pos_t* player_pos = get_player_position(player);
  set_t* viewed = get_player_viewed(player);
  grid_t* perspective_grid = make_grid_blank(grid_get_width(main_grid), grid_get_height(main_grid));


  // loops over all positions in the grid
  for (int x = 0; x < grid_get_width(main_grid); x++) {
    for (int y = 0; y < grid_get_height(main_grid); y++) {
      pos_t* temp_pos = position_new((double)x, (double)y);

      // if the position is the player position
      if (get_position_x(temp_pos) == get_position_x(player_pos) && get_position_y(temp_pos) == get_position_y(player_pos)) {
        grid_set_symbol(perspective_grid, temp_pos, '@');
        position_delete(temp_pos);
        continue;
      }

      // if temp_ps further than 'radius' away from player_pos, skip it 
      if (pow(get_position_x(player_pos) - get_position_x(temp_pos), 2) + 
        pow(get_position_y(player_pos) - get_position_y(temp_pos), 2) > 
        pow(radius, 2)) {
        position_delete(temp_pos);
        continue;
        }

      char* key = create_key(temp_pos);
      if (set_find(viewed, key) != NULL) {  // if the position has already been viewed
        grid_set_symbol(perspective_grid, temp_pos, grid_get_symbol(main_grid, temp_pos));
        position_delete(temp_pos);
      } else if (check_visible(main_grid, temp_pos, player_pos)) {  // if the position is viewable
        set_insert(viewed, key, temp_pos);
        grid_set_symbol(perspective_grid, temp_pos, grid_get_symbol(main_grid, temp_pos));
      } else {  // unviewable position--do nothing
        position_delete(temp_pos);
      }
      mem_free(key);
    }
  }
  // return the final grid
  return perspective_grid;
}