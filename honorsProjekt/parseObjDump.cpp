#include <bits/stdc++.h>

using namespace std;

vector<string> objectDump;

void execObjDump(int argc, char* argv[]) {			// these arguments are the exact same as argc and argv
	string temp = argv[1];
	temp = temp.substr(2); 
	temp += "_copy.dump";		
	const char* filename = temp.c_str();

	pid_t objdump = fork();

	if ( !objdump ) {	
		int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	 	dup2(fd, 1);   // make stdout go to file
    close(fd);     // fd no longer needed - the dup'ed handles are sufficient
    
		char* objdump_args[5];
	 
		objdump_args[0] = "objdump";
		objdump_args[1] = "-S";
		objdump_args[2]	= "-d";
		objdump_args[3]	= argv[1];
		objdump_args[4] = NULL; 
		
		int k = execvp(objdump_args[0], objdump_args);
		
		if ( k == -1 ) {
			cout << "Execution for object dump failed\n";
			exit(420);
		}
	}
	
	int status;
	waitpid(objdump, &status, 0);
	
	
	//				*** PARSING PART ***				*** PARSING PART ***
	
	// you can do this with a different method if you want to
	
	// open file
	ifstream parseMe(filename);		// got from online - cppreference
	if (parseMe.is_open()) {
		string line;
		
		while (getline(parseMe, line)) 
			objectDump.push_back(line);
		
		parseMe.close();
	}
	
	// now the vector objectDump has all the lines
	
}
