/*This module is responsible for the functions related to importing and exporting the game
 * to and from a file.*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "game.h"
#include "parser.h"

/*checks whether a string is a representation of a non fixed cell.
 * returns 1 if true and 0 otherwise*/
int checkFormatNotFixed(char* token){
	if(strlen(token) > 0){
			if(checkIfStringIsInt(token)){
				return 1;
		}
	}
	return 0;
}

/*checks whether a string is a representation of a fixed cell.
 * returns 1 if true and 0 otherwise*/
int checkFormatFixed(char* token){
	char check[1024];
	if(strlen(token) >= 2){
				strncpy(check, token, strlen(token) - 1);
				check[strlen(token) - 1] = '\0';
				if(checkIfStringIsInt(check) && token[strlen(token) - 1] == '.'){
						return 1;
				}
			}
	return 0;
}

/*checks whether a string is a representation of a board size.
 * returns 1 if true and 0 otherwise*/
int checkMN(char *token){
	if(token == NULL){
				printf("ERROR: invalid format\n");
				return 0;
		}
		if(!checkFormatNotFixed(token)){
				printf("ERROR: invalid format\n");
				return 0;
		}
	return 1;
}

/*imports a game board from fileDir, returns it if successful and NULL otherwise*/
Game* readFromFile(char* fileDir, int check_errors){
	int num_of_columns_in_block, num_of_rows_in_block, board_size;
	long file_size;
	char *buffer;
	char *token;
	char value[1024];
	FILE *fptr;
	int x = 1;
	int y = 1;
	Game *game;
	int converted_value;
	char delim[] = " \t\r\n\v\f";
	fptr = fopen(fileDir, "r");
	if(fptr == NULL){
		printf("ERROR: invalid file name or file doesn't exist\n");
		return NULL;
	}
	fseek(fptr, 0, SEEK_END);
	file_size = ftell(fptr);
	fseek(fptr, 0, SEEK_SET);
	buffer = (char*)malloc(file_size);
	if(buffer == NULL){
		printf("ERROR: problem with memory allocation\n");
		fclose(fptr);
		exit(0);
	}
	fread (buffer, 1, file_size, fptr);
	token = strtok(buffer, delim);
	if(checkMN(token) == 0){
		goto free;
	}
	num_of_rows_in_block = converStringToInt(token);
	token = strtok(NULL, delim);
	if(checkMN(token) == 0){
		goto free;
	}
	num_of_columns_in_block = converStringToInt(token);
	board_size = num_of_columns_in_block * num_of_rows_in_block;
	game = initializeGame(num_of_rows_in_block, num_of_columns_in_block);
	token = strtok(NULL, delim);
	while( token != NULL ) {
		if(checkFormatNotFixed(token)){
	    	  converted_value = converStringToInt(token);
	    	  if(converted_value >= 0 && converted_value <= board_size){
				  (game->board)[y][x].value = converted_value;
				  (game->board)[y][x].fixed = 0;
	    	  }
	    	  else{
	    		  printf("ERROR: one or more cell value is out of range\n");
	    		  goto free_without_print;
	    	  }
	      }
	      else if(checkFormatFixed(token)){
	    	  strncpy(value, token, strlen(token) - 1);
	    	  converted_value = converStringToInt(value);
	    	  if(converted_value >= 1 && converted_value <= board_size){
				  (game->board)[y][x].value = converted_value;
				  (game->board)[y][x].fixed = 1;
	    	  }
	    	  else{
	    		  printf("ERROR: one or more cell value is out of range\n");
	    		  goto free_without_print;
	    	  }
	      }
	      else{
	    	  goto free;
	      }
	      token = strtok(NULL, delim);
	      if(token == NULL && (x != board_size || y != board_size)){
	      	    	  goto free;
	      }
	      if(x == board_size){
	    	  if(y == board_size){
	    		  break;
	    	  }
	    	  x = 1;
	    	  y++;
	      }
	      else{
	    	  x++;
	      }
	}
	token = strtok(NULL, delim);
	if(check_errors == 1 && checkFixedCells(game) == 0){
		printf("ERROR: trying to load a board with an erroneous fixed cell\n");
		goto free_without_print;
	}
	if(x != board_size || y != board_size || token != NULL){
		free:
		printf("ERROR: invalid format\n");
		free_without_print:
		freeGame(game);
		free(buffer);
		fclose(fptr);
		return NULL;
	}
	if(check_errors == 1){
		game_status = Solve;
	}
	else{
		game_status = Edit;
	}
	free(buffer);
	fclose(fptr);
	return game;
}

/*the function saves the current game to the save directory provided to it*/
void saveToFile(Game *game, char* fileDir){
	int i, j;
	Cell cur;
	char converted[12];
	FILE *fptr = fopen(fileDir, "w");
	if(fptr == NULL){
				printf("ERROR: invalid file name or file doesn't exist\n");
				return;
	}
	if(game_status == Edit){
		if(!boardValueAreValid(game)){
			printf("ERROR: can't save an erroneous board in edit mode\n");
			fclose(fptr);
			return;
		}
		fixCellsWithValues(game);
	}
	sprintf(converted, "%d", game->num_of_rows_in_block);
	fprintf(fptr, "%s", converted);
	fprintf(fptr, " ");
	sprintf(converted, "%d", game->num_of_columns_in_block);
	fprintf(fptr,"%s", converted);
	fprintf(fptr, "\n");
	for(i=1;i<=game->board_size;i++){
		for(j=1;j<=game->board_size;j++){
				cur = (game->board)[i][j];
				sprintf(converted, "%d", cur.value);
				fprintf(fptr, "%s", converted);
				if(cur.fixed){
					fprintf(fptr, ".");
				}
				if(j == game->board_size){
					fprintf(fptr, "\n");
				}
				else{
					fprintf(fptr, " ");
				}
			}
	}
	printf("Board saved to: %s\n", fileDir);
	fclose(fptr);
}

