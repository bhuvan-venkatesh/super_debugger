#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

// *** Run with: gcc -g -std=c99 test.c -o test


char *strdup(const char *s) { 
    char *copy = malloc(strlen(s));  				// error
    strcpy(copy, s);  
    return copy;
}

int main() {
	char* orig = "Where The Streets Have No Name";
	char* copy = strdup(orig);
	copy[strlen(orig)] = '\n';						// error
	//printf("%s", copy);
	
	char* pleaseOverlap = malloc(21);
	strcpy(pleaseOverlap, "With Or Without You");
	pleaseOverlap[strlen(pleaseOverlap)] = '\n';
	//printf("%s%s", copy, pleaseOverlap);
	
	free(copy);
	free(pleaseOverlap);
	
	char* start = copy;
	*copy = 'O';								// error
	++copy;
	*copy = 'n';
	++copy;
	*copy = 'e';
	++copy;
	*copy = '\n';
	++copy;
	*copy = '\0';
	//printf("%s", start);
	
	int** p = malloc(6 * sizeof(int*));
	for (int i = 0; i < 6; ++i) 
		p[i] = calloc(6, sizeof(int));
	
	p[0][0];
	for (int i = 1; i < 6; ++i) {
		p[i][0] = 1;
		
		for (int j = 1; j < 6; ++j)
			p[i][j] = p[i - 1][j - 1] + p[i - 1][j];
	}
	
	for (int i = 0; i < 6; ++i) {	
		for (int j = 0; j <= 6; ++j)				// error
			//printf("%i   ", p[i][j]);
		//printf("\n");
		
		free(p[i]);
	}
	
	free(p);

	//printf("%s", start);
	//printf("%s", pleaseOverlap);
	
	char* string = malloc(10);
	write(1 /* stdout */, string, 10);
	char* error = string + 1;
	strcpy(error, "overwrite");
	strcpy(string, error);
	free(string);
	
	long long hugeSize = 1LL << 63;
	int* tooBig = malloc(hugeSize);
	
	for (int i = 0; i < 10; ++i) {
		string[i] = 'c';
		free(string);
	}
	
	*tooBig = 1 << 20;
	//printf("%d\n", tooBig);
}
