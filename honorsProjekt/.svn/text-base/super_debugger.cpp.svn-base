#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/stat.h>

#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/reg.h>

#include "parseValgrind.cpp"
#include "parseObjDump.cpp"
#include "tryingPtrace.cpp"

using namespace std;


// Can have option to compile their file manually so they do not need to know compiler flags 

/*
 *		Needs to compile with -g flag
 *
 *		Should have a modern version of valgrind/linux installed
 *
 *		Option to run with -f or --force to force continue
 *		./super_debugger -f ./yourProgram
 */ 
 
/*

			*** parseValgrind.cpp ***

struct errors {
  string errorMessage;
  vector< tuple<string, string, int> > description;
};


tuple<string, string, int> desLines;

// important
vector<errors> dividedErrors[9];
vector<errors> totalErrors;
vector<string> memory;


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

/*

			*** parseObjDump.cpp ***

struct x86 {
  string line;
  int lineNum;
  vector< vector< string > > assembly;
};


vector< string > objectDump;
vector< string > mainFunct;							// only debugging main for now
vector< string > sourceFile;
string filename;

// important
vector< x86 > x86_code;

*/

int main( int argc, char* argv[] ) {
	if ( argc < 2 ) {
		cerr << "\n\t\e[1mFormat is: ./super_debugger [-f / --force] ./your_program\e[0m\n\n";
		return 0;
	} 

	// execute and parse valgrind output	
	int k = execValgrind( argc, argv ); 			
	
	if ( k ) 							
		return 0;		// return early if not forced or if something bad happened

	// execute and parse object dump of program
	k = execObjDump( argc, argv );
	
	if ( k ) 							
		return 0;		
	
	// run our debugger
	main_debugger( argc, argv );
	
	return 0;
}
