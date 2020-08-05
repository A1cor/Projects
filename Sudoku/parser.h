/*This module is responsible for the functions related to the user input.*/
#ifndef PARSER_H_
#define PARSER_H_

/*gets the command from the user and calls processCommand to process it*/
Game* getCommand(Game *game, char input[1024], char command[4][1024]);

/*returns 1 if the input string represents an integer and 0 otherwise*/
int checkIfStringIsInt(char *str);

/*converts a string representing an int to int*/
int converStringToInt(char *str);


#endif /* PARSER_H_ */
