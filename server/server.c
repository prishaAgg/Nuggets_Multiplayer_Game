/*
 * server - implements all game logic as described in Requirements Spec
 * 
 * usage: ./server 2>server.log map.txt
 * 
 * Colinear, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../libcs50/mem.h"
#include "../support/message.h"
#include "../libcs50/set.h"
#include "../grid/grid.h"
#include "../vision/vision.h"
#include "../structures/structures.h"
#include<unistd.h>

#define MaxPlayers 26                   // max number of players

/**************** Static constants ****************/
static const int MaxNameLength = 50;    // max number of chars in playerName
static const int GoldTotal = 250;        // total amount of gold
static const int GoldMinNumPiles = 10;   // minimum number of gold piles
static const int GoldMaxNumPiles = 30;   // maximum number of gold piles

/**************** file-local global variables ****************/
player_t* players[MaxPlayers];          // Array to hold player pointers
int numPlayers = 0;                     // Current number of players
grid_t* main_grid;                      // main player grid
grid_t* original_grid;                  // grid to keep track of symbols (not changed)
addr_t spectator;                       // spectator address
int totalGold = GoldTotal;              // Remaining gold nuggets
bool flag = false;                      // returned by handle_message to exit message_loop

/*********** Function prototypes ***********/
int main(int argc, char* argv[]);
int parse_args(int argc, char* argv[], char** map_filename, int* seed);
void initialize_game(char* map_filename, int seed);
void game_over();
bool handle_message(void* arg, const addr_t from, const char* message);
void process_keystroke(char keystroke, player_t* player);
void update_grid();
player_t* add_player(char* name, addr_t* address, char letter);
player_t* get_player_by_address(addr_t* address);
player_t* find_player_at_position(pos_t* pos);
void setup_grid_with_gold(grid_t* grid);
char* format_grid_message(grid_t* grid);
void send_spectator_gold_message(addr_t spectator);
void send_gold_message(player_t* player, int collected, int purse);
int compare_players_by_score(const void* a, const void* b);
void handle_quit(player_t* player, addr_t spectator, const addr_t* sender, bool isSpectator);
bool position_equal(pos_t* pos1, pos_t* pos2);
bool sanitize_name(const char* input_name, char* sanitized_name, addr_t from);

/**************** main ****************/
/* Entry point for the server. Parses arguments,  
 * initializes the game, and starts the message loop.
 */
int main(int argc, char* argv[]) {
    char* map_filename = NULL;
    int seed = 0;

    if (parse_args(argc, argv, &map_filename, &seed) != 0) {
        return 1;
    }

    initialize_game(map_filename, seed);

    // Enter the message handling loop
    message_loop(NULL, 0, NULL, NULL, handle_message);
    return 0;
}

/**************** parse_args ****************/
/* Parses command-line arguments to extract the map filename 
 * and optional seed. Validates input and ensures the map file exists.
 * Returns: 0 on success, non-zero on failure.
 */
int parse_args(int argc, char* argv[], char** map_filename, int* seed) {
   // Check for proper number of arguments
   if (argc < 2 || argc > 3) {
       fprintf(stderr, "Usage: ./server map.txt [seed]\n");
       return 2;
   }
   *map_filename = argv[1];

   // Check if the map file exists
   struct stat buffer;
   if (stat(*map_filename, &buffer) != 0) {
       fprintf(stderr, "Error: map file '%s' does not exist.\n", *map_filename);
       return 3;
   }

    // Validate the seed is a non-negative integer
    if (argc == 3) {
        for (int i = 0; argv[2][i] != '\0'; i++) {
            if (!isdigit(argv[2][i])) {
                fprintf(stderr, "Error: seed must be a non-negative integer.\n");
                return 4;
            }
        }
        *seed = atoi(argv[2]);
    } else {
        *seed = 0;  // Default seed
    }

   return 0;
}

/**************** initialize_game ****************/
/* Sets up the game environment, including initializing the grid,
 * seeding the random number generator, and configuring player slots.
 */
void
initialize_game(char* map_filename, int seed)
{
    // Initialize random number generator
    if (seed > 0) {
        srand(seed);
    }
    else {
        srand(getpid());
    }

    // Initialize player slots
    for (int i = 0; i < MaxPlayers; i++) {
        players[i] = NULL;
    }

    // Initialize the messaging system
    int port = message_init(stderr);
    if (port <= 0) {
        fprintf(stderr, "Failed to initialize messaging system\n");
        exit(1);
    }

    // Load the grid
    main_grid = grid_new(map_filename);
    original_grid = grid_new(map_filename);
    if (main_grid == NULL) {
        fprintf(stderr, "Failed to initialize grid\n");
        exit(1);
    }

    // Set up grid with gold
    setup_grid_with_gold(main_grid);
}

/**************** handle_message ****************/
/* Processes incoming messages from players or the spectator.
 * Handles actions like movement, joining, and quitting.
 * Returns: true to continue receiving messages, false to quit (using flag)
 */
bool
handle_message(void* arg, const addr_t from, const char* message)
{
    if (flag) {
        return true;
    }
    // Handle "KEY" messages for player movement or actions
    if (strncmp(message, "KEY ", 4) == 0) {
        char keystroke = message[4];

        // Quit action
        if (keystroke == 'Q') {
            // Check if the sender is the spectator
            bool isSpectator = message_isAddr(spectator) && message_eqAddr(spectator, from);

            // Find the player if it's not the spectator
            player_t* player = NULL;
            if (!isSpectator) {
                player = get_player_by_address((addr_t*)&from);
            }

            // Handle player or spectator quitting
            handle_quit(player, spectator, &from, isSpectator);
            return false;
        }

        // Find the player associated with the message sender
        player_t* player = get_player_by_address((addr_t*)&from);

        // Process the player's keystroke
        if (player != NULL) {
            process_keystroke(keystroke, player);
        }
    }

    // Handle spectator messages
    else if (strncmp(message, "SPECTATE", 8) == 0) {
        // Replace the current spectator if needed
        if (message_isAddr(spectator)) {
            if (!message_eqAddr(spectator, from)) {
                message_send(spectator, "QUIT You have been replaced by a new spectator.");
            }
        }
        spectator = from;

        // Send grid dimensions and gold message to the new spectator
        char welcome_message[128];
        snprintf(welcome_message, sizeof(welcome_message),
                    "GRID %d %d", grid_get_height(main_grid), grid_get_width(main_grid));
        message_send(from, welcome_message);
        send_spectator_gold_message(spectator);
        update_grid();
    }

    // Handle player join messages
    else if (strncmp(message, "PLAY ", 5) == 0) {
        // Check if the maximum number of players has been reached
        if (numPlayers >= MaxPlayers) {
            message_send(from, "QUIT Game is full: no more players can join.");
            return false;
        }

        // Extract and sanitize the player's name
        const char* real_name_start = message + 5;
        char sanitized_name[MaxNameLength + 1];
        if (!sanitize_name(real_name_start, sanitized_name, from)) {
            return false; // Name is invalid, already sent QUIT message
        }

        // Assign a unique letter to the player
        char player_letter = 'A' + numPlayers; // Assign unique letter

        // Add the player to the game
        player_t* new_player = add_player(sanitized_name, (addr_t*)&from, player_letter);
        if (new_player != NULL) {
            // Send the player a welcome message and initial gold status
            char welcome_message[128];
            snprintf(welcome_message, sizeof(welcome_message),
                    "GRID %d %d", grid_get_height(main_grid), grid_get_width(main_grid));
            message_send(from, welcome_message);
            send_gold_message(new_player, 0, 0);

            // Update grid to reflect the new player
            update_grid();
        }
        else {
            message_send(from, "QUIT Error adding player.");
        }
    }
    // Return the flag to indicate whether to continue processing messages
    return flag;
}

/**************** process_keystroke ****************/
/* Handles a player's keystroke to perform movement, 
 * interaction with other players, and gold collection.
 */
void
process_keystroke(char keystroke, player_t* player)
{
    if (player == NULL) {
        fprintf(stderr, "Error: Player is NULL\n");
        return;
    }
        
    int dx = 0, dy = 0; // Movement deltas

    // Determine movement direction based on the keystroke
    if (keystroke == 'h' || keystroke == 'H') { dx = -1; }                  // Move left
    else if (keystroke == 'l' || keystroke == 'L') { dx = 1; }              // Move right
    else if (keystroke == 'j' || keystroke == 'J') { dy = 1; }              // Move down
    else if (keystroke == 'k' || keystroke == 'K') { dy = -1; }             // Move up
    else if (keystroke == 'y' || keystroke == 'Y') { dx = -1; dy = -1; }    // Diagonal up-left
    else if (keystroke == 'u' || keystroke == 'U') { dx = 1; dy = -1; }     // Diagonal up-right
    else if (keystroke == 'b' || keystroke == 'B') { dx = -1; dy = 1; }     // Diagonal down-left
    else if (keystroke == 'n' || keystroke == 'N') { dx = 1; dy = 1; }      // Diagonal down-right
    else {
        // Unknown keystroke: notify the player
        char* error_message = "ERROR usage: unknown keystroke";
        message_send(get_player_address(player), error_message);
        return;
    }

    // Determine if movement is continuous (uppercase letters)
    bool isContinuous = (keystroke >= 'A' && keystroke <= 'Z' && keystroke != 'Q'); // Uppercase = continuous

    pos_t* current_pos = get_player_position(player);

    // Process movement
    while (true) {
        pos_t* new_pos = position_new(get_position_x(current_pos) + dx, get_position_y(current_pos) + dy);

        if (!grid_valid_position(main_grid, new_pos)) {
            // Invalid position: stop movement
            position_delete(new_pos);
            break;
        }

        player_t* other_player = find_player_at_position(new_pos);
        if (other_player != NULL) {
            // Swap positions with another player
            pos_t* current_pos = get_player_position(player);
            pos_t* other_player_pos = get_player_position(other_player);

            // Swap positions
            double temp_x = get_position_x(current_pos);
            double temp_y = get_position_y(current_pos);
            set_position_x(current_pos, get_position_x(other_player_pos));
            set_position_y(current_pos, get_position_y(other_player_pos));
            set_position_x(other_player_pos, temp_x);
            set_position_y(other_player_pos, temp_y);

            // Update grid symbols for both players
            grid_set_symbol(main_grid, current_pos, get_player_letter(player));
            grid_set_symbol(main_grid, other_player_pos, get_player_letter(other_player));

            // Reflect the changes on the grid
            update_grid();
        } else {
            // Move player to the new position
            grid_set_symbol(main_grid, current_pos, grid_get_symbol(original_grid, current_pos));
            set_player_position_values(player, new_pos);
            grid_set_symbol(main_grid, new_pos, get_player_letter(player));
        }

        // Handle gold collection
        int gold = grid_get_gold(main_grid, get_player_position(player));
        if (gold > 0) {
            // Update player score and grid gold status
            set_player_score(player, get_player_score(player) + gold);
            grid_remove_gold(main_grid, get_player_position(player));
            totalGold -= gold;

            // Notify player of gold collection
            send_gold_message(player, gold, get_player_score(player));
            // Notify other players of updated scores
            for (int i = 0; i < numPlayers; i++) {
                if (!message_eqAddr(get_player_address(players[i]), get_player_address(player))) {
                    send_gold_message(players[i], 0, get_player_score(players[i]));    
                }
            }
        }
        // Update the grid for all players
        update_grid();
        // Free memory for new_pos after use
        position_delete(new_pos); 

        if (!isContinuous) {
            // Stop if the movement is not continuous
            break; 
        }
    }
}

/**************** update_grid ****************/
/* Updates the grid view for all players and the spectator.
 * Sends the updated visible grid to each player and the 
 * complete grid to the spectator. Ends the game if no gold remains.
 */
void update_grid() {
    // Update each player's visible grid
    for (int i = 0; i < numPlayers; i++) {
        if (players[i] != NULL) {
            // Calculate the visible grid for the player
            grid_t* visible_grid = calc_grid(main_grid, players[i]);
            // Calculate the visible grid for the player
            char* message = format_grid_message(visible_grid);
            message_send(get_player_address(players[i]), message);
            // Free allocated resources
            mem_free(message);
            grid_delete(visible_grid);
        }
    }

    // Update the spectator's grid if a spectator is present
    if (message_isAddr(spectator)) {
        // Format and send the full grid to the spectator
        char* full_message = format_grid_message(main_grid);
        message_send(spectator, full_message);
        // Free message
        mem_free(full_message);
    }

    // Check if the game should end
    if(totalGold == 0) {
        game_over();
    }
}

/**************** game_over ****************/
/* Ends the game and sends the final scores to all players and the spectator (if present).
 * Deletes all players, grids, and cleans up resources.
 */
void game_over() {
    char summary[1024];
    snprintf(summary, sizeof(summary), "QUIT GAME OVER:\n");
    // Sort players by score in descending order
    qsort(players, numPlayers, sizeof(player_t*), compare_players_by_score);
    // Append each player's summary to the game over message
    for (int i = 0; i < numPlayers; i++) {
        if (players[i] != NULL) {
            char line[1000];
            snprintf(line, sizeof(line), "%c %d %s\n",
                        get_player_letter(players[i]),
                        get_player_score(players[i]),
                        get_player_name(players[i]));
            strncat(summary, line, sizeof(summary) - strlen(summary) - 1);
        }
    }

    // Send the game over summary to all players
    for (int i = 0; i < numPlayers; i++) {
        if (players[i] != NULL) {
            message_send(get_player_address(players[i]), summary);
            player_delete(players[i]);
            players[i] = NULL;
        }
    }

    // Send the summary to the spectator, if present
    if (message_isAddr(spectator)) {
        message_send(spectator, summary);
    }

    // Clean up resources
    numPlayers = 0;
    grid_delete(main_grid);
    grid_delete(original_grid);
    message_done();
    // Signal that the game has ended
    flag = true;
}


/**************** find_player_at_position ****************/
/* Finds and returns the player located at the specified position.
 * If no player is at the position, returns NULL.
 */
player_t* find_player_at_position(pos_t* pos) {
   for (int i = 0; i < numPlayers; i++) {
       if (players[i] != NULL && position_equal(get_player_position(players[i]), pos)) {
           return players[i];
       }
   }
   return NULL;
}

/**************** send_spectator_gold_message ****************/
/* Sends a gold update message to the spectator, indicating
 * the total amount of uncollected gold remaining in the game.
 */
void send_spectator_gold_message(addr_t spectator) {
    // Allocate memory for the message
    char* message = (char*)mem_malloc(64 * sizeof(char));
    // Format the message with the total uncollected gold
    snprintf(message, 64, "GOLD 0 0 %d", totalGold);
    message_send(spectator, message);
    mem_free(message);
}

/**************** send_gold_message ****************/
/* Sends a gold update message to the specified player.
 * The message includes the collected gold, the player's purse,
 * and the total uncollected gold remaining in the game.
 * Updates the spectator with the remaining gold as well.
 */
void send_gold_message(player_t* player, int collected, int purse) {
    // Allocate memory for the message
    char* message = (char*)mem_malloc(64 * sizeof(char));
    // Format the message with collected, purse, and totalGold
    snprintf(message, 64, "GOLD %d %d %d", collected, purse, totalGold);
   message_send(get_player_address(player), message);
   mem_free(message);
   // Notify the spectator of the updated total gold (if present)
   if (message_isAddr(spectator)) {
       send_spectator_gold_message(spectator);
   }
}

/**************** compare_players_by_score ****************/
/* Comparator function for sorting players by their scores in descending order.
 * Used with qsort to rank players at the end of the game.
 */
int compare_players_by_score(const void* a, const void* b) {
   player_t* playerA = *(player_t**)a;
   player_t* playerB = *(player_t**)b;
   // Compare players by score (higher scores first)
   return get_player_score(playerB) - get_player_score(playerA);
}

/**************** add_player ****************/
/* Adds a new player to the game and initializes their structure.
 * The function checks if the maximum number of players is reached,
 * allocates memory for the new player, and sets their initial position on the grid.
 */
player_t*
add_player(char* name, addr_t* address, char letter)
{
   // Check if the maximum number of players has been reached
    if (numPlayers >= MaxPlayers) {
        fprintf(stderr, "Maximum number of players reached.\n");
        return NULL;
    }

    // Create a new player structure
    player_t* newPlayer = player_new(name, letter);
    if (newPlayer == NULL) {
        return NULL;
    }
    // Set the address of the player
    set_player_address(newPlayer, *address);

    // Get the grid dimensions
    int gridWidth = grid_get_width(main_grid);
    int gridHeight = grid_get_height(main_grid);

    // Generate a random starting position
    int x = rand() % gridWidth;
    int y = rand() % gridHeight;
    pos_t* new_pos = position_new(x, y);

    // Ensure the position is valid and unoccupied
    while (grid_get_symbol(main_grid, new_pos) != '.') {
        int x = rand() % gridWidth;
        int y = rand() % gridHeight;
        set_position_x(new_pos, x);
        set_position_y(new_pos, y);
    }
    
    // Set the player's position and update the grid
    set_player_position_values(newPlayer, new_pos);
    grid_set_symbol(main_grid, new_pos, letter);
    position_delete(new_pos);

    // Send a confirmation message to the player
    char letter_message[5];
    snprintf(letter_message, sizeof(letter_message),
                "OK %c", get_player_letter(newPlayer));
    message_send(get_player_address(newPlayer), letter_message);

    // Add the new player to the players array
    players[numPlayers++] = newPlayer;
    return newPlayer;
}

/**************** get_player_by_address ****************/
/* Finds a player by their address.
 * Searches through the list of players to find one 
 * whose address matches the provided address.
 */
player_t* get_player_by_address(addr_t* address) {
    for (int i = 0; i < numPlayers; i++) {
        if (players[i] != NULL && message_eqAddr(get_player_address(players[i]), *address)) {
            return players[i];
        }
    }
    return NULL;
}

/**************** setup_grid_with_gold ****************/
/* Places random piles of gold throughout the rooms in the grid.
 * Each pile is placed at a random location that meets specific criteria.
 * The total amount of gold is distributed randomly among the piles.
 */
void
setup_grid_with_gold(grid_t* grid)
{
    if (grid == NULL) {
        fprintf(stderr, "Grid was unable to be initialized.\n");
        return;
    }

    int goldTrack = GoldTotal;
    int gridWidth = grid_get_width(grid);
    int gridHeight = grid_get_height(grid);
    int** gold_grid = get_gold_grid(grid);
    // Generate random number of piles
    int numPiles = GoldMinNumPiles + rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1);

    int distribution[numPiles];
    for (int i = 0; i < numPiles; i++) {
        distribution[i] = 0;
    }

    // Distribute gold randomly among piles
    while (goldTrack > 0) {
        int randomIndex = rand() % numPiles;    // Pick a random pile
        distribution[randomIndex]++;            // Give it a nugget
        goldTrack--;                            // Reduce remaining nuggets
    }

    // Place the gold piles in valid room spots (`.`)
    for (int i = 0; i < numPiles; i++) {
        bool placed = false;
        pos_t* new_pos = position_new(-1, -1);
        while (!placed) {
            // Generate random coordinates within the grid
            int x = rand() % gridWidth;
            int y = rand() % gridHeight;
            set_position_x(new_pos, x);
            set_position_y(new_pos, y);
            // Check if the spot is a valid room and unoccupied
            if (grid_get_symbol(grid, new_pos) == '.' && gold_grid[y][x] == 0) {
                grid_set_gold(grid, new_pos, distribution[i]);  // Place the gold pile here in gold_grid
                grid_set_symbol(grid, new_pos, '*');
                placed = true;
            }
        }
        position_delete(new_pos);
    }
}

/**************** format_grid_message ****************/
/* Converts a 2D grid array into a formatted string suitable 
 * for sending to clients via message_send
 * The function constructs the message to represent the current 
 * grid layout, ensuring valid dimensions.
 */
char*
format_grid_message(grid_t* grid)
{
    if (grid == NULL) {
        fprintf(stderr, "Error: grid is NULL.\n");
        return NULL;
    }

    int height = grid_get_height(grid)-1;
    int width = grid_get_width(grid)-1;

    // Ensure the dimensions are valid
    if (height <= 0 || width <= 0) {
        fprintf(stderr, "Error: Invalid grid dimensions (width: %d, height: %d).\n", width, height);
        return NULL;
    }

        
    char** main_grid = get_main_grid(grid);
    if (main_grid == NULL) {
        fprintf(stderr, "Error: main_grid is NULL.\n");
        return NULL;
    }

    // Calculate buffer size and allocate memory for the formatted message
    int bufferSize = (width * height) + (height + 1) + strlen("DISPLAY\n") + 1;
    char* message = mem_malloc(bufferSize);
     // Initialize the buffer
    message[0] = '\0';
    strcat(message, "DISPLAY\n");

    // Construct the grid message
    for (int i = 0; i < height; i++) {
        if (main_grid[i] == NULL) {
            fprintf(stderr, "Error: Row %d in main_grid is NULL.\n", i);
            mem_free(message); // Free the allocated buffer
            return NULL;
        }
        // Append the current row and a newline
        strncat(message, main_grid[i], width);
        strcat(message, "\n");
    }
    return message;
}

/**************** handle_quit ****************/
/* Handles the process of a player or spectator quitting.
 * Sends a goodbye message and updates the game state accordingly.
 */
void
handle_quit(player_t* player, addr_t spectator, const addr_t* sender, bool isSpectator)
{
    // Determine the appropriate quit message
    const char* quit_message;
    if (isSpectator) {
        quit_message = "QUIT Thanks for watching!";
    } else {
        quit_message = "QUIT Thanks for playing!";
    }

    // Send the quit message
    message_send(*sender, quit_message);

    // Handle cleanup based on the sender type
    if (isSpectator) {
        // Reset the spectator address
        spectator = message_noAddr(); 
    }
    else {
        if (player != NULL) {
            // Restore the original grid symbol and invalidate player's position
            grid_set_symbol(main_grid, get_player_position(player), grid_get_symbol(original_grid, get_player_position(player)));
            set_position_x(get_player_position(player), -10);
            player = NULL;
            update_grid();
        }
    }
}


/**************** position_equal ****************/
/* Checks if two positions are equal by comparing their coordinates.
 * Returns true if both positions have the same x and y values.
 */
bool
position_equal(pos_t* pos1, pos_t* pos2){
   return get_position_x(pos1) == get_position_x(pos2) && get_position_y(pos1) == get_position_y(pos2);
}


/**************** sanitize_name ****************/
/* Sanitizes the player's name input.
 * Copies and truncates the input, replaces characters
 * per requirements spec, and validates the name.
 * Sends an error message if the name is invalid.
 */
bool
sanitize_name(const char* input_name, char* sanitized_name, addr_t from)
{
    if (input_name == NULL) {
        message_send(from, "QUIT Sorry - you must provide player's name.");
        return false;
    }

    // Copy and truncate the name if it exceeds MaxNameLength
    size_t len = strlen(input_name);
    if (len > MaxNameLength) {
        len = MaxNameLength;
    }
    strncpy(sanitized_name, input_name, len);
    sanitized_name[len] = '\0';

    // Replace invalid characters with underscores and validate name
    bool valid = false;
    for (size_t i = 0; i < len; i++) {
        if (isgraph((unsigned char)sanitized_name[i]) || isblank((unsigned char)sanitized_name[i])) {
            valid = true; // Name contains at least one valid character
        } else {
            sanitized_name[i] = '_'; // Replace invalid character
        }
    }

    // Reject empty or invalid names
    if (!valid) {
        message_send(from, "QUIT Sorry - you must provide a valid player's name.");
        return false;
    }
    return true;
}