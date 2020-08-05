/*This module is responsible for the functions related to the user input.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "structs.h"
#include "solve.h"
#include "game.h"
#include "game_gurobi.h"

/*returns 1 if the input string represents an integer and 0 otherwise*/
int checkIfStringIsInt(char *str){
	int i;
	int str_len = strlen(str);
	for(i = 0; i < str_len; i++){
		if(!isdigit(str[i])){
			return 0;
		}
	}
	return 1;
}

/*returns 1 if the input string represents a float and 0 otherwise*/
int checkIfStringIsFloat(char *str){
	int countDots = 0, i = 0;
	int str_len = strlen(str);
	for(i = 0; i < str_len; i++){
		if(str[i] == '.'){
			countDots++;
		}
		else if(!isdigit(str[i])){
			return 0;
		}
	}
	if(countDots != 1){
		return 0;
	}
	return 1;
}

/*converts a string representing a float to float*/
float convertStringToFloat(char *str){
	return atof(str);
}

/*converts a string representing an int to int*/
int converStringToInt(char *str){
	return atoi(str);
}

/*resets the input array*/
void initInput(char input[1024]){
	int i;
	for(i = 0; i < 1024; i++){
		input[i] = -1;
	}
}

/*resets the command array*/
void initCommand(char command[4][1024]){
	int i;
	int j;
	for(j = 0; j < 4; j++){
		for(i = 0; i < 1024; i++){
			command[j][i] = -1;
		}
	}
}


/*checks whether a command is valid and calls the right function for this command.*/
Game* processCommand(Game *game, char command[4][1024], int command_length){
	Game *out_game;
	int num_of_sol, fill_ret, i, set_ret;
	float converted_float;
	char *commandType = command[0];
	if(!strcmp(commandType, "solve")){
		if(command_length == 1){
			printf("ERROR: please enter a file name after solve\n");
			return game;
		}
		if(command_length > 2){
			printf("ERROR: too many parameters for solve\n");
			return game;
		}
		out_game = solve(game, command[1]);
	}
	else if(!strcmp(commandType, "edit")){
		if(command_length > 2){
				printf("ERROR: too many parameters for edit\n");
				return game;
		}
		if(command_length == 1){
			out_game = edit(game, "");
		}
		else{
			out_game = edit(game, command[1]);
		}
	}
	else if(!strcmp(commandType, "mark_errors")){
		if(game_status != Solve){
				printf("ERROR: mark_errors command is only available in Solve mode\n");
				return game;
		}
		if(command_length > 2){
			printf("ERROR: too many parameters for mark_errors\n");
			return game;
		}
		if(command_length == 1){
			printf("ERROR: please enter 1 or 0  after mark_errors\n");
			return game;
		}
		if(strcmp(command[1], "1") && strcmp(command[1], "0")){
			printf("ERROR: mark_errors parameter should be 1 or 0\n");
			return game;
		}
		else{
			if(!strcmp(command[1], "1")){
				markErrors(1);
			}
			else{
				markErrors(0);
			}
			printBoard(game);
			return game;
		}
	}
	else if(!strcmp(commandType, "print_board")){
		if(game_status != Solve){
			printf("ERROR: print_board command is only available in Solve mode\n");
			return game;
		}
		if(command_length > 1){
			printf("ERROR: print_board command doesn't have parameters\n");
			return game;
		}
		printBoard(game);
		return game;
	}
	else if(!strcmp(commandType, "num_solutions")){
		if(game_status != Solve && game_status != Edit){
				printf("ERROR: num_solutions command is only available in Solve or Edit mode\n");
				return game;
		}
		if(command_length > 1){
					printf("ERROR: print_board command doesn't have parameters\n");
					return game;
				}
				else{
					 num_of_sol = countSolutions(game);
					 printf("The number of possible solutions to this board is: %d\n", num_of_sol);
					 return game;
				}
	}
	else if(!strcmp(commandType, "autofill")){
		if(game_status != Solve){
				printf("ERROR: autofill command is only available in Solve mode\n");
				return game;
		}
		if(command_length > 1){
				printf("ERROR: autofill command doesn't have parameters\n");
				return game;
		}
		fill_ret = autoFillBoard(game);
		if(fill_ret){
			printBoard(game);
		}
		return game;
	}
	else if(!strcmp(commandType, "exit")){
			if(command_length > 1){
					printf("ERROR: exit command doesn't have parameters\n");
					return game;
			}
			exitGame(game);
	}
	else if(!strcmp(commandType, "reset")){
			if(game_status != Solve && game_status != Edit){
				printf("ERROR: reset command is only available in Solve or Edit mode\n");
				return game;
			}
			if(command_length > 1){
					printf("ERROR: reset command doesn't have parameters\n");
					return game;
			}
			resetBoard(game);
			printBoard(game);
			return game;
	}
	else if(!strcmp(commandType, "save")){
			if(game_status != Solve && game_status != Edit){
				printf("ERROR: save command is only available in Solve or Edit mode\n");
				return game;
			}
			if(command_length == 1){
				printf("ERROR: please enter a file name after save\n");
				return game;
			}
			if(command_length > 2){
				printf("ERROR: too many parameters for save\n");
				return game;
			}
			save(game, command[1]);
			return game;
	}
	else if(!strcmp(commandType, "undo")){
			if(game_status != Solve && game_status != Edit){
					printf("ERROR: undo command is only available in Solve or Edit mode\n");
					return game;
			}
			if(command_length > 1){
					printf("ERROR: undo command doesn't have parameters\n");
					return game;
			}
			if(undoMove(game)){
				printBoard(game);
			}
			return game;
	}
	else if(!strcmp(commandType, "redo")){
			if(game_status != Solve && game_status != Edit){
				printf("ERROR: redo command is only available in Solve or Edit mode\n");
				return game;
			}
			if(command_length > 1){
				printf("ERROR: redo command doesn't have parameters\n");
				return game;
			}
			if(redoMove(game)){
				printBoard(game);
			}
			return game;
	}
	else if(!strcmp(commandType, "set")){
			if(game_status != Solve && game_status != Edit){
				printf("Error: Set isn't available in Init mode\n");
				return 0;
			}
			if(command_length > 4){
				printf("ERROR: too many parameters for set, please enter 3 parameters\n");
				return game;
			}
			if(command_length == 1 || command_length == 2 || command_length == 3){
				printf("ERROR: not enough parameters for set, please enter 3 parameters\n");
				return game;
			}
			for(i = 1; i < 4; i++){
				if(!checkIfStringIsInt(command[i])){
					printf("ERROR:  parameter %d isn't an integer\n", i);
					return game;
				}
			}
			set_ret = setCell(converStringToInt(command[1]), converStringToInt(command[2]), converStringToInt(command[3]), game, 1);
			if(set_ret){
				printBoard(game);
			}
			return game;
	}
	else if(!strcmp(commandType, "validate")){
			if(game_status != Solve && game_status != Edit){
				printf("ERROR: validate command is only available in Solve or Edit mode\n");
				return game;
			}
			if(command_length > 1){
				printf("ERROR: validate command doesn't have parameters\n");
				return game;
			}
			validate(game);
			return game;
	}
	else if(!strcmp(commandType, "generate")){
				if(game_status != Edit){
						printf("ERROR: generate command is only available in Edit mode\n");
						return game;
				}
				if(command_length > 3){
					printf("ERROR: too many parameters for generate, please enter 2 parameters\n");
					return game;
				}
				if(command_length == 1 || command_length == 2){
					printf("ERROR: not enough parameters for generate, please enter 2 parameters\n");
					return game;
				}

				for(i = 1; i < 3; i++){
					if(!checkIfStringIsInt(command[i])){
						printf("ERROR:  parameter %d isn't an integer\n", i);
						return game;
					}
				}
				if(generate(game, converStringToInt(command[1]), converStringToInt(command[2]))){
					printBoard(game);
				}
				return game;
		}
	else if(!strcmp(commandType, "hint")){
				if(game_status != Solve){
						printf("ERROR: hint command is only available in Solve mode\n");
						return game;
				}
				if(command_length > 3){
						printf("ERROR: too many parameters for hint, please enter 2 parameters\n");
						return game;
				}
				if(command_length == 1 || command_length == 2){
						printf("ERROR: not enough parameters for hint, please enter 2 parameters\n");
						return game;
				}
				for(i = 1; i < 3; i++){
						if(!checkIfStringIsInt(command[i])){
							printf("ERROR:  parameter %d isn't an integer\n", i);
							return game;
						}
				}
				hint(game, converStringToInt(command[2]), converStringToInt(command[1]));
				return game;
			}
	else if(!strcmp(commandType, "guess_hint")){
			if(game_status != Solve){
					printf("ERROR: guess_hint command is only available in Solve mode\n");
					return game;
			}
			if(command_length > 3){
					printf("ERROR: too many parameters for guess_hint, please enter 2 parameters\n");
					return game;
			}
			if(command_length == 1 || command_length == 2){
					printf("ERROR: not enough parameters for guess_hint, please enter 2 parameters\n");
					return game;
			}
			for(i = 1; i < 3; i++){
					if(!checkIfStringIsInt(command[i])){
							printf("ERROR:  parameter %d isn't an integer\n", i);
							return game;
						}
					}
					guessHint(game, converStringToInt(command[2]), converStringToInt(command[1]));
					return game;
			}
	else if(!strcmp(commandType, "guess")){
			if(game_status != Solve){
					printf("ERROR: guess command is only available in Solve mode\n");
					return game;
			}
			if(command_length > 2){
					printf("ERROR: too many parameters for guess, please enter 1 parameter\n");
					return game;
			}
			if(command_length == 1){
					printf("ERROR: not enough parameters for guess, please enter 1 parameter\n");
					return game;
			}
			if(checkIfStringIsFloat(command[1]) || !strcmp(command[1], "1") || !strcmp(command[1], "0")){
					converted_float = convertStringToFloat(command[1]);
					if(converted_float >= 0 && converted_float <= 1){
							if(guess(game, convertStringToFloat(command[1]))){
										printBoard(game);
							}
					}
					else{
							printf("ERROR: guess parameter value should be between 0 and 1\n");
						}
					}
					else{
							printf("ERROR: guess parameter isn't a float\n");
						}
						return game;
			}
	else{
		printf("ERROR: invalid command\n");
		return game;
	}
	if(out_game != NULL){
		printBoard(out_game);
		return out_game;
	}
	return game;
}

/*gets the command from the user and calls processCommand to process it*/
Game* getCommand(Game *game, char input[1024], char command[4][1024]){
	Game *out;
	char* token;
	int command_index = 0;
	const char delim[] = " \t\r\n";
	start:
	printf("Please enter a command:\n");
	initInput(input);
	initCommand(command);
	fflush(stdin);
	if(feof(stdin)){
		exitGame(game);
	}
	fgets(input, 1024, stdin);
	if(feof(stdin)){
		exitGame(game);
	}
	if(strlen(input) > 256){
		printf("ERROR: input is too long (longer than 256 characters):\n");
		return NULL;
	}
	token = strtok(input, delim);
	while(token != NULL){
		if(command_index <= 3){
			strcpy(command[command_index], token);
		}
		command_index++;
		token = strtok(NULL, delim);
	}
	if(command_index == 0){
		goto start;
	}
	out = processCommand(game, command, command_index);
	if(game_status == Solve && isWin(out)){
		changeToInit();
		printf("The board is solved! Good job! :)\n");
		printf("Game was set to Init mode\n");
	}
	return out;
}

