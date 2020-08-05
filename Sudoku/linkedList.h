/*This module is responsible for the functions related to the MoveHistory doubly linked list.
 *Each list node represents a move and stores all the related information to it.*/

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdio.h>
#include <stdlib.h>

/*(list node)*/
typedef struct _Move{
	int row;
	int column;
	int invalid;
	int before_value;
	int after_value;
	int fixed_before;
	int fixed_after;
	struct _Move *next;
	struct _Move *prev;
	int move_start;
	int edge;
} Move;

/*doubly linked list*/
typedef struct MoveList{
		Move *cur_move;
} MoveList;

/*creates a new linked list with an edge as it's first and only node*/
MoveList* createList();

/*deletes the list and frees all related memory to it*/
void clearList(MoveList *list);

/*adds a new move to the list*/
void insertToList(MoveList *list, int row, int column, int invalid, int before_value, int after_value, int fixed_before, int fixed_after, int move_start);

/*returns a pointer to the move that needs to be undone and moves the list pointer accordingly*/
Move* undo(MoveList *list);

/*returns a pointer to the move that needs to be redon and moves the list pointer accordingly*/
Move* redo(MoveList *list);


#endif /* LINKEDLIST_H_ */
