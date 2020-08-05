/*This module is responsible for the functions related to importing and exporting the game
 * to and from a file.*/
#ifndef FILEPARSING_H_
#define FILEPARSING_H_

/*the function saves the current game to the save directory provided to it*/
void saveToFile(Game *game, char* fileDir);

/*imports a game board from fileDir, returns it if successful and NULL otherwise*/
Game* readFromFile(char* fileDir, int check_errors);

#endif /* FILEPARSING_H_ */
