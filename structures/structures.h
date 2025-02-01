/* 
 * structures.h - header file for CS50 structures module
 *
 * Colinear, 2024
 */

#ifndef __STRUCTURES
#define __STRUCTURES

#include <stdio.h>
#include "../libcs50/set.h"
#include "../libcs50/mem.h"
#include "../support/message.h"

/**************** global types ****************/
/*
typedef struct position pos_t;  // opaque to users of the module
typedef struct line line_t;  // opaque to users of the module
typedef struct player player_t;  // opaque to users of the module
*/

/**************** global types ****************/
/************* position *************/
typedef struct position pos_t;

/************* line *************/
typedef struct line line_t;

/************* player *************/
typedef struct player player_t;

/**************** functions ****************/

/**************** player_new() ****************/
/* Creates a new player with the given name and letter.
 * Allocates memory for the `player_t` structure and initializes its attributes,
 * including the name, letter, initial position, score, and the viewed set.
 */
player_t* player_new(char* name, char letter);

/**************** position_new() ****************/
/* Creates a new position with the given x and y coordinates.
 * Allocates memory for the `pos_t` structure and sets its coordinates.
 */
pos_t* position_new(double x, double y);

/**************** line_new() ****************/
/* Creates a new line structure with default slope and intercept.
 * Allocates memory for the `line_t` structure and sets the default values for slope (m) and intercept (c).
 */
line_t* line_new(void);

/**************** player_delete ****************/
/* Frees the memory associated with a player structure, including its name, position, and viewed set.
 */
void player_delete(player_t* item);

/**************** position_delete ****************/
/* Frees the memory associated with a position structure.
 */
void position_delete(pos_t* item);

/**************** line_delete ****************/
/* Frees the memory associated with a line structure.
 */
void line_delete(line_t* item);

/**************** createKey ****************/
/* Create a unique key for position structure.
 *
 * Caller provides:
 *   valid pointer to position structure.
 * We return:
 *   NULL if position is NULL
 *   char* key that is unique to the position
 * Notes:
 *   caller is responsible for freeing returned key.
 */
char* create_key(pos_t* pos);

/**************** get_player_name ****************/
/* Retrieves the name of a player.
 * Returns a pointer to the player's name or NULL if the player is NULL.
 */
char* get_player_name(player_t* player);

/**************** set_player_name ****************/
/* Sets the name of a player.
 */ 
void set_player_name(player_t* player, char* name_new);

/**************** get_player_position ****************/
/* Retrieves the position of a player.
 * Returns a pointer to the player's position or NULL if the player is NULL.
 */
pos_t* get_player_position(player_t* player);

/**************** set_player_position ****************/
/* Sets the position of a player.
 * Updates the player's position with the new position provided.
 */
void set_player_position(player_t* player, pos_t* position_new);

/**************** set_player_position_values ****************/
/* Sets the x and y values of the player's position based on a new position.
 */
void set_player_position_values(player_t* player, pos_t* position_new);

/**************** get_player_letter ****************/
/* Retrieves the letter associated with a player.
 * Returns the player's letter or '\0' if the player is NULL.
 */
char get_player_letter(player_t* player);

/**************** set_player_letter ****************/
/* Sets the letter associated with a player.
 * Updates the player's letter if the new letter is not the null character.
 */
void set_player_letter(player_t* player, char letter_new);

/**************** get_player_score ****************/
/* Retrieves the score of a player.
 * Returns the player's score or -1 if the player is NULL.
 */
int get_player_score(player_t* player);

/**************** set_player_score ****************/
/* Sets the score of a player.
 * Updates the player's score with the new value provided.
 */
void set_player_score(player_t* player, int score_new);

/**************** get_player_address ****************/
/* Retrieves the address of a player.
 * Returns the address structure of the player.
 * Assumes that the input player is not NULL.
 */
addr_t get_player_address(player_t* player);

/**************** set_player_address ****************/
/* Sets the address of a player.
 * Updates the player's address with the new address provided.
 */
void set_player_address(player_t* player, addr_t address_new);

/**************** get_player_viewed ****************/
/* Retrieves the set of viewed positions for a player.
 * Returns a pointer to the viewed set or NULL if the player is NULL.
 */
set_t* get_player_viewed(player_t* player);

/**************** set_player_viewed ****************/
/* Sets the set of viewed positions for a player.
 * Updates the player's viewed set with the new set provided.
 */
void set_player_viewed(player_t* player, set_t* viewed_new);

/**************** get_position_x ****************/
/* Retrieves the x-coordinate of a position.
 * Returns the x-coordinate or -1 if the position is NULL.
 */
double get_position_x(pos_t* pos);

/**************** set_position_x ****************/
/* Sets the x-coordinate of a position.
 * Updates the x-coordinate of the given position with the new value.
 */
void set_position_x(pos_t* pos, double x_new);

/**************** get_position_y ****************/
/* Retrieves the y-coordinate of a position.
 * Returns the y-coordinate or -1 if the position is NULL.
 */
double get_position_y(pos_t* pos);

/**************** set_position_y ****************/
/* Sets the y-coordinate of a position.
 * Updates the y-coordinate of the given position with the new value.
 */
void set_position_y(pos_t* pos, double y_new);

/**************** get_line_slope ****************/
/* Retrieves the slope (m) of a line.
 * Returns the slope or -1 if the line is NULL.
 */
double get_line_slope(line_t* line);

/**************** set_line_slope ****************/
/* Sets the slope (m) of a line.
 * Updates the slope of the given line with the new value provided.
 */
void set_line_slope(line_t* line, double m_new);

/**************** get_line_intercept ****************/
/* Retrieves the intercept (c) of a line.
 * Returns the intercept or -1 if the line is NULL.
 */
double get_line_intercept(line_t* line);

/**************** set_line_intercept ****************/
/* Sets the intercept (c) of a line.
 * Updates the intercept of the given line with the new value provided.
 */
void set_line_intercept(line_t* line, double c_new);

#endif // __STRUCTURES