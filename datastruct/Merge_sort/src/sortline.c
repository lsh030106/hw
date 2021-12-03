/*READ FILE/ sort them*/

#include "merge.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
	
	char temp[255];
	char *c;
	FILE *fd;
	int argc, left = 0;
	

	list *a = newList(0, NULL);
	fd = fopen("testfield.txt", "r");

	while (! feof(fd)) {
		fgets(temp, 255, fd);
		c = strtok(temp, " ");

		while (c != NULL) {
			argc++;	
			appendNode(a, atoi(c));
			c = strtok(NULL, " ");
		}
	}
	
	printf("%d\n", argc - 1);
	merge_sort(a, left, argc-1);
	fclose(fd);
	currNode(a);
	cleanUp(a);
	
	return 0;
}


