/*==================================\
| linkedlist  						|
\===================================/

File:      linkedlist.c
Author:    Gernot Klingler
E-Mail:    gernotk@gmx.at
Version:   1.0
Datum:     10.3.07
      
****************************************/


#include <stdlib.h>

#include "linkedlist.h"


void InsertAtBeginning (data_t datum, list_t *list)
{
	item_t *neues_element = (item_t*) malloc(sizeof(item_t));

	neues_element->data = datum;
	neues_element->next = list -> start; 

	if(!list->start)
		list->end = neues_element;

  	list->start = neues_element;
  	list->itemcount++;
}


item_t* GetItemAt(list_t *list, int n)
{
	if( n > list->itemcount )
	{	return NULL;
	}
	
	item_t *cursor = list->start;

	while( n-- && cursor ){
		cursor = cursor->next;
	}
	if(!cursor)
		return NULL;
	return cursor;
}


void InsertAtEnd(data_t datum, list_t *list)
{
	item_t *neues_element = (item_t*) malloc(sizeof(item_t));

	neues_element->data = datum;
	neues_element->next = NULL; 

	if(!list->start)
		list->start = neues_element;
	else
	  	list->end->next = neues_element;
  	
  	list->end = neues_element;
  	
  	list->itemcount++;
}


item_t* DeleteItem(item_t *elem, list_t *list)
{
	item_t *cursor = list->start;
   
	if (list->start == elem)
	{
		list->start = elem->next;
		cursor = list->start;
		
		if(!list->start)
			list->end = NULL;
			
		list->itemcount--;
	}
	else
	{
		while(cursor && cursor->next != elem)
			cursor = cursor->next;
		
		if(!cursor) /* element not found */
			return NULL;
			
		cursor->next = elem->next; 	
		
		if (elem == list->end)
			list->end = cursor;
			
		list->itemcount--;
	}
	free(elem);
	return cursor;
}


void InitList(list_t *list)
{	list->start = NULL;
	list->end = NULL;
	list->itemcount = 0;
}







