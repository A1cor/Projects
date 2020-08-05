#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "solve.h"
#include "structs.h"
#include "fileParsing.h"
#include "parser.h"
#include "game.h"

int main(){
		char input[1024];
		char command[4][1024];
		Game *game = NULL;
		changeToInit();
		printf("Noa and Tal's sudoku :D\n");
		while(1){
			game = getCommand(game, input, command);

		}
		if(game){

		}
		return 0;
}

