/*this module includes all functions that create gurobi env and use array belongs to gurobi model*/
#define ERRORMAL "ERROR: problem with memory allocation\n"
#define ERRORGUR "Error: Gurobi optimizer has failed\n"
#include <stdlib.h>
#include <stdio.h>
#include "gurobi_c.h"
#include "game.h"
#include "solve.h"

typedef struct{
	int row;
	int column;
	int value;
} Option;

GRBenv   *env   = NULL;
GRBmodel *model = NULL;
int       error = 0;
double    *sol;
int       *ind;
double    *val;
double    *obj;
double    *lb; /*determines lower bounds for the variables*/
double    *ub;  /*determines an upper bound for the variables.*/
Option    *obj_value;
char      *vtype;
int       optimstatus;
double    objval;
int number_of_varibals=0;

/*creates a gurobi environment. */
int createEnv(){
	int error;
	error = GRBloadenv(&env, NULL);
	if (error) {
		return 0;
	}
	return 1;
}

/*gurobi wont print messages*/
int noMessages(){
	int error;
	error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0);
	if (error) {
		return 0;
	}
	return 1;
}


/* Create an empty model named "mip1" */
int createEmptyModel(){
	int error;
	error = GRBnewmodel(env, &model, "mip1", number_of_varibals, obj, lb, ub, vtype, NULL);
	if (error) {
		return 0;
	}
	return 1;
}
/*create arr obj_value with the key between obj index and the real ijk indicator*/
void fillObjValue(Game *game){
	int i,j,value,invalid,count=0;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if(game->board[i][j].value==0){
				for(value=1;value<=game->board_size;value++){
					game->board[i][j].value=value;
					invalid=invalidCell(game,i,j,0);
					if(invalid==0){
						obj_value[count].row=i;
						obj_value[count].column=j;
						obj_value[count].value=value;
						count++;
					}
					game->board[i][j].value=0;
				}
			}
		}
	}
}

/*fills the object array*/
int createObjAndSendToModel(Game *game, int ILP){
	int count=0,i,j,value,invalid=0, there_is_valid_option=0;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if(game->board[i][j].value==0){
				there_is_valid_option=0;
				for(value=1;value<=game->board_size;value++){
					game->board[i][j].value=value;
					invalid=invalidCell(game,i,j,0);
					if(invalid==0){
						count++;
						there_is_valid_option=1;
					}
					game->board[i][j].value=0;
				}
				if(there_is_valid_option==0){
					return -1;
				}
			}
		}
	}
	number_of_varibals=count;
	obj_value= (Option*)malloc(number_of_varibals*sizeof(Option));
	if(obj_value==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	fillObjValue(game);
	obj=(double*)calloc(number_of_varibals,sizeof(double));
	if(obj==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	vtype=(char*)calloc(number_of_varibals,sizeof(char));
	if(vtype==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	/*put random coefficients for variables and  variable types to vtype arr*/
	for(i=0;i<count;i++){
		obj[i]= rand()%(2*game->board_size);
		if(ILP){
			vtype[i]=GRB_BINARY;
		}
		else{
			vtype[i]=GRB_CONTINUOUS;
		}
	}
	return 1;
}

/*sends the arrays to the model*/
int sendToModel(){
	int error;
	/* add variables to model */
	error = GRBaddvars(model, number_of_varibals, 0, NULL, NULL, NULL, obj, lb, ub, vtype, NULL);
	if (error) {
		return 0;
	}
	/* Change objective sense to minimization */
	error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE, GRB_MINIMIZE);
	if (error) {
		return 0;
	}
	/* update the model - to integrate new variables */
	error = GRBupdatemodel(model);
	if (error) {
		return 0;
	}
	return 1;
}

/*finds the index of the variable that represents the ij'th value*/
int findPlaceForOption(int i,int j,int value){
	int place;
	for(place=0;place<number_of_varibals;place++){
		if(obj_value[place].column==j && obj_value[place].row==i && obj_value[place].value==value){
			return place;
		}
	}
	return -1;
}

/*sets cell constraints (every cell should be filled with exactly one value)*/
int setCellConstraints(Game *game){
	int error;
	int i,j,place,value,current_k=0;
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			current_k=0;
			for(value=1;value<=game->board_size;value++){
				place=findPlaceForOption(i,j,value);
				if(place!=-1){
					current_k++;
				}
			}
			if(current_k>0){
				ind = (int*)calloc(current_k,sizeof(int));
				if(ind==NULL){
					printf(ERRORMAL);
					exit(0);
				}
				val = (double*)calloc(current_k,sizeof(double));
				if(val==NULL){
					printf(ERRORMAL);
					exit(0);
				}
				current_k=0;
				for(value=1;value<=game->board_size;value++){
					place=findPlaceForOption(i,j,value);
					if(place!=-1){
						ind[current_k]=place;
						val[current_k]=1.0;
						current_k++;
					}
				}
				/*add constrain to mode*/
				error = GRBaddconstr(model, current_k, ind, val, GRB_EQUAL, 1, NULL);
				if (error) {
					free(ind);
					free(val);
					return 0;
				}
				free(ind);
				free(val);
			}
		}
	}

	return 1;
}

/*sets row constraints*/
int setRowConstraints(Game *game){
	int error;
	int i,j,place,value,current_k=0;
	for(i=1;i<=game->board_size;i++){
		for(value=1;value<=game->board_size;value++){
			current_k=0;
			for(j=1;j<=game->board_size;j++){
				place=findPlaceForOption(i,j,value);
				if(place!=-1){
					current_k++;
				}
			}
			if(current_k>0){
				ind = (int*)calloc(current_k,sizeof(int));
				if(ind==NULL){
					printf(ERRORMAL);
					exit(0);
				}
				val = (double*)calloc(current_k,sizeof(double));
				if(val==NULL){
					printf(ERRORMAL);
					exit(0);
				}
				current_k=0;
				for(j=1;j<=game->board_size;j++){
					place=findPlaceForOption(i,j,value);
					if(place!=-1){
						ind[current_k]=place;
						val[current_k]=1.0;
						current_k++;
					}
				}
				/*add constrain to mode*/
				error = GRBaddconstr(model, current_k, ind, val, GRB_EQUAL, 1, NULL);
				if (error) {
					free(ind);
					free(val);

					return 0;
				}
				free(ind);
				free(val);
			}
		}
	}
	return 1;
}

/*sets column constraints*/
int setColumnConstraints(Game *game){
	int error;
	int i,j,place,value,current_k=0;
	for(i=1;i<=game->board_size;i++){
		for(value=1;value<=game->board_size;value++){
			current_k=0;
			for(j=1;j<=game->board_size;j++){
				place=findPlaceForOption(j,i,value);
				if(place!=-1){
					current_k++;
				}
			}
			if(current_k>0){
				ind = (int*)calloc(current_k,sizeof(int));
				if(ind==NULL){
					printf(ERRORMAL);
					exit(0);
				}
				val = (double*)calloc(current_k,sizeof(double));
				if(val==NULL){
					printf(ERRORMAL);
					exit(0);
				}
				current_k=0;
				for(j=1;j<=game->board_size;j++){
					place=findPlaceForOption(j,i,value);
					if(place!=-1){
						ind[current_k]=place;
						val[current_k]=1.0;
						current_k++;
					}
				}
				/*add constrain to mode*/
				error = GRBaddconstr(model, current_k, ind, val, GRB_EQUAL, 1, NULL);
				if (error) {
					free(ind);
					free(val);
					return 0;
				}
				free(ind);
				free(val);
			}
		}
	}
	return 1;
}

/*sets block constraints*/
int setBlockConstraints(Game *game){
	int error;
	int i,j,place,value,current_k=0,row,column;
	/*for each block*/
	for(i=1;i<=game->board_size;i=i+game->num_of_columns_in_block){
		for(j=1;j<=game->board_size;j=j+game->num_of_rows_in_block){
			/*for each value in each block*/
			for(value=1;value<=game->board_size;value++){
				current_k=0;
				for(row=j;row<j+game->num_of_rows_in_block;row++){
					for(column=i;column<i+game->num_of_columns_in_block;column++){
						place=findPlaceForOption(row,column,value);
						if(place!=-1){
							current_k++;
						}
					}
				}
				if(current_k>0){
					ind = (int*)calloc(current_k,sizeof(int));
					if(ind==NULL){
						printf(ERRORMAL);
						exit(0);
					}
					val = (double*)calloc(current_k,sizeof(double));
					if(val==NULL){
						printf(ERRORMAL);
						exit(0);
					}
					current_k=0;
					for(row=j;row<j+game->num_of_rows_in_block;row++){
						for(column=i;column<i+game->num_of_columns_in_block;column++){
							place=findPlaceForOption(row,column,value);
							if(place!=-1){
								ind[current_k]=place;
								val[current_k]=1.0;
								current_k++;
							}
						}
					}
					/*add constrain to mode*/
					error = GRBaddconstr(model, current_k, ind, val, GRB_EQUAL, 1, NULL);
					if (error) {
						free(ind);
						free(val);
						return 0;
					}
					free(ind);
					free(val);
				}
			}
		}
	}
	return 1;
}

/*bounds the variables between 0 and 1*/
int createBounds(){
	int i;
	lb=(double*)malloc(number_of_varibals*sizeof(double));
	ub=(double*)malloc(number_of_varibals*sizeof(double));
	if(ub==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	if(lb==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	for(i=0;i<number_of_varibals;i++){
		lb[i]=0.0;
		ub[i]=1.0;
	}
	return 1;
}


/* Optimize model - need to call this before calculation */
int optimizeTheModel(){
	int error;
	sol = (double*)calloc(number_of_varibals,sizeof(double));
	if(sol==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	error = GRBoptimize(model);
	if (error) {
		return 0;
	}
	/* Write model to 'mip1.lp' - this is not necessary but very helpful */
	error = GRBwrite(model, "mip1.lp");
	if (error) {
		return 0;
	}
	return 1;
}

/*gests the solution*/
int getSol(){
	int error;
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error) {
		printf("ERROR %d GRBgetintattr(): %s\n", error, GRBgeterrormsg(env));
		return -1;
	}
	/*no solution*/
	if (optimstatus != GRB_OPTIMAL) {
		return 0;
	}
	/* get the solution - the assignment to each variable */
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, number_of_varibals, sol);
	if (error) {
		return -1;
	}
	return 1;
}

/* Gets solution information */
Game *findHintBoard(Game *game){
	int i,j,row,column,value;
	Game *game_sol= initializeGame(game->num_of_rows_in_block,game->num_of_columns_in_block);
	for(i =1; i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			game_sol->board[i][j].value=game->board[i][j].value;
			game_sol->board[i][j].invalid=game->board[i][j].invalid;
			game_sol->board[i][j].fixed=game->board[i][j].fixed;
		}
	}
	for(i=0;i<number_of_varibals;i++){
		if(sol[i]==1){
			row = obj_value[i].row;
			column= obj_value[i].column;
			value=obj_value[i].value;
			game_sol->board[row][column].value=value;
			if(!boardValueAreValid(game_sol)){
				game_sol->board[row][column].value=0;
			}
		}
	}
	return game_sol;
}

/*recieves a cell index and finds a hint*/
int getHint(int row, int column, Game *game){
	Game *game_sol;
	int value;
	game_sol=findHintBoard(game);
	value= game_sol->board[row][column].value;
	freeGame(game_sol);
	return value;
}

/*finds the probabilities for values for the cell i,j */
int getValueForCellByPrecent(Game *game_sol,int i, int j,float X){
	int value,place;
	double random;
	float precent,num_possible=0,sum=0 ;
	float *arr_of_option_without_precent= (float*)calloc(game_sol->board_size+1,sizeof(float));
	if(arr_of_option_without_precent==NULL){
		printf(ERRORMAL);
		exit(0);
	}
	for(value=1;value<=game_sol->board_size;value++){
		place=findPlaceForOption(i,j,value);
		precent=sol[place];
		if(precent>=X){
			arr_of_option_without_precent[value]=precent;
		}
		else{
			arr_of_option_without_precent[value]=0;
		}
	}
	for(value=1;value<=game_sol->board_size;value++){
		game_sol->board[i][j].value=value;
		if(boardValueAreValid(game_sol) && arr_of_option_without_precent[value]>0){
			num_possible+=arr_of_option_without_precent[value];
		}
		else{
			arr_of_option_without_precent[value]=0;
		}
		game_sol->board[i][j].value=0;
	}
	if(num_possible>0){
		random= (double)rand();
		random=random/RAND_MAX;
		random=random*num_possible;
		for(value=1;value<=game_sol->board_size;value++){
			if(random>=sum && random<= sum+arr_of_option_without_precent[value]){
				break;
			}
			sum+=arr_of_option_without_precent[value];
		}
	}
	else{
		value=0;
	}
	free(arr_of_option_without_precent);
	return value;
}

/*fillAllGuesses helper function*/
Game *findGuessBoard(Game *game,float X){
	int i,j;
	int value;
	Game *game_sol= initializeGame(game->num_of_rows_in_block,game->num_of_columns_in_block);
	for(i =1; i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			game_sol->board[i][j].value=game->board[i][j].value;
			game_sol->board[i][j].invalid=game->board[i][j].invalid;
			game_sol->board[i][j].fixed=game->board[i][j].fixed;
		}
	}
	for(i=1;i<=game_sol->board_size;i++){
		for(j=1;j<=game_sol->board_size;j++){
			if(game_sol->board[i][j].value==0){
				value=getValueForCellByPrecent(game_sol, i, j,X);
				game_sol->board[i][j].value=value;
			}
		}
	}
	return game_sol;
}

/*fills the board with guesses with probabilities higher than X*/
void fillAllGuesses(Game *game,float X, int start){
	Game *game_sol;
	int i,j;
	game_sol=findGuessBoard(game,X);
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
			if(game->board[i][j].value==0 && game_sol->board[i][j].value!=0){
				setCell(j,i,game_sol->board[i][j].value,game,start);
				start=0;
			}
		}
	}
	freeGame(game_sol);
}

/*prints the guesses corresponding to the received index*/
void printGuessHint(Game *game, int row, int column){
	int value,place,count;
	printf("guess hint cell <%d,%d>:\n",column,row);
	for(value=1;value<=game->board_size;value++){
		place=findPlaceForOption(row,column,value);
		if(place!=-1 && sol[place]>0){
			printf("value %d has %f percent chance to fit in this cell\n",value,sol[place]);
			count++;
		}
	}
	if(count==0){
		printf("there are no guesses with a positive chance\n");
	}
}
void freelbub(){
	free(ub);
	free(lb);
}

/* Free model and environment */
void freeGR(int sol_free){

	if(sol_free){
		free(sol);
		free(vtype);
		free(obj_value);
	}
	free(obj);
	GRBfreemodel(model);
	GRBfreeenv(env);


}

