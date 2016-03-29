#include "parseValgrind.cpp"

using namespace std;

/*
 *		
 *
 *
 *
 *
 */


/*
 *		Must have object file and normal file in the same location	
 *
 *		Should have a modern version of valgrind/linux installed
 *
 *		Option to run with -f or -force to force continue
 *		./super_debugger -f ./yourProgram
 */ 
 
/*

struct errors {
  string errorMessage;
  vector< tuple<string, string, int> > description;
};

vector<string> memory;
vector<errors> totalErrors;
tuple<string, string, int> desLines;
vector<errors> dividedErrors[9];


									***  Errors  *** 
0 - SIGSEGV
1 - Invalid read
2 - Invalid write
3 - Invalid free
4 - Mismatched free
5 - Source and destination		--- 	can handle right away
6 - Conditional jump
7 - Syscall										---		can handle right away	
8 - Argument fishy 						---		can handle right away 
 
*/  


int main(int argc, char* argv[]) {
	// execute and parse valgrind output	
	int k = execValgrind(argc, argv); 			
	
	if ( k ) {							
		if ( k == 1 )
			cerr << "\e[1mSomething bad happened --- Debugger did not run\e[0m\n";
		return 0;		// return early if not forced or if something bad happened
	}

	// execute and parse object dump of program
	
	
	// run our debugger
	
	
	return 0;
}
