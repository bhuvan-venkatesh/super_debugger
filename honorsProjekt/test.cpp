#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>
#include <iomanip>

#include <algorithm>
#include <utility>
#include <tuple>
#include <string>
#include <iterator>
#include <regex>
#include <bitset>

#include <vector>
#include <set>
#include <map>
#include <stack>
#include <queue>
#include <list>

using namespace std;

int main() {
	// mixing up new with free -- bad practice
	int* p = new int();
	*p = 0;
	free(p);
	
	// did not allocate enough memory
	p = (int*)malloc(2);
	*p = 1024;
	
	cout << *p << "\n";
	
	// mixing up malloc with delete
	delete(p);
	
	// double free
	//free(p);
	
	// did not free
	char* c = new char();
	*c = 'f';
	
	// no memory
	*p = 10;

	cout << *p << "\n";
	
	return 0;
}
