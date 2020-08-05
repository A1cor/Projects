/*this module includes all game functions that use gurobi*/

#ifndef GAME_GUROBI_H_
#define GAME_GUROBI_H_

/*checks whether the board is solvable using gurobi ILP*/
int validate(Game *game);

/*receives cell index and finds a hint for it if exists*/
void hint(Game *game, int row,int column);

/*receives a threshold value and fills the game board with guesses with higher probability than the threshold*/
int guess(Game *game, float X);

/*receives a cell index and finds all possible valid values for it with probability higher than 0 for a solution*/
void guessHint(Game *game, int row, int column);

/*Generates a puzzle by randomly filling X empty cells with legal values,
running ILP to solve the board, and then clearing all but Y random cells.*/
int generate(Game *game, int X,int Y);

#endif /* GAME_GUROBI_H_ */
