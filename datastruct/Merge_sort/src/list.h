#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef struct _NODE {
	int data;
	struct _NODE *next;
}node;

typedef struct _LIST {
	node *head;
	node *tail;
}list;

void errorHandle(const char *ERROR_MSG);
void currNode(list *);
void cleanUp(list  *);
int indexData(list *, int idx);
int appendNode(list *, int idx);
int insertNode(list *, int idx, int data);
int deleteNode(list *, int idx);
node *indexNode(list *, int idx);
list *newList(int, char **);
void replaceNode(list *, int, int);
