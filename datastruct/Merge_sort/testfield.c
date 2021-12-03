#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {

	srand((unsigned)time(NULL));
	FILE *fp;
	int num = 0;
	
	fp = fopen("../testfield.txt", "w");
	if (fp == NULL) {
		return 1;
	}

	for (int i = 1; i < 10000; i++) {
		num = rand()%100;
		if ((i % 30) == 0) {
			fprintf(fp, "\n");
		}
		fprintf(fp, "%d", num);
		fprintf(fp, " ");
	}

	fclose(fp);
}
