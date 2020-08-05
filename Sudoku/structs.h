/*this module contains all the structs that are needed for the game and its board.*/

#ifndef STRUCTS_H_
#define STRUCTS_H_
#include "linkedList.h"
enum status{Init, Edit, Solve};
enum status game_status;
int mark_errors;

typedef struct{
	int invalid;
	int value;
	int fixed;
	} Cell;

typedef struct{
	int num_of_rows_in_block;
	int num_of_columns_in_block;
	int board_size;
	MoveList *move_history;
	Cell **board;
	} Game;

#endif /* STRUCTS_H_ */
