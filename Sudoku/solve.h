/*This module is responsible for the functions related to solving a game board.*/


#ifndef SOLVE_H_
#define SOLVE_H_
#include "game.h"

/*returns the number of possible solutions for the game using a stack*/
int countSolutions(Game *game);

/*receives a cell index and fills it's array with its possible options */
void fillArrWithOption(int *arr_of_options,Game *game,int row,int column);

/*receives an array and return 1 if not empty and 0 otherwise*/
int stillHasOptionForCell(int *arr_of_options, Game *game);

#endif /* SOLVER_H_ */


