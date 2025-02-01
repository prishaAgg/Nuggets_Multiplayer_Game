## Design Spec


  

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server. We only deal with the client, and helper modules `grid` and `vision`.

## Server

### User interface

 The indexer's only interface with the user is on the command-line; it has one required argument, the map, and one optional argument, a  seed for randomization.

```bash
./server map.txt [seed]
```

### Inputs and outputs

*Input:* 
The client program sends its playername (if it has one) to the server. Additional input comes from the client during the game and is described below:

| keystroke | Action                                      |
| --------- | ------------------------------------------- |
| `Q`       | quit the game                               |
| `h`       | move left, if possible                      |
| `l`       | move right, if possible                     |
| `j`       | move down, if possible                      |
| `k`       | move up, if possible                        |
| `y`       | move diagonally up and left, if possible    |
| `u`       | move diagonally up and right, if possible   |
| `b`       | move diagonally down and left, if possible  |
| `n`       | move diagonally down and right, if possible |


*Output:* 
Per the requirements spec, the server will output a grid for every client that is updated accordingly, along with the game status (number of nuggets claimed and unclaimed, how much gold has just been received, and whether an unknown keystroke has been pressed).

At the end of the game, the server will output the game-over summary: "GAME OVER:" followed by a simple textual table with one row for every player (including any who quit early) and with three columns: player letter, player purse, and player name. The list may be sorted by player letter, or by score, or unsorted.

Additionally, the server will log useful information (all the inputs coming in from each client, the grid that is sent to the spectator as it is updated, and the scores of each player as that gets updated) that can be saved in a logfile.

### Functional decomposition into modules

The `server` program handles the core game operations by setting up the environment, handling interactions and updates, and coordinating the game’s flow to ensure players and spectators receive relevant updates, ending in an end-of-game summary when conditions are met.

We anticipate the following functions within it:

1. *main*, which calls the major functions
2. *initializeGame()*, which initializes the game
3. *message_loop()*, which is the main loop that handles all incoming messages and client connections
4. *processKeystroke(char keystroke, Player player)*, which handles player keystrokes to manage movement and actions
5. *updateGrid()*, which updates the grid after every movement, gold collection, or player join/quits
6. *gameOver()*, which communicates the final state to all clients

Apart from the pre-built *support* library, We will also make use of some helper modules that provide data structures.

8. `grid`, which defines the grid functionality

9. `vision`, which calculates and tracks the visible grid for each client/spectator


### Pseudo code for logic/algorithmic flow

#### `main`
The main function does contain much as most functionality is divided amongst other functions

Pseudocode:
```plain text
initializeGame()
message_loop()
```

#### `initializeGame()`

Initializes the game, reading the map file and setting up the grid, placing gold piles, and preparing to accept clients.

-   Parse the command-line arguments.
    
-   Seed the random number generator for gold placement.
    
-   Load and validate the map file to initialize the grid.
    
-   Randomly place between `GoldMinNumPiles` and `GoldMaxNumPiles` gold piles within the grid.

Pseudocode:
```plaintext

parse command line arguments

load map from file

seed random generator if seed is provided

set up grid with gold piles

initialize network, prepare for connections through `message_init()`
```
    
#### `message_loop()`

This is the main loop that handles all incoming messages and client connections.

-   Accept connections up to MaxPlayers players or a single spectator.
    
-   Receive and parse messages from clients, including keystrokes for movement.
    
-   Route valid messages to corresponding functions (like movement functions).
    
-   Log all inputs and update client states

Pseudocode:

```plaintext
while game is active

accept new connections up to MaxPlayers and one spectator

receive messages from clients

for each message:

if message is valid keystroke

	call processKeystroke() with the keystroke and player

else

	ignore (log if logging)

if game over conditions are met:

	call gameOver()
```
#### `processKeystroke(keystroke, player)` 

Handles player keystrokes to manage movement and actions based on grid constraints.

-   Validate keystroke and update player position if the movement is valid (like spot is empty or contains gold).
    
-   If movement is invalid (like blocked by a wall), ignore or send an optional error message to the player.  

Pseudocode:
```plaintext
Waits until keystroke

if movement is valid:

	call update_grid with keystroke and player name

else:

	ignore or send error message
```

#### `updateGrid()`

Updates the grid after every movement, gold collection, or player join/quit.

-   Adjust grid cells to reflect current player positions, gold status, and other changes.
-  If the player collects gold, update their purse and broadcast the new grid state.
    
-   Notify all players with their updated view that is calculated by the vision module.
    
-   So will use `message_send()` to inform each client of their new status

```plain text
  for each player in game
    if keystroke is not Q
      get player’s current position
      get symbol at player’s position from grid

      if player’s position contains gold
        collect gold and update player’s purse
        remove gold from grid
        update symbol at previous position to '.'
        update symbol at current position to player's letter
      // Calculate player’s view using the vision module
      visible_grid = calc_grid(main_grid, player.name)

      // Format and send the updated grid for this player
      format message with player’s visible grid
      send message to player
  else
	  get the player's current position
	  update symbol at position to '.'
  if spectator exists
    get the full main grid for complete visibility
    format message with full grid
    send message to spectator
```

#### `gameOver()`

Communicates the final state to all clients.

-   Generate the game-over summary (acc. to requirement spec).
    
-   Broadcast the QUIT message to all clients with the summary.
    
-   Print the summary to log file, then shut down the server.
    
-   Close messaging system

``` plain text
broadcast QUIT message with summary
print summary and shut down server
```

### Major data structures

-   *Player:* Struct that has attributes like `name`, `letter`, `position`, `visibility status`, `collection`.
    
-   *main_grid:* (Described in data structures of `grid` module).
    
-   *gold_grid:* (Described in data structures of `grid` module) 

## Grid module

The `grid` module is responsible for managing the in-memory representation of the game's map as specified by `map.txt`. It uses a 2D array structure to store symbols that represent different elements of the game, such as players, room layout, and piles of gold. The module provides functions to update, query, and manage the grid, ensuring that game state changes are accurately reflected as gameplay progresses.  

### Functional decomposition

1. `new_grid`, which initializes the 2D array grid given the map.txt file and sets up the initial game state

2. `place_gold`, which places a pile of gold with a specified amount at a given (x, y) location in the grid

3. `update_grid_symbol`, which updates a specific cell in the grid with a given symbol (e.g. new player, a door, a wall)

4. `remove_gold`, which clears or marks a location as empty after the gold is collected

5. `get_gold_amount`, which returns the amount of gold at a specific location in the grid

6. `get_symbol`, which returns the symbol at a specific location in the grid

7. `get_main_grid`, which returns the current state of the main grid

8. `get_gold_grid`, which returns the current state of the gold grid

  

### Pseudo code for logic/algorithmic flow

#### `new_grid`

```plain text
initialize a 2D array of type char* (`main_grid`)

load the array with data from `map.txt` (assigning the proper symbol to each slot)

initialize another 2D array of type int (`gold_grid`) to track the amount of gold at each location
```
#### `place_gold`

  ```plain text

call `update_grid_symbol` to change the symbol to indicate a gold pile

go to the appropriate location in `gold_grid` and update number of nuggets
```
  
#### `update_grid_symbol`

```plain text
go to the appropriate location in `main_grid` and update symbol
```

#### `remove_gold`

```plain text
call `place_gold` to set location to having 0 nuggets

call `update_grid_symbol` to update symbol to have no gold pile
```
  
#### `get_gold_amount`

```plain text
return value at specified location in `gold_grid`
```

#### `get_symbol`

  ```plain text
return value at specified location in `main_grid`
```
  
#### `get_main_grid`

```plain text
return `main_grid`
```
  
#### `get_gold_grid`

```plain text
return `gold_grid`
```
 
### Major data structures

Main grid (`main_grid`): A 2D array of characters representing the map layout as specified by `map.txt`. Each cell contains a symbol representing empty space, gold, players, and room detailing (door, hallway, wall).

Gold grid (`gold_grid`): A 2D array of integers where 0 indicates no gold pile, and any value greater than 0 represents the number of gold nuggets at that location.

## Vision module

  The `vision` module calculates and tracks the visible grid for each player. It exports one function, *calc_grid*, which takes the entire main grid and the player name, and outputs the main grid that should be displayed for the player. It also sets a range limit on vision, a radius of five spots (EXTRA CREDIT).

### Functional decomposition


1.  `calc_grid`, the only exported function that actually calculates the actual main grid viewed by a player

2. `is_inside`, which determines whether a given position (the y coordinate is always an integer, the x coordinate may not) is inside the room

3. `check_visible`, which checks whether two points are visible from each other

4. `calc_line`, which calculates the linear equation of the line passing through two inputted points. The line between the points cannot be horizontal or vertical.
  

### Pseudo code for logic/algorithmic flow

#### `calc_grid`

```plain text
create a new grid

loops over all position in the grid except for the player's position

	if the position is in visited:
	set the character value of the position in the new grid to that of the position in the inputted grid
	continue
	
	if check_visible(player position, the position):
		add the position to visited
		set the character value of the position in the new grid to that of the position in the inputted grid

return the new grid
```
  
  #### `is_inside`
  
  ``` plain text
if x is an integer
	return whether the character at inputted position is . or @ or a capital letter

return whether one of ((floor(x),y)) and ((ceiling(x), y)) are not inside
```

#### `check visible`

```plain text
if both positions have the same y coordinates
	loop over all the points between them
		if the point is outside
			return false
	return true

if both positions have the same x coordinates
	loop over all the points between them
		if the point is outside
			return false
	return true
	
slope = first element of calc_line(position1, position2)
y-intersect = second element of calc_line(position1, position2)
	
loop over all the integer y coordinates between the two positions' y coordinates
	calculate the point of intersection using the slope and y-intersect
	if the point of intersection is outside
		return false

return true
```
#### `calc_line`
```plain text
slope = (y2 - y1)/(x2 - x1)
y-intersect = y1 - (m * x1)

return (slope, y-intersect)
	
```  
### Major data structures

`visited`: a hashtable of bags. The keys of the hashtable are the player names and the items in the bags are positions. This data structure helps keep track of the map that has been seen by each player.