#include "list.h"

void merge_sort(list *, int, int);
void merge(list *, int, int, int);

int main(int argc, char **argv) {
	
	list *b_list = newList(argc, argv);
	int left = 0;
	int right = argc - 2;

	merge_sort(b_list, left, right);
	currNode(b_list);
	
	return 0;
}

void merge_sort(list *b_list, int left, int right) {
	int mid;

	if (left < right) {
		mid = (left + right) / 2;

		merge_sort(b_list, left, mid);
		merge_sort(b_list, mid + 1, right);
		merge(b_list, left, mid, right);
	}
}

void merge(list *b_list, int left, int mid, int right) {
	
	int pivot = mid + 1;
	int lleft = left;
	int lright = right;
	int lmid = mid;
	
	while (pivot <= right) {
		
		if (indexData(b_list, lleft) > indexData(b_list, pivot)) {

			replaceNode(b_list, lleft, pivot++);
		}
		else {
			lleft++;
		}

	}
		
}
	
	
