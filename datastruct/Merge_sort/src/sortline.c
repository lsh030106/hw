/*READ FILE/ sort them*/

#include "merge.h"
#include <string.h>

int main(void) {
	
	clock_t start, end;
	float res;

	char temp[255];
	char *c;
	FILE *fd;
	int argc, left = 0;
	

	list *a = newList(0, NULL);
	fd = fopen("testfield_100000.txt", "r");

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
	
	start = clock();
	merge_sort(a, left, argc-1);
	end = clock();
	res = (float)(end - start)/CLOCKS_PER_SEC;

	fclose(fd);
	currNode(a);
	cleanUp(a);
 	
	printf("Merge time required : %f sec\n", res);

	return 0;
}


