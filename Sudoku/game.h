/*This module is responsible for the functions related to the game board.*/
#ifndef GAME_H_
#define GAME_H_
#include "structs.h"

/*Creates an empty game board*/
void initializeEmptyBoard(Game *game);

/*autofills all cells which have only one possible value.
 * returns 1 if autofill was successful and 0 otherwise */
int autoFillBoard(Game *game);

/*prints the game board*/
void printBoard(Game *game);

/*the function saves the current game to the save directory provided to it*/
void save(Game *game, char* save_dir);

/*checks if all board game values are valid.
 * return 1 if valid and 0 otherwise.*/
int boardValueAreValid(Game *game);

/*frees the game object and all related memory to it*/
void freeGame(Game *game);

/*initializes an empty cell*/
void initializeEmptyCell(Cell *c);

/*receives the cell index, desired new value and start = {0,1} (start == 1 -> first move of a chain of moves).
 *updates the cell if all values are valid and the game isn't in Init mode
 *returns 1	if successful and 0 otherwise*/
int setCell(int x, int y, int z, Game *game, int start);

/*undo move*/
int undoMove(Game *game);

/*redo move*/
int redoMove(Game *game);

/*resets the board to it's first state according to it's move history*/
void resetBoard(Game *game);

/*sets the invalid field of all invalid cells in the board to 1 (true)*/
void markInvalidCells(Game *game);

/*checks if all fixed cells are valid in relation to all the other fixed cells,
 * returns 1 if true and 0 otherwise*/
int checkFixedCells(Game *game);

/*turns all cells with values to fixed cells*/
void fixCellsWithValues(Game *game);

/*imports a game from a file in Solve mode.*/
Game* solve(Game *cur_game, char* fileName);

/*imports a game from a file in Edit mode, if the file name provided is an empty string,
 * initializes a new game with 3x3 blocks board.*/
Game* edit(Game *cur_game, char* fileName);

/*exits the game cleanly, calls the freeGame function to clear the current game and
 * all the memory related to it*/
void exitGame(Game *game);

/*sets the mar_errors field to 1 or 0 indicating whether to mark errors or not
 * when printing the game board.*/
void markErrors(int mark);

/*receives the desired number of rows and columns in a block and initializes a new game*/
Game* initializeGame(int num_of_rows_in_block, int num_of_columns_in_block);

/*checks whether a cell is valid and returns 1 or 0 otherwise*/
int invalidCell(Game *game, int row , int column, int ignore_not_fixed);

/*returns the number of empty cells in the board*/
int numberOfEmptyCell(Game *game);

/*copies one game to another*/
void copyGame(Game *game, Game *copy_game);

/*check if the input array has only one valid option,
 * returns 1 if true and 0 otherwise*/
int oneValidOption(int *arr,Game *game);

/*returns 1 if game board is solved and 0 otherwise*/
int isWin(Game *game);

/*changes the game mode to Init*/
void changeToInit();
#endif /* GAME_H_ */
