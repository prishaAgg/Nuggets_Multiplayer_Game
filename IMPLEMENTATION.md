# Nuggets
## Implementation Spec


According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

Muneeb: Vision module (including testing), significant contributions to server.c  
Neha: Grid module (including testing), structures module, significant contributions to server.c  
Prisha: server.c  

Debugging server.c, specs: Everyone!

---

## Server

### Data Structures

#### `player_t`
Represents a player in the game, including their name, position, score, and unique identifier.

```c
typedef struct player {
    char* name;         // Player's name
    char letter;        // Unique identifier for the player
    pos_t* position;    // Current position in the grid
    int score;          // Player's score
    addr_t address;     // Address for sending messages
} player_t;
```

#### `grid_t`
Represents the game grid, including the layout, dimensions, and gold placement.

```c
typedef struct grid {
    char** main_grid;  // The grid layout
    int** gold_grid;   // Gold placement on the grid
    int height;        // Grid height
    int width;         // Grid width
} grid_t;
```

#### Global Variables

```c
player_t* players[MaxPlayers];  // Array to hold player pointers
int numPlayers;                // Current number of players
grid_t* main_grid;             // The main game grid
grid_t* original_grid;         // Copy of the initial grid
addr_t spectator;              // Spectator address
int totalGold;                 // Remaining gold nuggets
```


### Function Prototypes with One-Liner Descriptions


#### **Core Functions**

```c
// Parses command-line arguments for map file and optional seed
int parseArgs(int argc, char* argv[], char** map_filename, int* seed);

// Initializes the game environment, including grid setup and gold placement
void initializeGame(char* map_filename, int seed);

// The main entry point of the server
int main(int argc, char* argv[]);

// Handles game termination and sends final scores to players
void gameOver();

// Handles incoming messages from players and the spectator
bool handleMessage(void* arg, const addr_t from, const char* message);

// Processes a player's movement based on a keystroke
void processKeystroke(char keystroke, player_t* player);

// Updates the grid for all players and the spectator
void updateGrid();
```

---

#### **Player Management Functions**

```c
// Adds a new player to the game
player_t* addPlayer(char* name, addr_t* address, char letter);

// Finds a player by their network address
player_t* getPlayerByAddress(addr_t* address);

// Finds the player located at a specific grid position
player_t* findPlayerAtPosition(pos_t* pos);
```

---

#### **Gold and Messaging Functions**

```c
// Randomly places gold piles in valid grid positions
void setup_grid_with_gold(grid_t* grid);

// Formats the current grid into a string for messaging
char* formatGridMessage(grid_t* grid);

// Sends a gold update message to the spectator
void sendSpectatorGoldMessage(addr_t spectator);

// Sends a gold update message to a specific player
void sendGoldMessage(player_t* player, int collected, int purse);
```

---

#### **Helper Functions**

```c
// Compares two players by their scores
int comparePlayersByScore(const void* a, const void* b);

// Handles player or spectator quitting the game
void handle_quit(player_t* player, addr_t spectator, const addr_t* sender, bool isSpectator);

// Compares two grid positions for equality
bool position_equal(pos_t* pos1, pos_t* pos2);

// Sanitizes and validates a player's name
bool sanitize_name(const char* input_name, char* sanitized_name, addr_t from);
```

---

### Detailed Pseudocode for Each Function

---

#### **`main`**
```c
main:
    Parse command-line arguments using `parseArgs`.
    If argument parsing fails:
        Exit with an error code.
    Initialize the game using `initializeGame`.
    Start the message loop using `message_loop` to handle player and spectator interactions.
```

---

#### **`parseArgs`**
```c
parseArgs:
    Validate the number of command-line arguments.
    If invalid:
        Print usage instructions and return an error code.
    Extract the map filename from arguments.
    Verify the map file exists using `stat`.
    If a seed is provided:
        Check that it is a valid non-negative integer.
        Convert the seed string to an integer.
    If no seed is provided:
        Default to 0.
    Return success.
```

---

#### **`initializeGame`**
```c
initializeGame:
    Seed the random number generator (using seed or process ID).
    Initialize the `players` array to NULL.
    Initialize the messaging system and retrieve the port.
    If messaging initialization fails:
        Print error and exit.
    Create the main game grid and a copy for reference.
    If grid creation fails:
        Print error and exit.
    Randomly place gold piles using `setup_grid_with_gold`.
```

---

#### **`gameOver`**
```c
gameOver:
    Create a summary string containing the final scores.
    Sort the players by their scores in descending order.
    For each player:
        Append their score and name to the summary string.
    Send the summary to all players and the spectator.
    Free all player resources and reset the game state.
    Delete the main and original grids.
    Terminate the messaging system.
```

---

#### **`handleMessage`**
```c
handleMessage:
    Check the type of the incoming message.
    If the message starts with "KEY":
        Process the keystroke using `processKeystroke`.
    If the message is "SPECTATE":
        Update the spectator and send initial grid and gold messages.
    If the message starts with "PLAY":
        Validate the player's name using `sanitize_name`.
        Add the player using `addPlayer`.
        Send the player a welcome message and update the grid.
    Handle quitting if necessary.
```

---

#### **`processKeystroke`**
```c
processKeystroke:
    Determine the direction of movement from the keystroke.
    Calculate the new position based on the player's current position and movement deltas.
    If the new position is invalid:
        Stop processing.
    If the new position contains another player:
        Swap the positions of the two players.
        Update the grid.
    If the new position contains gold:
        Add the gold to the player's score.
        Update the total gold remaining.
        Send gold messages to all players and the spectator.
    Update the grid.
```

---

#### **`updateGrid`**
```c
updateGrid:
    For each player:
        Calculate their visible grid using `calc_grid`.
        Format the grid using `formatGridMessage`.
        Send the formatted grid to the player.
    If there is a spectator:
        Format the main grid using `formatGridMessage`.
        Send the formatted grid to the spectator.
    If all gold has been collected:
        Call `gameOver`.
```

---

#### **`addPlayer`**
```c
addPlayer:
    If the maximum number of players has been reached:
        Print an error and return NULL.
    Create a new player object.
    Assign the player a unique letter.
    Randomly place the player in a valid grid position.
    Update the grid with the player's position.
    Send a confirmation message to the player.
    Add the player to the `players` array.
    Return the new player object.
```

---

#### **`getPlayerByAddress`**
```c
getPlayerByAddress:
    For each player in the `players` array:
        If the player's address matches the provided address:
            Return the player.
    Return NULL if no match is found.
```

---

#### **`findPlayerAtPosition`**
```c
findPlayerAtPosition:
    For each player in the `players` array:
        If the player's position matches the provided position:
            Return the player.
    Return NULL if no match is found.
```

---

#### **`setup_grid_with_gold`**
```c
setup_grid_with_gold:
    Randomly determine the number of gold piles within the allowed range.
    Distribute the total gold among the piles.
    For each pile:
        Randomly place it in a valid room spot on the grid.
        Update the grid's gold array and symbol.
```

---

#### **`formatGridMessage`**
```c
formatGridMessage:
    Calculate the dimensions of the grid.
    Allocate a buffer for the formatted message.
    Append each row of the grid to the buffer, followed by a newline.
    Prefix the message with "DISPLAY\n".
    Return the formatted message.
```

---

#### **`sendSpectatorGoldMessage`**
```c
sendSpectatorGoldMessage:
    Format a gold message with the total remaining gold.
    Send the message to the spectator.
```

---

#### **`sendGoldMessage`**
```c
sendGoldMessage:
    Format a gold message with the collected, purse, and total gold.
    Send the message to the player.
    If there is a spectator:
        Call `sendSpectatorGoldMessage`.
```

---

#### **`comparePlayersByScore`**
```c
comparePlayersByScore:
    Retrieve the scores of the two players.
    Return the difference in scores (descending order).
```

---

#### **`handle_quit`**
```c
handle_quit:
    Determine whether the sender is a player or spectator.
    Send a quit message to the sender.
    If the sender is a player:
        Free their resources and update the grid.
    If the sender is the spectator:
        Reset the spectator address.
```

---

#### **`position_equal`**
```c
position_equal:
    Compare the x and y coordinates of the two positions.
    Return true if they are equal; otherwise, return false.
```

---

#### **`sanitize_name`**
```c
sanitize_name:
    Check if the input name is NULL.
    Copy and truncate the input name to the maximum allowed length.
    Replace invalid characters with underscores.
    Check if the sanitized name contains at least one valid character.
    If the name is invalid:
        Send an error message and return false.
    Return true if the name is valid.
```

--- 


## Vision Module

### Data Structures

#### `line_t`
Represents a mathematical line with a slope and intercept, used for visibility checks.

```c
typedef struct line {
    double slope;      // Slope of the line
    double intercept;  // Y-intercept of the line
} line_t;
```

### Definition of Function Prototypes

#### Core Functions

- **`calc_grid`**: Calculates the grid from a player's perspective, marking visible areas.
  ```c
  grid_t* calc_grid(grid_t* main_grid, player_t* player);
  ```

- **`check_visible`**: Checks if a position on the grid is visible from another position.
  ```c
  bool check_visible(grid_t* main_grid, pos_t* pos1, pos_t* pos2);
  ```

- **`calc_line`**: Calculates the line equation between two positions.
  ```c
  line_t* calc_line(pos_t* pos1, pos_t* pos2);
  ```

- **`is_inside_vert`**: Determines if a position is inside a room when moving vertically.
  ```c
  bool is_inside_vert(grid_t* main_grid, pos_t* pos);
  ```

- **`is_inside_horiz`**: Determines if a position is inside a room when moving horizontally.
  ```c
  bool is_inside_horiz(grid_t* main_grid, pos_t* pos);
  ```

---

### Detailed Pseudocode

#### `calc_grid`

    get player's current position
    get player's previously viewed set
    create a blank grid for the player's perspective

    for each position (x, y) in the main grid
        create a temporary position object
        if the position is the player's position
            mark it as '@' in the perspective grid
            delete the temporary position
            continue
		
		if the position is more than radius away from the player's position
			delete the temporary position
            continue

        create a key for the position
        if the position is already in the viewed set
            copy the symbol from the main grid to the perspective grid
            delete the temporary position
        else if the position is visible from the player's position
            add the position to the viewed set
            copy the symbol from the main grid to the perspective grid
        else
            delete the temporary position
        free the key

    return the player's perspective grid

#### `check_visible`

    calculate the line equation between pos1 and pos2
    if pos2 is a wall and the line slope is not 0 or infinite
        delete the line and return false

    if pos1 and pos2 have different x-coordinates
        determine the left and right positions
        for each x between left and right
            calculate the y-coordinate using the line equation
            create a position object for (x, y)
            if not inside a valid vertical room
                delete the position and the line, return false
            delete the position

    if pos1 and pos2 have different y-coordinates
        determine the up and down positions
        for each y between up and down
            calculate the x-coordinate using the line equation
            create a position object for (x, y)
            if not inside a valid horizontal room
                delete the position and the line, return false
            delete the position

    delete the line and return true

#### `calc_line`

    calculate the slope (m) using the difference in y and x coordinates
    calculate the y-intercept (c) using the slope and one position
    create a new line object and set its slope and intercept
    return the line object

#### `is_inside_vert`

    if the y-coordinate is an integer
        return whether the position is inside a room

    calculate the ceiling and floor of the y-coordinate
    create positions for the ceiling and floor
    check if either position is inside a room
    delete the positions and return the result

#### `is_inside_horiz`

    if the x-coordinate is an integer
        return whether the position is inside a room

    calculate the ceiling and floor of the x-coordinate
    create positions for the ceiling and floor
    check if either position is inside a room
    delete the positions and return the result

---


---

## Grid Module

### Data Structures

#### `grid_t`
Represents a 2D grid structure, which includes a main grid for the map and a separate grid for tracking gold placement.

```c
typedef struct grid {
    char** main_grid;  // 2D array representing the map layout
    int** gold_grid;   // 2D array tracking gold placement
    int height;        // Number of rows in the grid
    int width;         // Number of columns in the grid
} grid_t;
```

### Definition of Function Prototypes

#### Core Functions

- **`grid_new`**: Creates and initializes a new grid from a given map file.
  ```c
  grid_t* grid_new(char* mapFile);
  ```

- **`grid_delete`**: Frees the memory associated with a grid.
  ```c
  void grid_delete(grid_t* grid);
  ```

- **`grid_set_symbol`**: Sets a symbol at a specific position in the main grid.
  ```c
  void grid_set_symbol(grid_t* grid, pos_t* pos, char c);
  ```

- **`grid_get_symbol`**: Gets the symbol at a specific position in the main grid.
  ```c
  char grid_get_symbol(grid_t* grid, pos_t* pos);
  ```

- **`grid_set_gold`**: Places a specific amount of gold at a given position.
  ```c
  void grid_set_gold(grid_t* grid, pos_t* pos, int n);
  ```

- **`grid_get_gold`**: Retrieves the amount of gold at a given position.
  ```c
  int grid_get_gold(grid_t* grid, pos_t* pos);
  ```

- **`grid_remove_gold`**: Removes all gold from a specific position.
  ```c
  void grid_remove_gold(grid_t* grid, pos_t* pos);
  ```

#### Utility Functions

- **`grid_get_width`**: Returns the grid's width.
  ```c
  int grid_get_width(grid_t* grid);
  ```

- **`grid_get_height`**: Returns the grid's height.
  ```c
  int grid_get_height(grid_t* grid);
  ```

- **`grid_valid_position`**: Checks if a position is valid for movement.
  ```c
  bool grid_valid_position(grid_t* grid, pos_t* pos);
  ```

- **`grid_is_inside`**: Checks if a position is inside the grid boundaries.
  ```c
  bool grid_is_inside(grid_t* grid, pos_t* pos);
  ```

- **`grid_in_room`**: Checks if a position is inside a valid room.
  ```c
  bool grid_in_room(grid_t* grid, pos_t* pos);
  ```

- **`grid_new_blank_array`**: Creates a blank 2D array for the grid.
  ```c
  char** grid_new_blank_array(int width, int height);
  ```

- **`make_grid_blank`**: Creates a blank grid structure.
  ```c
  grid_t* make_grid_blank(int width, int height);
  ```

- **`grid_print`**: Prints the grid to the console for debugging.
  ```c
  void grid_print(grid_t* grid);
  ```

---

### Detailed Pseudocode

#### `grid_new`

    open the map file for reading
    if the file cannot be opened
        return NULL

    determine the grid dimensions by reading the file line by line
    reset the file pointer to the beginning of the file

    create a blank 2D array for the main grid
    read characters from the file and populate the main grid array
    create a 2D array for the gold grid and initialize all elements to 0

    return a pointer to the newly created grid

#### `grid_delete`

    if the grid is NULL
        return

    free all rows of the main grid
    free the main grid array

    free all rows of the gold grid
    free the gold grid array

    free the grid structure itself

#### `grid_set_symbol`

    if the grid or position is NULL
        return

    if the position is not inside the grid
        return

    set the symbol in the main grid at the given position

#### `grid_get_symbol`

    if the grid or position is NULL
        return '\0'

    if the position is not inside the grid
        return '\0'

    return the symbol in the main grid at the given position

#### `grid_set_gold`

    if the grid, position, or gold amount is invalid
        return

    if the position is not inside the grid
        return

    set the gold amount in the gold grid at the given position

#### `grid_get_gold`

    if the grid or position is NULL
        return 0

    if the position is not inside the grid
        return 0

    return the gold amount in the gold grid at the given position

#### `grid_remove_gold`

    set the gold amount at the given position to 0

#### `grid_valid_position`

    if the position is inside the grid
        get the symbol at the position
        if the symbol is not valid for movement
            return false

    return true

#### `grid_is_inside`

    check if the position's coordinates are within the grid's boundaries
    return true if valid, false otherwise

#### `grid_in_room`

    if the position is inside the grid
        get the symbol at the position
        if the symbol represents a room or open space
            return true

    return false

#### `grid_new_blank_array`

    allocate memory for the 2D array
    for each row
        allocate memory for the row
        initialize all elements to a default value
    return the 2D array

#### `make_grid_blank`

    create a new grid structure
    allocate a blank main grid
    initialize the grid's dimensions
    return the grid

#### `grid_print`

    for each row in the grid
        for each column in the row
            print the symbol at the position

    print a newline at the end of each row

---


---

## Structures Module

### Data Structures

#### `pos_t`
Represents a position on the grid using `x` and `y` coordinates.
```c
typedef struct position {
    double x;  // X-coordinate
    double y;  // Y-coordinate
} pos_t;
```

#### `line_t`
Represents a line in a 2D space using slope (`m`) and y-intercept (`c`).
```c
typedef struct line {
    double m;  // Slope
    double c;  // Y-intercept
} line_t;
```

#### `player_t`
Represents a player in the game.
```c
typedef struct player {
    char* name;       // Player's name
    addr_t address;   // Address of the player
    pos_t* position;  // Position of the player
    char letter;      // Unique letter assigned to the player
    int score;        // Player's score
    set_t* viewed;    // Set of positions the player has viewed
} player_t;
```

---

### Definition of Function Prototypes

#### Core Functions

- **`player_new`**: Creates a new player with a name and a unique letter.
  ```c
  player_t* player_new(char* name, char letter);
  ```

- **`position_new`**: Allocates and initializes a new position with given `x` and `y` coordinates.
  ```c
  pos_t* position_new(double x, double y);
  ```

- **`line_new`**: Creates a new line structure with default values for slope and intercept.
  ```c
  line_t* line_new(void);
  ```

- **`player_delete`**: Frees memory associated with a player.
  ```c
  void player_delete(player_t* player);
  ```

- **`position_delete`**: Frees memory associated with a position.
  ```c
  void position_delete(pos_t* position);
  ```

- **`line_delete`**: Frees memory associated with a line.
  ```c
  void line_delete(line_t* line);
  ```

- **`create_key`**: Creates a unique string key for a position based on its coordinates.
  ```c
  char* create_key(pos_t* pos);
  ```

#### Getter Functions

- Functions to retrieve fields from `player_t`, `pos_t`, and `line_t`:
  ```c
  char* get_player_name(player_t* player);
  addr_t get_player_address(player_t* player);
  pos_t* get_player_position(player_t* player);
  char get_player_letter(player_t* player);
  int get_player_score(player_t* player);
  set_t* get_player_viewed(player_t* player);
  double get_position_x(pos_t* pos);
  double get_position_y(pos_t* pos);
  double get_line_slope(line_t* line);
  double get_line_intercept(line_t* line);
  ```

#### Setter Functions

- Functions to update fields in `player_t`, `pos_t`, and `line_t`:
  ```c
  void set_player_name(player_t* player, char* name_new);
  void set_player_address(player_t* player, addr_t address_new);
  void set_player_position(player_t* player, pos_t* position_new);
  void set_player_letter(player_t* player, char letter_new);
  void set_player_score(player_t* player, int score_new);
  void set_player_viewed(player_t* player, set_t* viewed_new);
  void set_position_x(pos_t* pos, double x_new);
  void set_position_y(pos_t* pos, double y_new);
  void set_line_slope(line_t* line, double m_new);
  void set_line_intercept(line_t* line, double c_new);
  ```

---

### Detailed Pseudocode

#### `player_new`

    if name or letter is invalid
        return NULL

    allocate memory for a player structure
    if memory allocation fails
        return NULL

    initialize the player's fields:
        name, letter, position (-1, -1), score (0), viewed (new set)

    return the newly created player

#### `position_new`

    allocate memory for a position structure
    if memory allocation fails
        return NULL

    set x and y coordinates
    return the position

#### `line_new`

    allocate memory for a line structure
    if memory allocation fails
        return NULL

    initialize slope (m) and intercept (c) to 0
    return the line

#### `player_delete`

    if player is NULL
        return

    free memory associated with:
        player's position
        player's viewed set (freeing all keys/values)
        the player structure itself

#### `position_delete`

    if position is NULL
        return

    free the memory associated with the position

#### `line_delete`

    if line is NULL
        return

    free the memory associated with the line

#### `create_key`

    if position is NULL
        return NULL

    allocate memory for a string key
    if memory allocation fails
        print an error and exit

    format the string as "x_y" using the position's coordinates
    return the key

#### Getter Functions

- **`get_player_name`**: Return the player's name.
- **`get_position_x`**: Return the `x` coordinate of the position.
- Similar functions for other fields in `player_t`, `pos_t`, and `line_t`.

#### Setter Functions

- **`set_player_name`**: Update the player's name if valid.
- **`set_position_x`**: Update the `x` coordinate of the position.
- Similar functions for other fields in `player_t`, `pos_t`, and `line_t`.

---

## Testing plan

### unit testing

### **Unit Testing: Grid Module**

The `gridTest` program is designed to test the core functionality of the `grid` module to ensure it operates as expected. It validates the following:

1. **Grid Initialization**:
   - The program initializes the grid using a map file (`../maps/main.txt`) with the function `grid_new`.
   - It tests the module's ability to read and parse the map file into a grid structure.

2. **Grid Printing**:
   - The program calls `grid_print` to verify that the grid is correctly constructed and matches the structure defined in the map file.
   - The printed output should visually match the map's layout.

3. **Memory Deallocation**:
   - After grid initialization and printing, the program calls `grid_delete` to test that all dynamically allocated memory is properly freed.
   - Running tools like `valgrind` ensures no memory leaks are present during grid initialization, usage, or deallocation.

This basic test ensures that the grid module can read map files, accurately represent them in memory, print the grid, and clean up allocated resources. It is a foundational step in verifying the module before integrating it into the larger system.


### **Unit Testing: Vision Module**

#### **1. `test_calc_line`**
- **Purpose**: Verifies the `calc_line` function correctly calculates the slope (`m`) and intercept (`c`) of a line between two positions.
- **Description**:
  - Two `pos_t` positions are initialized.
  - The line is calculated using these positions.
  - Slope and intercept are printed for validation.
  - Assertions can be added to check expected values of slope and intercept.

---

#### **2. `test_is_inside_vert`**
- **Purpose**: Tests whether a position is vertically inside a room in the grid.
- **Description**:
  - A grid is initialized from a map file.
  - Two positions are created: one inside a room and one outside.
  - The `is_inside_vert` function is tested with assertions for true (inside) and false (outside) cases.
  - Memory is cleaned up after testing.

---

#### **3. `test_is_inside_horiz`**
- **Purpose**: Tests whether a position is horizontally inside a room in the grid.
- **Description**:
  - Similar to `test_is_inside_vert`, but evaluates horizontal boundaries.
  - Two positions (inside and outside a room) are tested with the `is_inside_horiz` function.
  - Assertions verify correct behavior.
  - Memory is cleaned up after testing.

---

#### **4. `test_check_visible`**
- **Purpose**: Ensures that the `check_visible` function correctly determines if two positions are visible to each other in the grid.
- **Description**:
  - A grid is initialized, and positions are created.
  - `check_visible` is used to test visibility between positions.
  - Assertions verify that:
    - Two positions in the same room are visible (`true`).
    - Positions in different rooms are not visible (`false`).
  - The grid is cleaned up after testing.

---

#### **5. `test_calc_grid`**
- **Purpose**: Validates the `calc_grid` function, which calculates a player's visible perspective of the grid.
- **Description**:
  - A main grid and a player are initialized.
  - The player's position is updated to test visibility.
  - The perspective grid is printed for visual verification.
  - Simulated movement (using `w`, `a`, `s`, `d` inputs) is tested:
    - The player moves around the grid.
    - The perspective grid updates accordingly.
    - Assertions check that the player's position (`@`) is correctly marked.
  - Memory is cleaned up after testing.

---

### integration testing


1. **Server Initialization**:
   - Start the server with different map files and seeds, including:
     - A valid map file and seed.
     - An invalid map file (non-existent or incorrect format).
     - A missing or invalid seed (e.g., negative numbers, non-numeric input).
   - Verify that the server initializes correctly with valid inputs and gracefully handles invalid inputs by displaying appropriate error messages.

2. **Client Connections**:
   - Use the compiled client to:
     - Connect as a spectator  and verify the initial game state is displayed.
     - Join the game as a player and check if the server assigns a unique identifier and sends game dimensions.

3. **Command Handling**:
   - Send a variety of valid commands through the client to test server responses:
     - **Movement Commands**: Test `KEY <direction>` inputs (e.g., `KEY h`, `KEY l`) and verify if the player's position updates correctly.
     - **Quit Command**: Send `Q` to ensure the server removes the player or spectator and updates the game state.
     - **Gold Collection**: Move the player to gold-containing positions and check the gold count updates.

4. **Invalid Inputs**:
   - Send invalid commands, including:
     - Commands with incorrect syntax (e.g., `e`).
     - Exceeding player limits by attempting to join when the game is full.
     - Joining with an invalid name (e.g., empty or containing non-graphical characters).

5. **Stress Testing**:
   - Simulate multiple client connections to ensure the server handles concurrent players and spectators without crashing or lagging.

6. **Game Events**:
   - Test how the server handles major game events:
     - Players quitting and rejoining.
     - Collecting all gold to trigger the game-over condition.

7. **Server Shutdown**:
   - Terminate the server gracefully (exit(0)) and check for resource deallocation or memory leaks.

#### **Expected Outcomes**
- The server processes valid commands correctly and updates the game state for all connected clients.
- Invalid inputs are handled gracefully with appropriate error messages.
- The server remains stable under stress with multiple clients.
- The game state synchronizes consistently between the server and clients.

---

## Limitations

a number is not displayed after "GOLD received" when using narrow.txt (even with the given, functional server binary file in the shared directory). However, it works as expected in other maps.