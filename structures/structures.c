#include <stdio.h>
#include <string.h>
#include "../libcs50/set.h"
#include "../libcs50/mem.h"
#include "../support/message.h"
#include "structures.h"

/**************** global types ****************/
/************* position *************/
typedef struct position {
  double x;
  double y;
  } pos_t;

/************* line *************/
typedef struct line {
  double m;
  double c;
  } line_t;

/************* player *************/
typedef struct player {
  char* name;
  addr_t address;
  pos_t* position;
  char letter;
  int score;
  set_t* viewed;
} player_t;

/**************** player_new() ****************/
/* see structures.h for description */
player_t*
player_new(char* name, char letter)
{
  if (name == NULL || letter == '\0')
  {
    return NULL;
  }
  player_t* player = mem_malloc(sizeof(player_t));
  if (player == NULL) {
    return NULL;              
  } else {
    player->name = mem_malloc(strlen(name) + 1);  
    // Copy the name into the allocated memory
    strcpy(player->name, name);
    player->letter = letter;
    player->position = position_new(-1, -1);
    player->score = 0;
    player->viewed = set_new();
    return player;
  }
}

/**************** position_new() ****************/
/* see structures.h for description */
pos_t*
position_new(double x, double y)
{
  pos_t* pos = mem_malloc(sizeof(pos_t));
  if (pos == NULL) {
    return NULL;              
  }
  pos->x = x;
  pos->y = y;
  return pos;
}

/**************** line_new() ****************/
/* see structures.h for description */
line_t*
line_new(void)
{
  line_t* line = mem_malloc(sizeof(line_t));
  if (line == NULL) {
    return NULL;           
  }
  line->m = 0;
  line->c = 0;
  return line;
}

/**************** player_delete ****************/
/* see structures.h for description */
void 
player_delete(player_t* player)
{
  mem_free(player->name);
  position_delete(player->position);
  set_delete(player->viewed, mem_free);
  mem_free(player);
}

/**************** position_delete ****************/
/* see structures.h for description */
void 
position_delete(pos_t* position)
{
  mem_free(position);
}

/**************** line_delete ****************/
/* see structures.h for description */
void 
line_delete(line_t* line)
{
  mem_free(line);
}

/**************** create_key ****************/
/* see structures.h for description */
char*
create_key(pos_t* pos)
{
    if (pos == NULL)
    {
        return NULL;
    }
    char* key = mem_malloc(50 * sizeof(char));
    if (key == NULL) {
    fprintf(stderr, "Failed to allocate memory for key");
    exit(1);
    }
    // Format the key as a string "x_y"
    sprintf(key, "%.6f_%.6f", (double) pos->x, (double) pos->y);
    return key;
}

/**************** get_player_name ****************/
/* see structures.h for description */
char*
get_player_name(player_t* player)
{
  if (player == NULL)
  {
    return NULL;
  }
  return player->name;
}

//assume not NULL
/**************** get_player_address ****************/
/* see structures.h for description */
addr_t
get_player_address(player_t* player)
{
  return player->address;
}

/**************** get_player_position ****************/
/* see structures.h for description */
pos_t*
get_player_position(player_t* player)
{
  if (player == NULL)
  {
    return NULL;
  }
  return player->position;
}

/**************** get_player_letter ****************/
/* see structures.h for description */
char
get_player_letter(player_t* player)
{
  if (player == NULL)
  {
    return '\0';
  }
  return player->letter;
}

/**************** get_player_score ****************/
/* see structures.h for description */
int
get_player_score(player_t* player)
{
  if (player == NULL)
  {
    return -1;
  }
  return player->score;
}

/**************** get_player_viewed ****************/
/* see structures.h for description */
set_t*
get_player_viewed(player_t* player)
{
  if (player == NULL)
  {
    return NULL;
  }
  return player->viewed;
}

/**************** get_position_x ****************/
/* see structures.h for description */
double
get_position_x(pos_t* pos)
{
  if (pos == NULL)
  {
    return -1;
  }
  return pos->x;
}

/**************** get_position_y ****************/
/* see structures.h for description */
double
get_position_y(pos_t* pos)
{
  if (pos == NULL)
  {
    return -1;
  }
  return pos->y;
}

/**************** get_line_slope ****************/
/* see structures.h for description */
double
get_line_slope(line_t* line)
{
  if (line == NULL)
  {
    return -1;
  }
  return line->m;
}

/**************** get_line_intercept ****************/
/* see structures.h for description */
double
get_line_intercept(line_t* line)
{
  if (line == NULL)
  {
    return -1;
  }
  return line->c;
}

/**************** set_player_name ****************/
/* see structures.h for description */
void 
set_player_name(player_t* player, char* name_new)
{
  if (player != NULL && name_new != NULL)
  {
    player->name = name_new;
  }
}

/**************** set_player_address ****************/
/* see structures.h for description */
void 
set_player_address(player_t* player, addr_t address_new)
{
  
  if (player != NULL)
  {
    player->address = address_new;
  }
  
}

/**************** set_player_position ****************/
/* see structures.h for description */
void 
set_player_position(player_t* player, pos_t* position_new)
{
  if (player != NULL && position_new != NULL)
  {
    player->position = position_new;
  }
}

/**************** set_player_position_values ****************/
/* see structures.h for description */
void set_player_position_values(player_t* player, pos_t* position_new)
{
  if (player != NULL && position_new != NULL)
  {
    set_position_x(get_player_position(player), get_position_x(position_new));
    set_position_y(get_player_position(player), get_position_y(position_new));
  }
}

/**************** set_player_letter ****************/
/* see structures.h for description */
void 
set_player_letter(player_t* player, char letter_new)
{
  if (player != NULL && letter_new != '\0')
  {
    player->letter = letter_new;
  }
}

/**************** set_player_score ****************/
/* see structures.h for description */
void 
set_player_score(player_t* player, int score_new)
{
  if (player != NULL)
  {
    player->score = score_new;
  }
}

/**************** set_player_viewed ****************/
/* see structures.h for description */
void 
set_player_viewed(player_t* player, set_t* viewed_new)
{
  if (player != NULL && viewed_new != NULL)
  {
    player->viewed = viewed_new;
  }
}

/**************** set_position_x ****************/
/* see structures.h for description */
void 
set_position_x(pos_t* pos, double x_new)
{
  if (pos != NULL)
  {
    pos->x = x_new;
  }
}

/**************** set_position_y ****************/
/* see structures.h for description */
void 
set_position_y(pos_t* pos, double y_new)
{
  if (pos != NULL)
  {
    pos->y = y_new;
  }
}

/**************** set_line_slope ****************/
/* see structures.h for description */
void 
set_line_slope(line_t* line, double m_new)
{
  if (line != NULL)
  {
    line->m = m_new;
  }
}

/**************** set_line_intercept ****************/
/* see structures.h for description */
void 
set_line_intercept(line_t* line, double c_new)
{
  if (line != NULL)
  {
    line->c = c_new;
  }
}