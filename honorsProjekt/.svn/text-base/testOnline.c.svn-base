/* 
 * This program takes a sample of the specified size (SIZE_OF_SAMPLES)
 * from integers between 0 and a MAX value (ends inclusive) without
 * replacement.  Contains bugs to practice debugging.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX             6
#define SIZE_OF_SAMPLES 3
#define REP             5

int MemberQ(int *array, int arrSize, int x);
void UniqRandInt(int max, int howmany, int * resultPtr);

int main(void) {
	int arr[SIZE_OF_SAMPLES], i, j, max, size;

	srand(9809);
	max = MAX;
	size = SIZE_OF_SAMPLES;

	for (i = 0; i < REP; i++) {
		UniqRandInt(max, size, arr);
		for(j = 0; j < size; j++) 
			printf("%d ", arr[i]);
		printf("\n");
	}
	return 0;
}

/*
 * resultPtr[] will be initialized with a sample of the specified size
 * (n) from integers between 0 and a max value (ends inclusive)
 * without replacement.
 */
void UniqRandInt(int max, int n, int *resultPtr) {
	int cntr = 0, r;

	while(cntr < n) {
		r = rand();
		r = r % (max + 1);
		if (MemberQ(resultPtr, cntr, r)) {
			resultPtr[cntr] =  r;
			cntr++;
		}
	}
	return;
}

/*
 * Check if an integer x is included in array[] of arrSize.
 * Returns: 1 if x is a member
 *          0 if x is not a member
 */
int MemberQ(int *array, int arrSize, int x) {
	int i;

	for (i = 0; i < arrSize; i++) {
		if (array[i] == x) 
			return 1;
	}
	return 0;
}
