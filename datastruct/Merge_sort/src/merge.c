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
	if (left == right) {
		return;
	}

	mid = (left + right) / 2;
	merge_sort(b_list, left, mid);
	merge_sort(b_list, mid + 1, right);
	merge(b_list, left, mid, right);
		
}

void merge(list *b_list, int left, int mid, int right) {
	
	int Lleft = left;
	int Rleft = mid + 1;
	
	while (Lleft <= mid && Rleft <= right) {
		
		if (indexData(b_list, Lleft) >= indexData(b_list, Rleft)) {

			replaceFront(b_list, Lleft++, Rleft++);
			mid++;
		}
		else {
			Lleft++;
		}

	}

}
	
	
