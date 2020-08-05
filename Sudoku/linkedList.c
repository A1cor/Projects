/*This module is responsible for the functions related to the MoveHistory doubly linked list.
 *Each list node represents a move and stores all the related information to it.*/


#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"

/*creates an empty move which indicates that this is the edge of the linked list and returns it*/
Move* createEdge(){
	Move *move = (Move*)malloc(sizeof(Move));
	if(move == NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	move->edge = 1;
	move->next = NULL;
	move->prev = NULL;

	return move;
}

/*creates a new linked list with an edge as it's first and only node*/
MoveList* createList(){
	MoveList *list = NULL;
	list = (MoveList*)malloc(sizeof(MoveList));
	if(list == NULL){
			printf("ERROR: problem with memory allocation\n");
			exit(0);
	}
	list->cur_move = createEdge();
	return list;
}

/*receives move information, creates a move node and returns it*/
Move* createMove(int row, int column, int invalid, int before_value, int after_value, int fixed_before, int fixed_after, int move_start){
	Move *move = (Move*)malloc(sizeof(Move));
	if(move == NULL){
		printf("ERROR: problem with memory allocation\n");
		exit(0);
	}
	move->column = column;
	move->row = row;
	move->fixed_before = fixed_before;
	move->fixed_after = fixed_after;
	move->move_start = move_start;
	move->invalid = invalid;
	move->before_value = before_value;
	move->after_value = after_value;
	move->next = NULL;
	move->prev = NULL;
	move->edge = 0;
	return move;
}

/*deletes the list and frees all related memory to it*/
void clearList(MoveList *list){
	Move *temp = list->cur_move;
	Move *prev;
	if(temp == NULL){
		goto skip;
	}
	while(temp->prev != NULL){
		temp = temp->prev;
	}
	while(temp != NULL){
		prev = temp;
		temp = temp->next;
		free(prev);
	}
	skip: free(list);
}

/*adds a new move to the list*/
void insertToList(MoveList *list, int row, int column, int invalid, int before_value, int after_value, int fixed_before, int fixed_after, int move_start){
	Move *temp;
	Move *del;
	Move *del_temp;
	Move *move = createMove(row, column, invalid, before_value, after_value, fixed_before, fixed_after, move_start);
	temp = list->cur_move;
	while(temp->next != NULL && !temp->next->move_start){
		temp = temp->next;
	}
	del = temp->next;
	while(del != NULL){
		del_temp = del->next;
		free(del);
		del = del_temp;
	}
	temp->next = move;
	move->prev = temp;
	if(move->move_start){
		list->cur_move = move;
	}
}

/*returns a pointer to the move that needs to be undone and moves the list pointer accordingly*/
Move* undo(MoveList *list){
	Move* ret = list->cur_move;
	if(ret->edge){
		return NULL;
	}
	list->cur_move = list->cur_move->prev;
	while(!list->cur_move->move_start && !list->cur_move->edge){
		list->cur_move = list->cur_move->prev;
	}
	return ret;
}

/*returns a pointer to the move that needs to be redon and moves the list pointer accordingly*/
Move* redo(MoveList *list){
	Move *temp = list->cur_move;
	temp = temp->next;
	if(temp == NULL){
		return NULL;
	}
	while(!temp->move_start){
		temp = temp->next;
		if(temp == NULL){
			return NULL;
		}
	}
	list->cur_move = temp;
	return temp;
}


