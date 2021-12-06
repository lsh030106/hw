/*READ FILE/ sort them*/

#include "merge.h"
#include <string.h>
#include <unistd.h>

#define HELP "-h : look at the instructions"
#define ARGFILE "-f filename : filename of testset for execute merge-sort"

void validation(list *p_li, FILE *);
char * getfilename(int argc, char **argv);

int main(int argc, char **argv) {
	clock_t start, end;
	float res;
	
	char temp[255];
	char *c, *filename;
	FILE *fd, *fdo;
	int fargc, left = 0;
	
	if (argc < 2) {
		printf("%s\n\n%s\n", HELP, ARGFILE);
		return 1;
	}

	filename = getfilename(argc, argv);
	if (filename == NULL) {
		return 1;
	}

	list *a = newList(0, NULL);
	fd = fopen(filename, "r");
	fdo = fopen("varset/Result_set.txt", "w");
	
	if (fd == NULL) {
		printf("not find file (%s), check the your path\n", filename);
		return 1;
	}

	while (! feof(fd)) {
		fgets(temp, 255, fd);
		c = strtok(temp, " ");

		while (c != NULL) {
			fargc++;	
			appendNode(a, atoi(c));
			c = strtok(NULL, " ");
		}
	}
	
	printf("%d\n", fargc - 1);
	
	start = clock();
	merge_sort(a, left, fargc-1);
	end = clock();
	res = (float)(end - start)/CLOCKS_PER_SEC;
	
	fclose(fd);
	validation(a, fdo);
	fclose(fdo);
	cleanUp(a);

	printf("Merge time required : %f sec\n", res);

	return 0;
}
	
char * getfilename(int argc, char **argv) {
	int c;
	
	while ((c = getopt(argc, argv, "hf:")) != -1) {

		switch(c) {
			case 'h':
				break;

			case 'f':
				return optarg;

			case '?':
				break;

			default:
				break;
		}
	}

	printf("%s\n\n%s\n", HELP, ARGFILE);
	return NULL;
}
	
void validation(list *p_li, FILE *fp) {
	int i = 0;
	node *curr = p_li->head->next;
	
	while (1) {
		i++;
		if ((i % 30) == 0) {
			fprintf(fp, "\n");
		}
		
		fprintf(fp, "%d", curr->data);
		curr = curr->next;
		
		if (curr == NULL) {
			break;
		}
		fprintf(fp, " ");
	}
}
