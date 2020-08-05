/*this module includes all functions that create gurobi env and use array belongs to gurobi model*/

#ifndef GUROBI_H_
#define GUROBI_H_

/*creates a gurobi environment. */
int createEnv();

/*gurobi wont print messages*/
int noMessages();

/* Create an empty model named "mip1" */
int createEmptyModel();

/*fills the object array*/
int createObjAndSendToModel(Game *game, int ILP);

/*sends the arrays to the model*/
int sendToModel();

/*sets cell constraints (every cell should be filled with exactly one value)*/
int setCellConstraints(Game *game);

/*sets row constraints*/
int setRowConstraints(Game *game);

/*sets column constraints*/
int setColumnConstraints(Game *game);

/*sets block constraints*/
int setBlockConstraints(Game *game);

/*bounds the variables between 0 and 1*/
int createBounds();

/* Optimize model - need to call this before calculation */
int optimizeTheModel();

/*gests the solution*/
int getSol();

/* Gets solution information */
Game *findHintBoard(Game *game);

/*recieves a cell index and finds a hint*/
int getHint(int row, int column, Game *game);

/*fills the board with guesses with probabilities higher than X*/
void fillAllGuesses(Game *game,float X, int start);

/*prints the guesses corresponding to the received index*/
void printGuessHint(Game *game, int row, int column);

/* Free model and environment */
void freeGR(int sol);

void freelbub();

#endif /* GUROBI_H_ */
