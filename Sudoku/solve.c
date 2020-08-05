/*This module is responsible for the functions related to the stack and number of solutions.*/


#include "structs.h"
#include <stdlib.h>
#include "game.h"
#include "solve.h"
#include "stack.h"
/*get an array and game board and update the array- array[i]==0 -> i is an valid number to the cell, else i is unvalid option
 *
 */

/*receives a cell index and fills it's array with its possible options */
void fillArrWithOption(int *arr_of_options,Game *game,int row,int column){
	int i,j,value1=0,value2;
	int column_start_point;
	int row_start_point ;
	if(column%game->num_of_columns_in_block==0){
		column_start_point= column -game->num_of_columns_in_block+1;
	}
	else{
		column_start_point= column -column%game->num_of_columns_in_block+1;
	}
	if(row%game->num_of_rows_in_block==0){
		row_start_point= row -game->num_of_rows_in_block+1;
	}
	else{
		row_start_point= row -row%game->num_of_rows_in_block+1;
	}
	for(i=1;i<=game->board_size;i++){
		if(game->board[i][column].value!=0){
			value1= game->board[i][column].value;
			arr_of_options[value1]++;
		}
		if(game->board[row][i].value!=0){
			value2= game->board[row][i].value;
			arr_of_options[value2]++;
		}
	}
	for(i=row_start_point;i<row_start_point+game->num_of_rows_in_block;i++){
		for(j=column_start_point;j<column_start_point+game->num_of_columns_in_block;j++){
			if(game->board[i][j].value!=0){
				value1=game->board[i][j].value;
				arr_of_options[value1]++;
			}
		}
	}
}

/*gets cell index and value and updates the cell */
void setCellForAlgo(int row, int column, int value,Game *game){
	game->board[row][column].value=value;
}

/*receives a cell index and fills it with a valid option */
int fillCellByAlgo(int row, int column,Game *game, int *arr_of_options){
	int i=1;
	for(i=1;i<=game->board_size;i++){
		if(arr_of_options[i]==0){
			arr_of_options[i]=1;
			setCellForAlgo(row,column,i,game);
			return 1;
		}
	}
	return 0;
}

/*receives an array and return 1 if not empty and 0 otherwise*/
int stillHasOptionForCell(int *arr_of_options, Game *game){
	int i=1;
	for(i=1;i<=game->board_size;i++){
		if(arr_of_options[i]==0){
			return 1;
		}
	}
	return 0;
}

 /*implements the backtracking algorithm with a stack*/
int backTracking(int i, int j, Game *game){
	Stack *stack= stackCreate();
	StackNode *node;
	int going_back=0;
	int succeed=0,count=0;
	int *arr_of_options;
	while(1){
		if(game->board[i][j].value!=0){
			if(j<game->board_size){
				j++;
			}
			else{
				if(i<game->board_size){
					i++;
					j=1;
				}
				else{
					count++;
					if(stackSize(stack)>0){
						node= stackPop(stack);
						i=node->i;
						j=node->j;
						arr_of_options=node->arr_of_options;
						initializeEmptyCell(&game->board[i][j]);
						going_back=1;
						free(node);
					}
				}
			}
		}
		else{
			if(going_back==0){
				arr_of_options=(int*)calloc(game->board_size+1,sizeof(int));
				if(arr_of_options==NULL){
					printf("ERROR: problem with memory allocation\n");
					exit(0);
				}
				fillArrWithOption(arr_of_options,game,i,j);
			}
			succeed= fillCellByAlgo(i,j,game,arr_of_options);
			if(succeed){
				if(j<game->board_size){
					arr_of_options[game->board[i][j].value]++;
					stackPush(stack,i,j,arr_of_options);
					j++;
					going_back=0;
				}
				else{
					if(i<game->board_size){
						arr_of_options[game->board[i][j].value]++;
						stackPush(stack,i,j,arr_of_options);
						i++;
						j=1;
						going_back=0;
					}
					else{
						count++;
						free(arr_of_options);
						initializeEmptyCell(&game->board[i][j]);
						if(stackSize(stack)>0){
							node= stackPop(stack);
							i=node->i;
							j=node->j;
							arr_of_options=node->arr_of_options;
							initializeEmptyCell(&game->board[i][j]);
							going_back=1;
							free(node);
						}
					}
				}
			}
			else{
				free(arr_of_options);
				if(stackSize(stack)>0){
					node= stackPop(stack);
					i=node->i;
					j=node->j;
					arr_of_options=node->arr_of_options;
					arr_of_options[game->board[i][j].value]++;
					initializeEmptyCell(&game->board[i][j]);
					going_back=1;
					free(node);
				}
				else{
					stackDestroy(stack);
					return count;
				}
			}
		}
	}
	return count;
}

/*returns the number of possible solutions for the game using a stack*/
int countSolutions(Game *game){
	int i=1,j=1, start_index_row, start_index_column, check=0;
	if(!boardValueAreValid(game)){
		printf("ERROR: board is invalid\n");
		return 0;
	}
	for(i=1;i<=game->board_size && check ==0;i++){
		for(j=1;j<=game->board_size && check ==0;j++){
			if(game->board[i][j].value==0){
				if(check==0){
					start_index_row=i;
					start_index_column=j;
					check=1;
				}
			}
		}
	}

	return backTracking(start_index_row,start_index_column,game);


}
