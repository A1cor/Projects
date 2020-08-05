/*This module is responsible for the functions related to the game board.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "linkedList.h"
#include "fileParsing.h"
#include "solve.h"


/*prints a cell*/
void printCell(Cell *c){
	if(c->value!=0){
		int value= c->value;
		if(c->fixed==1){
			printf(" %2d.",value);
		}
		else{
			if((c->invalid==1) && (mark_errors==1 || game_status==Edit) ){
				printf(" %2d*",value);
			}
			else{
				printf(" %2d ",value);
			}
		}
	}
	else{
		printf("    ");
	}
}
/*print ----------- 4N+m+1 times*/
void printRowSepration(Game *game){
	int i=1;
	for(i=1;i<=(4*game->board_size)+game->num_of_rows_in_block+1;i++){
		printf("-");
	}
	printf("\n");
}

/*prints part of the game board*/
void printBlockRow(int i, Game *game){
	int j=1,k=0;
	printRowSepration(game);
	for(k=i;k<i+game->num_of_rows_in_block;k++){
		for(j=1;j<=game->board_size;j++){
			if(j%game->num_of_columns_in_block==1 ||game->num_of_columns_in_block==1){
				printf("|");
			}
			printCell(&(game->board[k][j]));
		}
		printf("|\n");
	}
}

/*prints the game board*/
void printBoard(Game *game){
	int k;
	for(k=0;k<game->num_of_columns_in_block;k++){
		printBlockRow(k*game->num_of_rows_in_block +1, game);
	}
	printRowSepration(game);
}

/*initializes an empty cell*/
void initializeEmptyCell(Cell *c){
	c->fixed=0;
	c->value=0;
	c->invalid=0;

}

/*Creates an empty game board*/
void initializeEmptyBoard(Game *game){
	int i=1,j=1;
	game->board = (Cell**)malloc((game->board_size+1)*sizeof(Cell*));
	if(game->board==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	for(i=1;i<=game->board_size;i++){
		game->board[i]=(Cell*)malloc((game->board_size+1)*sizeof(Cell));
		if(game->board[i] == NULL){
			printf("ERROR: problem with memory allocation\n");
			exit(0);
		}
		if(game->board[i]==NULL){
			printf("ERROR: problem with memory allocation\n");
			exit(0);
		}
	}
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			initializeEmptyCell(&game->board[i][j]);
		}
	}

}

/*receives the desired number of rows and columns in a block and initializes a new game*/
Game* initializeGame(int num_of_rows_in_block, int num_of_columns_in_block){
	Game *game = (Game*)malloc(sizeof(Game));
	if(game==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	game->num_of_columns_in_block = num_of_columns_in_block;
	game->num_of_rows_in_block = num_of_rows_in_block;
	game->board_size = num_of_rows_in_block * num_of_columns_in_block;
	game->move_history = createList();
	initializeEmptyBoard(game);
	return game;
}

/*receives a row index and checks whether the values in this row are valid.
 * returns 1 if valid and 0 otherwise.
 */
int checkRowValid(int row ,Game *game){
	int i=1;
	int value;
	int *arr = (int*)calloc(game->board_size+1,sizeof(int));
	if(arr==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	for(i=1;i<=game->board_size;i++){
		if(game->board[row][i].value!=0){
			value=game->board[row][i].value;
			arr[value]++;
		}
	}
	for(i=1;i<=game->board_size;i++){
		if(arr[i]>1){
			free(arr);
			return 0;
		}
	}
	free(arr);
	return 1;
}

/*receives a column index and checks whether the values in this column are valid.
 * returns 1 if valid and 0 otherwise.
 */
int checkColumnValid(int column,Game *game){
	int i=1;
	int value;
	int *arr =(int*) calloc(game->board_size+1,sizeof(int));
	if(arr==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	for(i=1;i<=game->board_size;i++){
		if(game->board[i][column].value!=0){
			value=game->board[i][column].value;
			arr[value]++;
		}
	}
	for(i=1;i<=game->board_size;i++){
		if(arr[i]>1){
			free(arr);
			return 0;
		}
	}
	free(arr);
	return 1;
}

/*receives the first cell index of a block and checks whether the values in this block are valid.
 * returns 1 if valid and 0 otherwise.
 */
int checkBlockValid(int row,int column,Game *game){
	int i,j,value;
	int *arr =(int*) calloc(game->board_size+1,sizeof(int));
	if(arr==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	for(i=row;i<row+game->num_of_rows_in_block;i++){
		for(j=column;j<column+game->num_of_columns_in_block;j++){
			if(game->board[i][j].value!=0){
				value=game->board[i][j].value;
				arr[value]++;
			}
		}
	}
	for(i=1;i<=game->board_size;i++){
		if(arr[i]>1){
			free(arr);
			return 0;
		}
	}
	free(arr);
	return 1;
}

/*checks if all board game values are valid.
 * return 1 if valid and 0 otherwise.*/
int boardValueAreValid(Game *game){
	int i,correct,j;
	for(i=1;i<=game->board_size;i++){
		correct=checkColumnValid(i,game);
		if(correct==0){
			return 0;
		}
	}
	for(i=1;i<=game->board_size;i++){
		correct=checkRowValid(i,game);
		if(correct==0){
			return 0;
		}
	}
	for(i=1;i<=game->board_size;i=i+game->num_of_columns_in_block){
		for(j=1;j<=game->board_size;j=j+game->num_of_rows_in_block){
			correct=checkBlockValid(j, i, game);
			if(correct==0){
				return 0;
			}
		}
	}
	return 1;
}


/*receives the cell index, desired new value and start = {0,1} (start == 1 -> first move of a chain of moves).
 *updates the cell if all values are valid and the game isn't in Init mode
 *returns 1	if successful and 0 otherwise*/
int setCell(int x, int y, int z, Game *game, int start){
	Cell cur_cell;
	int max_value;
	max_value = game->num_of_columns_in_block*game->num_of_rows_in_block;
	if(x > game->board_size || x < 1){
		printf("Error: column number is incorrect - out of range. Must be between 1 and %d.\n", game->board_size);
		return 0;
	}
	if(y > game->board_size || y < 1){
		printf("Error: row number is incorrect - out of range. Must be between 1 and %d.\n", game->board_size);
		return 0;
	}

	if(z > max_value || z < 0){
		printf("Error: the 3rd parameter (value) is invalid, must be between %d and %d\n", 0, max_value);
		return 0;
	}
	if((game->board)[y][x].fixed && game_status == Solve){
		printf("Error: cell is fixed, can't change fixed cells in Solve mode\n");
		return 0;
	}
	cur_cell = (game->board)[y][x];
	(game->board)[y][x].value = z;
	insertToList(game->move_history, y, x, cur_cell.invalid, cur_cell.value, z, cur_cell.fixed, 0, start);
	if((game->board)[y][x].fixed){
			(game->board)[y][x].fixed = 0;
	}
	markInvalidCells(game);
	return 1;
}



/*turns all cells with values to fixed cells*/
void fixCellsWithValues(Game *game){
	int i, j;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if((game->board)[i][j].value != 0){
				(game->board)[i][j].fixed = 1;
			}
		}
	}

}

/*frees the game object and all related memory to it*/
void freeGame(Game *game){
	int i;
	if(game == NULL){
		return;
	}
	for(i=1;i<=game->board_size;i++){
		free(game->board[i]);
	}
	free(game->board);
	clearList(game->move_history);
	free(game);

}

/*checks whether a cell is valid and returns 1 or 0 otherwise*/
int invalidCell(Game *game, int row , int column, int ignore_not_fixed){
	int i,j,start_i,start_j,value;
	value=game->board[row][column].value;
	if(value==0){
		return 0;
	}
	else{
		for(i=1;i<=game->board_size;i++){
			if(i!=column){
				if((ignore_not_fixed==1 && game->board[row][i].fixed==1)|| ignore_not_fixed==0){
					if(game->board[row][i].value==value){
						return 1;
					}
				}

			}
		}
		for(i=1;i<=game->board_size;i++){
			if(i!=row){
				if((ignore_not_fixed==1 &&game->board[i][column].fixed==1)|| ignore_not_fixed==0){
					if(game->board[i][column].value==value){
						return 1;
					}
				}

			}
		}
		if( row%game->num_of_rows_in_block!=0){
			start_i=row- row%game->num_of_rows_in_block +1;
		}
		else{
			start_i=row-game->num_of_rows_in_block +1;
		}
		if(column%game->num_of_columns_in_block!=0){
			start_j=column- column%game->num_of_columns_in_block +1;
		}
		else{
			start_j=column-game->num_of_columns_in_block +1;
		}
		for(i=start_i; i<start_i+game->num_of_rows_in_block;i++){
			for(j=start_j; j<start_j+game->num_of_columns_in_block;j++){
				if(i!=row || j!=column){
					if((ignore_not_fixed==1 &&game->board[i][j].fixed==1)|| ignore_not_fixed==0){
						if(game->board[i][j].value==value){
							return 1;
						}

					}
				}
			}
		}
	}
	return 0;
}

/*sets the invalid field of all invalid cells in the board to 1 (true)*/
void markInvalidCells(Game *game){
	int i, j;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			game->board[i][j].invalid=invalidCell(game,i,j,0);

		}
	}
}

/*check if the input array has only one valid option,
 * returns 1 if true and 0 otherwise*/
int oneValidOption(int *arr,Game *game){
	int count=0,i=1,value=0;
	for (i=1; i<= game->board_size;i++){
		if(arr[i]==0){
			count++;
			value=i;
		}
	}
	if(count==1){
		return value;
	}
	return 0;
}

/*copies one game to another*/
void copyGame(Game *game, Game *copy_game){
	int i,j;
	copy_game->move_history = createList();
	copy_game->num_of_columns_in_block=game->num_of_columns_in_block;
	copy_game->num_of_rows_in_block=game->num_of_rows_in_block;
	copy_game->board_size=game->board_size;
	initializeEmptyBoard(copy_game);
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			copy_game->board[i][j].value= game->board[i][j].value;
			copy_game->board[i][j].fixed= game->board[i][j].fixed;
			copy_game->board[i][j].invalid= game->board[i][j].invalid;
		}
	}
}

/*autofills all cells which have only one possible value.
 * returns 1 if autofill was successful and 0 otherwise */
int autoFillBoard(Game *game){
	int i=1, j=1, value,start=1,changes=0;
	Game *copy_game;
	int *arr_of_options;
	if(!boardValueAreValid(game)){
		printf("ERROR: the board is erroneous\n");
		return 0;
	}
	copy_game = (Game*)malloc(sizeof(Game));
	if(copy_game==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	copyGame(game,copy_game);
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if(copy_game->board[i][j].value==0){
				arr_of_options=(int*) calloc(game->board_size+1, sizeof(int));
				if(arr_of_options==NULL){
					printf("ERROR: problem with memory allocation\n");
					exit(0);
				}
				fillArrWithOption(arr_of_options,copy_game,i,j);
				if(stillHasOptionForCell(arr_of_options,copy_game)){
					value= oneValidOption(arr_of_options,copy_game);
					if(value!=0){
						setCell(j,i,value,game,start);
						start=0;
						changes++;
					}
				}
				free(arr_of_options);
			}

		}
	}
	freeGame(copy_game);
	if(changes==0){
		printf("autofill didn't find any cells to fill\n");
	}
	return 1;
}

/*checks if all fixed cells are valid in relation to all the other fixed cells,
 * returns 1 if true and 0 otherwise*/
int checkFixedCells(Game *game){
	int i, j;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if((game->board)[i][j].fixed == 1){
				if(invalidCell(game, i , j, 1)){
					return 0;
				}
			}
		}
	}
	return 1;
}

/*imports a game from a file in Edit mode, if the file name provided is an empty string,
 * initializes a new game with 3x3 blocks board.*/
Game* edit(Game *cur_game, char* fileName){
	Game *game;
	if(strcmp(fileName, "") == 0){
		game_status = Edit;
		return initializeGame(3, 3);
	}
	game = readFromFile(fileName, 0);
	if(game != NULL){
		markInvalidCells(game);
		game_status = Edit;
		if(cur_game != NULL){
			freeGame(cur_game);
		}
	}
	return game;
}

/*imports a game from a file in Solve mode.*/
Game* solve(Game *cur_game, char* fileName){
	Game *game;
	game = readFromFile(fileName, 1);
	if(game != NULL){
		markInvalidCells(game);
		game_status = Solve;
		if(cur_game != NULL){
			freeGame(cur_game);
		}
	}
	return game;
}

/*sets the mar_errors field to 1 or 0 indicating whether to mark errors or not
 * when printing the game board.*/
void markErrors(int mark){
	if(mark == 0){
		mark_errors = 0;
	}
	else{
		mark_errors = 1;
	}
}

/*the function saves the current game to the save directory provided to it*/
void save(Game *game, char* save_dir){
	if(game_status != Solve && game_status != Edit){
		printf("ERROR: The save command is only available in Solve or Edit mode\n");
		return;
	}
	saveToFile(game, save_dir);
}

/*receives a Move object and Undo = {0,1} (Undo == 1 -> Undo Move and Undo == 0 -> Redo Move).
 *changes the board accordingly until reaching the next move*/
void setCellsByMove(Game *game, Move *move, int undo){
	while(move->next != NULL && !move->next->move_start){
		move = move->next;
	}
	while(1){
		int y = move->row;
		int x = move->column;
		if(undo){
			printf("undo: changed cell [%d][%d] back from %d to %d\n", x, y, move->after_value, move->before_value);
			(game->board)[y][x].value = move->before_value;
			(game->board)[y][x].fixed = move->fixed_before;
		}
		else{
			printf("redo: changed cell [%d][%d] from %d to %d\n", x, y, move->before_value, move->after_value);
			(game->board)[y][x].value = move->after_value;
			(game->board)[y][x].fixed = move->fixed_after;
		}
		if(move->move_start){
			break;
		}
		move = move->prev;
	}
	markInvalidCells(game);
}

/*undo move*/
int undoMove(Game *game){
	Move *move = undo(game->move_history);
	if(move == NULL){
		printf("Error: no moves to undo\n");
		return 0;
	}
	setCellsByMove(game, move, 1);
	return 1;
}

/*redo move*/
int redoMove(Game *game){
	Move *move = redo(game->move_history);
	if(move == NULL){
		printf("Error: no moves to redo\n");
		return 0;
	}
	setCellsByMove(game, move, 0);
	return 1;
}

/*resets the board to it's first state according to it's move history*/
void resetBoard(Game *game){
	Move *move = undo(game->move_history);
	while(move != NULL){
		setCellsByMove(game, move, 1);
		move = undo(game->move_history);
	}
}

/*exits the game cleanly, calls the freeGame function to clear the current game and
 * all the memory related to it*/
void exitGame(Game *game){
	freeGame(game);
	printf("exiting...\n");
	exit(0);
}

/*returns the number of empty cells in the board*/
int numberOfEmptyCell(Game *game){
	int count=0,i,j;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if(game->board[i][j].value==0){
				count++;
			}
		}
	}
	return count;
}

/*returns 1 if game board is solved and 0 otherwise*/
int isWin(Game *game){
	int valid,number;
	valid=boardValueAreValid(game);
	number=numberOfEmptyCell(game);
	if(valid==1 && number==0){
		return 1;
	}
	return 0;
}

/*changes the game mode to Init*/
void changeToInit(){
	game_status = Init;
}

