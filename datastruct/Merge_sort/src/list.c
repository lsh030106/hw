#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

typedef struct _NODE {
	int data;
	struct _NODE *next;
}node;

typedef struct _LIST {
	node *head;
	node *tail;
}list;

void errorHandle(const char *);
void currNode(list *);
void cleanUp(list *);
void replaceNode(list *, int, int);
void replaceFront(list *p_li, int t, int r);
int indexData(list *, int);
int appendNode(list *, int);
int insertNode(list *, int, int);
int deleteNode(list *, int);
node *indexNode(list *, int);
list *newList(int, char **);

list *newList(int argc, char **argv) {
	
	node *head = (node *)malloc(sizeof(node));
	list *li = (list *)malloc(sizeof(list));

	if (head == NULL || li == NULL) {
		errorHandle("Not Enough Memory");
		return NULL;
	}

	head->next = NULL;
	node *tail = head;

	li->head = head;
	li->tail = tail;
	
	if (argc != 0 && argv != NULL) {
		
		for (int i = 1; i < argc; i++) {
			appendNode(li, atoi(argv[i]));
		}
	}
	
	return li;
}

void replaceFront(list *p_li, int t, int r) {
	
	node *ntargetHead;
	node *nrepHead;
	node *temp;
	node *temp_;

	ntargetHead = indexNode(p_li, t - 1);
	nrepHead = indexNode(p_li, r - 1);
	
	temp = nrepHead->next;
	nrepHead->next = nrepHead->next->next;

	temp_ = ntargetHead->next;
	
	ntargetHead->next = temp;
	temp->next = temp_;
	
}

void replaceNode(list *p_li, int t, int r) {
	node *temp_t;
	node *temp_r;
	node *temp;

	temp_t = indexNode(p_li, t - 1);
	temp_r = indexNode(p_li, r - 1);
	
	temp = temp_t->next;
	temp_t->next = temp_r->next;
	temp_r->next = temp;

	temp = temp_t->next->next;
	temp_t->next->next = temp_r->next->next;
	temp_r->next->next = temp;
}

int insertNode(list *p_li, int idx, int data) {
	
	node *temp;
	node *n_node = (node *)malloc(sizeof(node));
	if (temp == NULL) {
		return 0;
	}

	n_node->data = data;

	temp = indexNode(p_li, idx);
	if (temp == NULL) {
		return 0;
	}

	n_node->next = temp->next;
	temp->next = n_node;

	if (n_node->next == NULL) {
		p_li->tail = n_node;
	}

	return 1;
}

int deleteNode(list *p_li, int idx) {

	node *temp;
	node *t_node;

	temp = indexNode(p_li, idx);
	if (temp == NULL) {
		return 0;
	}

	t_node = temp->next;
	temp->next = t_node->next;
	free(t_node);

	return 1;
}

// Need to add other algorithm 
node *indexNode(list *p_li, int idx) {
	
	node *temp = p_li->head;

	for (int i = 0; i < idx + 1; i++) {
		temp = temp->next;
		if (temp == NULL) {
			return NULL;
		}
	}

	return temp;
}

void currNode(list *p_li) {

	node *curr = p_li->head->next;

	while (1) {
		printf("%d", curr->data);
		curr = curr->next;
		
		if (curr == NULL) {
			break;
		}
		printf(", ");
	}
	printf("\n");
}

int indexData(list *t_li, int n) {
	
	node *temp = t_li->head->next;

	for (int i = 0; i < n; i++) {
		temp = temp->next;
		if (temp == NULL) {
			printf("list out of range\n");
			return 0;
		}
	}
	
	return temp->data;	
}

int appendNode(list *p_li, int n) {
	
	node *n_node = (node *)malloc(sizeof(node));
	if (n_node == NULL) {
		errorHandle("Not Enough Memory");
		return 0;
	}

	n_node->next = p_li->tail->next;
	n_node->data = n;
	p_li->tail->next = n_node;
	p_li->tail = n_node;

	return 1;
}

void cleanUp(list *p_li) {
	node *curr = p_li->head->next;
	node *temp;

	while (curr != NULL) {
		temp = curr->next;
		free(curr);
		curr = temp;
	}
}

void errorHandle(const char *ErrorMsg) {
	
	fprintf(stderr, "%s : %d\n", ErrorMsg, errno);
}
