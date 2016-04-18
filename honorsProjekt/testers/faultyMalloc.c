#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// Compile with: gcc -std=c99 -g -w faultyMalloc.c -o faultyMalloc

#define SMALL 1 << 11

#define MIN_ALLOC_SIZE 24
#define MAX_ALLOC_SIZE 1024 * 100
#define CHANCE_OF_FREE 95
#define CHANCE_OF_REALLOC 50
#define TOTAL_ALLOCS 400000

typedef struct link_t {
	size_t size;
	struct link_t* prev;
	struct link_t* next;
	struct link_t* prevF;		
	struct link_t* nextF;
	char buff[];
} ll;

ll* head;
ll* tail;

ll* headF;
ll* tailF;

ll* freer1(ll* t) {					
	if (t -> next) {
		if ((t -> next -> size) & 1) {
			ll* n = t -> next;
			t -> size += (n -> size + sizeof(ll));
			t -> next = n -> next;			
				
			if (n -> next) 
				n -> next -> prev = t;
			else tail = t;
				
			if (n -> prevF) 
				n -> prevF -> nextF = n -> nextF;	
			else headF = n -> nextF;
			
			if (n -> nextF) 
				n -> nextF -> prevF = n -> prevF;
			else tailF = n -> prevF;			
			
			n -> nextF = n -> prevF = NULL;
				
			return t;
		}
	}
	
	(t -> size) |= 1;						
	return t;
}

ll* freer2(ll* t) {						
	if (t -> prev) {
		if ((t -> prev -> size) & 1) {
			ll* p = t -> prev;
			p -> size += (sizeof(ll) + (t -> size) ^ 1);
			p -> next = t -> next;
			
			if (t -> next) 
				t -> next -> prev = p;
			else tail = p;
				
			if (p -> prevF) 
				p -> prevF -> nextF = p -> nextF;
			else headF = p -> nextF;
			
			if (p -> nextF) 
				p -> nextF -> prevF = p -> prevF;
			else tailF = p -> prevF;			

			t -> nextF = t -> prevF = NULL;
			
			return p;
		}
	}

	return t;
}


void copy(void* src, void* dest, size_t size) {
	while (size--) {
		*(char*)dest = *(char*)src;
		++dest;
		++src;	
	}
}


void *calloc(size_t num, size_t size) {
	void* ptr = malloc(num * size);
	if (!ptr)
		memset(ptr, 0, num * size);				
	return ptr;
}


void *malloc(size_t size) {		
	void* temp = NULL;	
	if (size & 0x3)
		size = ((size >> 2) + 1) << 2;			
		
	if (!head) {
		size_t space = size;
		
		if (size < SMALL)
			space <<= 9;			
	
		temp = sbrk(sizeof(ll) + space); 
		ll* t = head = tail = (ll*)temp;
		t -> prev = t -> next = t -> prevF = t -> nextF = NULL;
		t -> size = size;
		
		if (size != space) {
			ll* m = (ll*)((void*)(t + 1) + size);
			tail = m;
			
			m -> size = space - size - sizeof(ll) + 1 /* free */;
			m -> prevF = m -> nextF = NULL;
			headF = tailF = m;
			
			t -> next = m;
			m -> prev = t;
			m -> next = NULL;
		}
		
		return (void*)(t + 1);
	}
	
	ll* t = headF;
	
	if (t) {
		if (((t -> size) & 1) && (t -> size >= size + 1)) {		
			(t -> size) ^= 1;

			if (t -> size >= size + 2 * sizeof(ll)) {
				ll* m = (ll*)((void*)(t + 1) + size);
	
				if (headF -> nextF) {
					headF = headF -> nextF;
					headF -> prevF = NULL;
					tailF -> nextF = m;
					m -> prevF = tailF;
					m -> nextF = NULL;
					tailF = m;
				}
				else {
					m -> prevF = m -> nextF = NULL;
					tailF = headF = m;
				}
				
				t -> nextF = t -> prevF = NULL;
				
				if (t -> next) 
					t -> next -> prev = m;
				else tail = m;			
				
				m -> size = t -> size - size - sizeof(ll) + 1 /* free */;
				t -> size = size;
				m -> next = t -> next;
				t -> next = m;
				m -> prev = t;
			}
			else {
				headF = headF -> nextF;
				
				t -> nextF = t -> prevF = NULL;
				
				if (headF == NULL)
					tailF = NULL;
				else headF -> prevF = NULL;
			}
		
			return (void*)(t + 1);
		}
		else {
			t = t -> nextF;
		}
	}
	
	while (t) {		
		if ((t -> size) & 1 && t -> size >= size + 1) {	
			(t -> size) ^= 1;
			if (t -> size >= size + 2 * sizeof(ll)) {
				ll* m = (ll*)((void*)(t + 1) + size);
				
				if (t -> prevF) 
					t -> prevF -> nextF = t -> nextF;	
				else headF = t -> nextF;

			
				if (t -> nextF) 
					t -> nextF -> prevF = t -> prevF;
				else tailF = t -> prevF;

				t -> nextF = t -> prevF = NULL;
				
				tailF -> nextF = m;				
				m -> prevF = tailF;
				m -> nextF = NULL;
				tailF = m;
				
				if (t -> next) 
					t -> next -> prev = m;
				else tail = m;						
				
				m -> size = t -> size - size - sizeof(ll) + 1 /* free */;
				t -> size = size;
				m -> next = t -> next;
				t -> next = m;
				m -> prev = t;
			}
			else {
				if (t -> prevF) 
					t -> prevF -> nextF = t -> nextF;		
				else headF = t -> nextF;

				if (t -> nextF) 
					t -> nextF -> prevF = t -> prevF;
				else tailF = t -> prevF;
				
				t -> nextF = t -> prevF = NULL;
			}
			
			return (void*)(t + 1);
		}
			
		t = t -> nextF;
	}

	size_t space = size;
		
	if (size < SMALL)
		space <<= 9;	

	temp = sbrk(sizeof(ll) + space); 
	t = (ll*)temp;	
	t -> prev = tail;
	tail -> next = t;
	tail = t;
	t -> next = t -> prevF = t -> nextF = NULL;
	t -> size = size;
	
	if (size != space) {
		ll* m = (ll*)((void*)(t + 1) + size);
		tail = m;
		
		if (tailF) {
			tailF -> nextF = m;
			m -> prevF = tailF;
			m -> nextF = NULL;
			tailF = m;
		}
		else {
			headF = tailF = m;
			m -> prevF = m -> nextF = NULL;
		}
		
		m -> size = space - size - sizeof(ll) + 1 /* free */;
		t -> next = m;
		m -> prev = t;
		m -> next = NULL;
	}
	
	return (void*)(t + 1);
}


void free(void *ptr) {					
	ll* t = (ll*)(ptr - sizeof(ll));
	
	if (t -> prev) {
		if ((t -> prev -> size) & 1) {
			ll* p = t -> prev;		
			p -> size += (t -> size + sizeof(ll));
			p -> next = t -> next;
			
			if (p -> prevF) 
				p -> prevF -> nextF = p -> nextF;	
			else headF = p -> nextF;
			
			if (p -> nextF) 
				p -> nextF -> prevF = p -> prevF;
			else tailF = p -> prevF;			
			
			if (t -> next) {
				if ((t -> next -> size) & 1) {
					ll* n = t -> next;
					p -> size += (sizeof(ll) + (n -> size) ^ 1);
					p -> next = n -> next;
					
					if (n -> next) 
						n -> next -> prev = p;
					else tail = p;
					
					if (n -> prevF) 
						n -> prevF -> nextF = n -> nextF;	
					else headF = n -> nextF;
			
					if (n -> nextF) 
						n -> nextF -> prevF = n -> prevF;
					else tailF = n -> prevF;			
					
					n -> nextF = n -> prevF = NULL;
					
					if (tailF) {
						tailF -> nextF = p;
						p -> prevF = tailF;
						tailF = p;
						p -> nextF = NULL;
					}
					else {
						headF = tailF = p;
						p -> prevF = p -> nextF = NULL;
					}
					
					t -> prevF = t -> nextF = NULL;
					
					return;
				}
				else {
					t -> next -> prev = p;
				}
			}
			else {
				tail = p;
			}

			if (tailF) {
				tailF -> nextF = p;
				p -> prevF = tailF;
				tailF = p;
				p -> nextF = NULL;
			}
			else {
				headF = tailF = p;
				p -> prevF = p -> nextF = NULL;
			}
			
			t -> prevF = t -> nextF = NULL;
			
			return;
		}
	}
	
	if (t -> next) {
		if ((t -> next -> size) & 1) {
			ll* n = t -> next;
			t -> size += (n -> size + sizeof(ll));
			t -> next = n -> next;	
			
			if (n -> next) 
				n -> next -> prev = t;
			else tail = t;		

			if (n -> prevF) 
				n -> prevF -> nextF = n -> nextF;
			else headF = n -> nextF;
	
			if (n -> nextF) 
				n -> nextF -> prevF = n -> prevF;
			else tailF = n -> prevF;			
			
			n -> nextF = n -> prevF = NULL;
			
			if (tailF) {
				tailF -> nextF = t;
				t -> prevF = tailF;
				tailF = t;
				t -> nextF = NULL;
			}
			else {
				headF = tailF = t;
				t -> prevF = t -> nextF = NULL;
			}		
							
			return;
		}
	}
	
	(t -> size) |= 1;				
			
	if (tailF) {
		tailF -> nextF = t;
		t -> prevF = tailF;
		tailF = t;
		t -> nextF = NULL;
	}
	else {
		headF = tailF = t;
		t -> prevF = t -> nextF = NULL;
	}
}


void *realloc(void *ptr, size_t size) {			
	if (!ptr) {
		ptr = malloc(size);
		return ptr;
	}
	
	if (ptr && !size) {
		free(ptr);
		return NULL;
	}
	
	if (size & 0x3)
		size = ((size >> 2) + 1) << 2;

	ll* t = (ll*)(ptr - sizeof(ll));
	size_t origSize = t -> size;
/*	
	if (size <= origSize) {
		if (origSize >= size + 2 * sizeof(ll)) {
			ll* m = (ll*)((void*)(t + 1) + size);
			
			if (tailF) {
				tailF -> nextF = m;
				m -> prevF = tailF;
				m -> nextF = NULL;
				tailF = m;
			}
			else {
				headF = tailF = m;
				m -> prevF = m -> nextF = NULL;
			}
			
			if (t -> next) 
				t -> next -> prev = m;		
			else tail = m;							
			
			m -> size = origSize - size - sizeof(ll) + 1;
			t -> size = size;
			m -> next = t -> next;
			t -> next = m;
			m -> prev = t;
		}
		
		return (void*)(t + 1);
	}
	
	ll* k = freer1(t);
	if (k -> size >= size + 1) {
		(k -> size) ^= 1;
		if (k -> size >= size + 2 * sizeof(ll)) {
			ll* m = (ll*)((void*)(k + 1) + size);
			
			if (tailF) {
				tailF -> nextF = m;
				m -> prevF = tailF;
				m -> nextF = NULL;
				tailF = m;
			}
			else {
				headF = tailF = m;
				m -> prevF = m -> nextF = NULL;
			}
			
			if (k -> next) 
				k -> next -> prev = m;
			else tail = m;							
			
			m -> size = k -> size - size - sizeof(ll) + 1;
			k -> size = size;
			m -> next = k -> next;
			k -> next = m;
			m -> prev = k;
		}
		
		return (void*)(k + 1);
	} 
	
	k = freer2(t);
	
	if (k -> size >= size + 1) {	
		copy(ptr, k + 1, origSize);
		(k -> size) ^= 1;
		if (k -> size >= size + 2 * sizeof(ll)) {
			ll* m = (ll*)((void*)(k + 1) + size);

			if (tailF) {
				tailF -> nextF = m;
				m -> prevF = tailF;
				m -> nextF = NULL;
				tailF = m;
			}
			else {
				headF = tailF = m;
				m -> prevF = m -> nextF = NULL;
			}
			
			if (k -> next) 
				k -> next -> prev = m;
			else tail = m;				
				
			m -> size = k -> size - size - sizeof(ll) + 1;
			k -> size = size;
			m -> next = k -> next;
			k -> next = m;
			m -> prev = k;
		}
		
		return (void*)(k + 1);
	}
	
	if (tailF) {
		tailF -> nextF = k;
		k -> prevF = tailF;
		k -> nextF = NULL;
		tailF = k;
	}
	else {
		headF = tailF = k;
		k -> prevF = k -> nextF = NULL;
	}

	void* p = malloc(size);
	copy(ptr, p, origSize); 
	return p;
*/

	free(ptr);
	void* p = malloc(size);
	copy(ptr, p, origSize);
	return p;	
}


int main() {
	int i;
	void *realloc_ptr = NULL;
	void **dictionary = malloc(TOTAL_ALLOCS * sizeof(void *));
	int *dictionary_elem_size = malloc(TOTAL_ALLOCS * sizeof(int));
	int dictionary_ct = 0;
	int data_written = 0;

	for (i = 0; i < TOTAL_ALLOCS; i++) {
		int size =
		(rand() % (MAX_ALLOC_SIZE - MIN_ALLOC_SIZE + 1)) + MIN_ALLOC_SIZE;
		void *ptr;

		if (realloc_ptr == NULL) {
			ptr = malloc(size);
			data_written = 0;
		} else {
			ptr = realloc(realloc_ptr, size);
			realloc_ptr = NULL;
		}

		if (ptr == NULL) {
			printf("Memory failed to allocate!\n");
			return 1;
		}

		if (rand() % 100 < CHANCE_OF_FREE)
			free(ptr);
		else {
			if (!data_written) {
				*((void **)ptr) = &dictionary[dictionary_ct];
				data_written = 1;
			}

			if (rand() % 100 < CHANCE_OF_REALLOC)
				realloc_ptr = ptr;
			else {
				*((void **)(ptr + size - sizeof(void *))) = &dictionary[dictionary_ct];
				dictionary[dictionary_ct] = ptr;
				dictionary_elem_size[dictionary_ct] = size;
				dictionary_ct++;
			}
		}
	}

	for (i = dictionary_ct - 1; i >= 0; i--) {
		if (*((void **)dictionary[i]) != &dictionary[i]) {
			printf("Memory failed to contain correct data after many allocations "
			"(beginning of segment)!\n");
			return 100;
		}

		if (*((void **)(dictionary[i] + dictionary_elem_size[i] -
			sizeof(void *))) != &dictionary[i]) {
			printf("Memory failed to contain correct data after many allocations "
			"(end of segment)!\n");
			return 101;
		}

		free(dictionary[i]);
	}

	printf("Memory was allocated and freed!\n");
	return 0;

}
