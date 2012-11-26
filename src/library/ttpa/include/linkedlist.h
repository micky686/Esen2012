/*==================================\
| linkedlist  						|
\===================================/

File:      linkedlist.h
Author:    Gernot Klingler
E-Mail:    gernotk@gmx.at
Version:   1.0
Datum:     10.3.07
      
****************************************/

#ifndef __LINKEDLIST_H
#define __LINKEDLIST_H

typedef struct data{
	int rodl;
	int slot;
} data_t;

typedef struct item{
  struct item *next;
  data_t data;
} item_t;

typedef struct {
	item_t *start;
	item_t *end;
	int itemcount;
} list_t;

void InitList(list_t *list);
void InsertAtBeginning (data_t datum, list_t *liste);
item_t* GetItemAt(list_t *liste, int n);
void InsertAtEnd(data_t datum, list_t *liste);
item_t* DeleteItem(item_t *elem, list_t *liste);

#endif
