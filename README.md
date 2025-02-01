# Nuggets

## Overview
This repository contains the code for the "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

### Subdirectory Structure
- `server/`: Contains the main server implementation that integrates all modules.
- `grid/`: Implements the grid module for creating and managing game maps.
- `vision/`: Implements the vision module to calculate player visibility in the grid.
- `structures/`: Defines data structures used throughout the project.
- `libcs50/`: Contains reusable library functions for sets, memory management, and file handling.
- `support/`: Contains support functions for messaging and communication between clients and the server.

## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps.