/*this module includes all game functions that use gurobi*/

#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "solve.h"
#include "structs.h"
#include "gurobi_c.h"
#include "gurobi.h"

/*calls gurobi ILP/LP*/
int callGurobi(Game *game, int ILP){
	int value;
	if(!createEnv()){
		return -1;
	}
	if(!noMessages()){
		return -1;
	}
	value=createObjAndSendToModel(game,ILP);
	if(value==0){
		return -1;
	}
	if(value==-1){
		return 2;
	}
	if(ILP==0){
		if(!createBounds()){
			return -1;
		}
	}
	if(!createEmptyModel()){
		return -1;
	}
	if(!sendToModel()){
		return -1;
	}
	if(!setCellConstraints(game)){
		return -1;
	}
	if(!setRowConstraints(game)){
		return -1;
	}
	if(!setColumnConstraints(game)){
		return -1;
	}
	if(!setBlockConstraints(game)){
		return -1;
	}
	if(!optimizeTheModel()){
		return -1;
	}
	value=getSol();
	return value;
}
/*fill all cells which have only one possible value.
 * returns 1 if put any valid option and 0 otherwise */
int fillPossibleCells(Game *game){
	int i=1, j=1, value,start=1;
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
					}
				}
				free(arr_of_options);
			}
		}
	}
	freeGame(copy_game);
	if(start==0){
		/*need to continue with setCell 0*/
		return 2;
	}
	return 1;
}
/*checks whether the board is solvable using gurobi ILP*/
void validate(Game *game){
	int value=0,valid;
	valid=boardValueAreValid(game);
	if(valid==0){
		printf("there is erroneous\n");
		return;
	}
	value=callGurobi(game,1);
	if(value==1){
		printf("solution found- board solvable\n");
	}
	if(value==0 || value==2){
		printf("no solution found- board is not solvable\n");
	}
	if(value==-1){
		printf("ERROR: a problem has accrued with Gurobi\n");
	}
	if(value==2){
		freeGR(0);
	}
	else{
		freeGR(1);
	}

}

/*receives cell index and finds a hint for it if exists*/
void hint(Game *game, int row,int column){
	int value=0,valid;
	if(column<0 || column>game->board_size){
		printf("first parameter not in range, should be between 1 and %d\n",game->board_size);
		return;
	}
	if(row<0 || row>game->board_size){
		printf("Second parameter not in range, should be between 1 and %d\n",game->board_size);
		return;
	}
	valid=boardValueAreValid(game);
	if(valid==0){
		printf("the board is erroneous\n");
		return;
	}
	if(game->board[row][column].fixed==1||game->board[row][column].value!=0){
		printf("this cell already contains a value\n");
		return;
	}
	value=callGurobi(game,1);

	if(value==1){
		int value = getHint(row,column,game);
		printf("Hint- set cell <%d,%d> to %d.\n",column,row,value);
	}
	if(value==0 || value==2){
		printf("no solution found- board is not solvable, can not present a hint\n");
	}
	if(value==-1){
		printf("ERROR: a problem has accrued with Gurobi\n");
	}
	if(value==2){
		freeGR(0);
	}
	else{
		freeGR(1);
	}
}

/*receives a threshold value and fills the game board with guesses with higher probability than the threshold*/
int guess(Game *game, float X){
	int value=0,valid,start=1;
	valid= fillPossibleCells(game);
	if(valid==0){
		printf("the board is erroneous\n");
		return 0;
	}
	value = callGurobi(game,0);
	/*need to continue with setCell 0*/
	if(valid==2){
		start=0;
	}
	if(value==1){
		fillAllGuesses(game,X,start);
		freelbub();
		freeGR(1);
		return 1;
	}
	if(value==0 || value==2){
		printf("no solution found- board is not solvable\n");

		if(value==2){
			freeGR(0);
		}
		else{
			freelbub();
			freeGR(1);
		}
		return 0;
	}
	if(value==-1){
		printf("ERROR: a problem has accrued with Gurobi\n");
		freelbub();
		freeGR(1);
		return 0;
	}
	return 0;
}

/*receives a cell index and finds all possible valid values for it with probability higher than 0 for a solution*/
void guessHint(Game *game, int row, int column){
	int value=0,valid;
	if(column<0 || column>game->board_size){
		printf("first parameter not in range, should be between 1 and %d\n",game->board_size);
		return;
	}
	if(row<0 || row>game->board_size){
		printf("Second parameter not in range, should be between 1 and %d\n",game->board_size);
		return;
	}
	valid=boardValueAreValid(game);
	if(valid==0){
		printf("the board is erroneous\n");
		return;
	}
	if(game->board[row][column].fixed==1||game->board[row][column].value!=0){
		printf("this cell already contains a value\n");
		return;
	}
	value = callGurobi(game,0);
	if(value==1){
		printGuessHint(game, row,column);
		freelbub();
		freeGR(1);
	}
	if(value==0 ||value==2){
		printf("no solution found- board is not solvable, can not present a hint\n");
		if(value==2){
			freeGR(0);
		}
		else{
			freelbub();
			freeGR(1);
		}
	}
	if(value==-1){
		printf("ERROR: a problem has accrued with Gurobi\n");
		freelbub();
		freeGR(1);

	}
}

/*returns a random empty cell index*/
int *randomCell(Game *copy_game){
	int row, column;
	int *result=(int*)malloc(2*sizeof(int));
	if(result==NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	row=(rand()%copy_game->board_size)+1;
	column=(rand()%copy_game->board_size)+1;
	while(copy_game->board[row][column].value!=0){
		row=(rand()%copy_game->board_size)+1;
		column=(rand()%copy_game->board_size)+1;
	}
	result[0]=row;
	result[1]=column;
	return result;
}

/*deletes Y random values from the game board*/
void emptyYCells(Game *copy_game, int Y){
	int row, column,i;
	for(i=1;i<=Y;i++){
		row=(rand()%copy_game->board_size)+1;
		column=(rand()%copy_game->board_size)+1;
		while(copy_game->board[row][column].value==0){
			row=(rand()%copy_game->board_size)+1;
			column=(rand()%copy_game->board_size)+1;
		}
		copy_game->board[row][column].value=0;
		copy_game->board[row][column].fixed=0;
		copy_game->board[row][column].invalid=0;
	}
}

/*fills X random cells with random valid values*/
int fillXCells(Game *copy_game, int X){
	int value=0,count,*indexes,random_index,count_X;
	int *arr_of_option;
	for(count_X=1;count_X<=X ;count_X++){
		arr_of_option= (int*)calloc(copy_game->board_size+1,sizeof(int));
		if(arr_of_option==NULL){
			printf("ERROR: problem with memory allocation\n");
			exit(0);
		}
		indexes = randomCell(copy_game);
		fillArrWithOption(arr_of_option,copy_game,indexes[0],indexes[1]);
		if(stillHasOptionForCell(arr_of_option,copy_game)){
			count=0;
			for(value=1;value<=copy_game->board_size;value++){
				if(arr_of_option[value]==0){
					count++;
				}
			}
			random_index=rand()%count;
			count =0;
			for(value=1;value<=copy_game->board_size;value++){
				if(arr_of_option[value]==0){
					if(count==random_index){
						copy_game->board[indexes[0]][indexes[1]].value=value;
					}
					count++;
				}
			}
		}
		else{
			free(indexes);
			free(arr_of_option);
			return 0;
		}
		free(indexes);
		free(arr_of_option);
	}
	return 1;
}

/*Generates a puzzle by randomly filling X empty cells with legal values,
running ILP to solve the board, and then clearing all but Y random cells.*/
int generate(Game *game, int X,int Y){
	int value=0,valid,count,i,j,k, first=1,number_to_earase=0;
	Game *copy_game ;
	Game *game_sol ;
	count= numberOfEmptyCell(game);
	if(X<0 || X>count){
		printf("first parameter not in range, should be between 0 and %d\n",count);
		return 0;
	}
	if(Y<=0 || Y>(game->board_size*game->board_size)){
		printf("Second parameter not in range, should be between 0 and %d\n",(game->board_size*game->board_size));
		return 0;
	}
	valid=boardValueAreValid(game);
	if(valid==0){
		printf("the board is erroneous\n");
		return 0;
	}

	for(i=0;i<1000;i++){
		copy_game =(Game*)malloc(sizeof(Game));
		if(copy_game==NULL){
			printf("ERROR: problem with memory allocation\n");
			exit(0);
		}
		copyGame(game,copy_game);
		/*fill X cells*/
		valid= fillXCells(copy_game,X);
		if(valid){
			/*after fill X cell valid do ILP*/
			value=callGurobi(copy_game,1);
			if(value==1){
				game_sol= findHintBoard(copy_game);
				/*empty all but Y cells*/
				number_to_earase=game_sol->board_size*game_sol->board_size-Y;
				emptyYCells(game_sol,number_to_earase);
				for(j=1;j<=game->board_size;j++){
					for(k=1;k<=game->board_size;k++){
						if(game_sol->board[j][k].value!=game->board[j][k].value){
							setCell(k,j,game_sol->board[j][k].value,game,first);
							first=0;
						}
					}
				}
				freeGame(copy_game);
				freeGame(game_sol);
				freeGR(1);
				return 1;
			}
			if(value==2){
				freeGR(0);
			}
			else{
				freelbub();
				freeGR(1);
			}
		}
		freeGame(copy_game);
	}
	printf("generate didn't find any cells to fill\n");
	return 0;
}




