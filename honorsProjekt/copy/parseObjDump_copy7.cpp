using namespace std;


struct x86 {
  string line;
  int lineNum;
  vector< vector<string> > assembly;
};

vector< string > objectDump;
vector< string > mainFunct;							// only debugging main for now
vector< string > sourceFile;
string filename;

// important
vector< x86 > x86_code;


// Got these online - http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring

// trim from start (in place)
static inline void ltrim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), not1(ptr_fun<int, int>(isspace))));
}

// trim from end (in place)
static inline void rtrim(string &s) {
    s.erase(find_if(s.rbegin(), s.rend(), not1(ptr_fun<int, int>(isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
static inline string ltrimmed(string s) {
    ltrim(s);
    return s;
}

// trim from end (copying)
static inline string rtrimmed(string s) {
    rtrim(s);
    return s;
}

// trim from both ends (copying)
static inline string trimmed(string s) {
    trim(s);
    return s;
}

// ------ END ------


void printData() {
	for ( int i = 0; i < x86_code.size(); ++i ) {
		cout << "--------------------------------------------------------------------------------\n\n";
		cout << "Line number in " << filename << " is: " << x86_code[i].lineNum << "\n";
		cout << "Line is: \"" << x86_code[i].line << "\"\n";
		
		cout << "Assembly code is:\n";
		vector< vector<string> > temp = x86_code[i].assembly;
		for ( int j = 0; j < temp.size(); ++j ) {
			cout << "  Parsed Output:\n";
			cout << "    Memory address is: " << temp[j][0] << "\n";
			cout << "    Instruction code is: " << temp[j][1] << "\n";
			cout << "    Instr in x86 readable format is: " << temp[j][2] << "\n";
		}
		
		cout << "\n";
	}
}


/*
int getObjDump( string file_name ) {
	ifstream parseMe( file_name );				// got from online - cppreference
	if ( parseMe.is_open() ) {
		string line;

		while ( getline(parseMe, line) ) 
			objectDump.push_back( line );

		parseMe.close();
	}
	else {
		cerr << "\n\t\e[1mObject dump file could not be opened\e[0m\n\n";
		return 1;
	}
	
	return 0;
}
*/


void getObjDump( string& file_contents ) {			
	// got online --- http://stackoverflow.com/questions/236129/split-a-string-in-c 
	stringstream ss( file_contents );
	string line;
	while ( getline(ss, line) )
		objectDump.push_back( line );
}


int getFileLines( string fileloc ) {
	ifstream source( fileloc );	
	if ( source.is_open() ) {
		string line;

		while ( getline(source, line) ) 
			sourceFile.push_back( line );

		source.close();
	}
	else {
		cerr << "\n\t\e[1mYour file --- " << filename << " --- could not be opened\e[0m\n\n";
		return 1;
	}
	
	return 0;
}


void getFileName( string fileloc ) {
	int idx = 0;

	for ( int i = fileloc.length() - 1; i >= 0; --i ) {
		if ( fileloc[i] == '/' ) {
			idx = i;
			break;
		}
	}

	filename = fileloc.substr( idx + 1 );
}


vector< string > parseStr( string code ) {
	vector< string > parsedx86;
	int k = 0;

	string addr = code.substr( 0, (k = code.find(':')) );
	trim( addr );
	parsedx86.push_back( addr );
	
	string data;
	int i;
	for (	i = k + 1; i < code.length(); ++i ) 
		if ( code[i] != '\t' )
			break;
	
	for ( ; i < code.length(); ++i ) {
		if ( code[i] != '\t' )
			data += code[i];
		else break;
	}

	trim( data );
	parsedx86.push_back( data );
	
	string instr;
	
	for ( ; i < code.length(); ++i )
		instr += code[i];
	
	trim( instr );
	parsedx86.push_back( instr );
	
	return parsedx86;
}


void parseAssem( const vector< int >& indices, const vector< int >& linenumber ) {
	int i = 0;
	
	while ( i < indices.size() - 1 ) {
		x86 temp;	
		int k = indices[i];
		int e = indices[i + 1];
		
		temp.line = trimmed( sourceFile[linenumber[i] - 1] );
		temp.lineNum = linenumber[i];
		
		while ( k < e ) {
			temp.assembly.push_back( parseStr(mainFunct[k]) );
			++k;
		}
		
		++i;
		x86_code.push_back( temp );
	}
	
	x86 temp;	
	int k = indices[i];
	
	temp.line = sourceFile[linenumber[i] - 1];
	temp.lineNum = linenumber[i];
	
	while ( k < mainFunct.size() ) {
		temp.assembly.push_back( parseStr(mainFunct[k]) );
		++k;
	}
	
	x86_code.push_back( temp );
}


int execObjDump( int argc, char* argv[] ) {			// these arguments are the exact same as argc and argv
	int argNum = 1;
	if ( !strcmp(argv[1], "-f") || !strcmp(argv[1], "--force") )
		++argNum;
/*
	string temp = argv[argNum];
	temp = temp.substr( 2 ); 
	temp += "_copy.asm";		
	const char* file_name = temp.c_str();				// convert to char array
	cout << file_name << "\n";
*/

	int readMe[2];

  if ( pipe(readMe) == -1 ) {
    cerr << "\n\t\e[1mCreational of pipe for valgrind failed\e[0m\n\n";
    return 1;	
  }

	pid_t objdump = fork();
	
	if ( objdump == -1 ) {
    cerr << "\n\t\e[1mForking failed for object dump process\e[0m\n\n";
    return 1;
  }

	if ( !objdump ) {			
		if ( close(readMe[0]) )	{						// child does not read
			cerr << "\n\t\e[1mCould not close file descriptor for objdump pipe\e[0m\n\n";
			exit(1);
		}
	
		if ( dup2(readMe[1], 1) == -1 ) {   				// make stdout go to file
			cerr << "\n\t\e[1mCould not redirect stdout to our file for objdump pipe\e[0m\n\n";
			exit( 1 );
		}
		
		/*
		if ( close(fd) ) {     			// fd no longer needed - the dup'ed handles are sufficient
			cerr << "\n\t\e[1mCould not close file descriptor for objdump pipe\e[0m\n\n";
			exit( 1 );
		}
		*/

		const char* objdump_args[5];
		objdump_args[0] = "objdump";
		objdump_args[1]	= "-d";
		objdump_args[2]	= "-l";
		objdump_args[3]	= argv[argNum];
		objdump_args[4] = NULL; 

		int k = execvp( objdump_args[0], (char* const*)objdump_args );

		if ( k == -1 ) {
			cerr << "\n\t\e[1mExecution for object dump failed\e[0m\n\n";
			exit( 1 );
		}
	}

	if ( close(readMe[1]) ) {								// parent does not write
		cerr << "\n\t\e[1mCould not close file descriptor for objdump pipe\e[0m\n\n";
		return 1;
	}
	
	char str[65536];
	memset( str, 0, 65536 );
	string file_contents;
	
	int readpipe = 0;
	
	do {
		if ( readpipe == -1 ) {
			cerr << "\n\t\e[1mCould not read from object dump pipe\e[0m\n\n";
      return 1;
		}
	 
		usleep( 100000 );							// using a hack wait method
		
		if ( strlen(str) ) {
			file_contents += str;
			memset( str, 0, sizeof(str) );
		}
	} while ( (readpipe = read( readMe[0], str, sizeof(str) )) );
	
	int status;
	int k = waitpid( objdump, &status, 0 );			
	
	if ( k == -1 ) {
    cerr << "\n\t\e[1mChild process for object dump failed\e[0m\n\n";
    return 1;
  }
  
  if ( WIFEXITED(status) && WEXITSTATUS(status) == 1 ) 
    return 1;

	if ( close(readMe[0]) ) {
  	cerr << "\n\t\e[1mCould not close file descriptor for objdump pipe\e[0m\n\n";
		return 1;
  }	

	//				*** PARSING PART ***		
	
/*	
	// get the object dump file
	if ( getObjDump(file_name) ) 
		return 1;					// only return if we failed
*/

	if ( file_contents.empty() ) {
		cerr << "\n\t\e[1mObject dump file is empty\e[0m\n\n";
		return 1;
	}
	
	// puts the object dump file in vector format
	getObjDump( file_contents );

	// now the vector objectDump has all the lines
	int len = objectDump.size();
	
	int incre = 0;
	
	while ( incre < len && objectDump[incre++].find("<main>:") == -1 );
	
	if ( incre == len ) {
		cerr << "\n\t\e[1mMain was not found in the object dump\e[0m\n\n";
		return 1;
	}
	
	incre++;
	int idx = 0;
	vector< int > indices;
	vector< int > linenumber;
	string fileloc;
		
	while ( !objectDump[incre].empty() ) {
		if ( objectDump[incre][0] == ' ' ) {
			mainFunct.push_back( objectDump[incre] );
			++idx;
		}
		else {
			// this line corresponds to the next block of x86 instructions				
			string line = objectDump[incre];
			int linelen = line.length();
			int numstart = 0;
			
			for ( int i = linelen - 1; i; --i ) {
				if ( line[i] == ':' ) {
					numstart = i;
					break;
				}
			}

			int linenum = stoi( line.substr(numstart + 1) );
			linenumber.push_back( linenum );
			indices.push_back( idx );
			
			if ( !idx )
				fileloc = line.substr( 0, numstart );
		}
		
		++incre;
	}
	
	if ( fileloc.empty() ) {
		cerr << "\n\t\e[1mMake sure you compiled your program with the -g flag\e[0m\n\n";
		return 1;
	}
	
	// gets the file name
	getFileName( fileloc );
	
	// gets the lines of the original program
	if ( getFileLines(fileloc) )
		return 1;
	
	// parse the assembly x86 file
	parseAssem( indices, linenumber );
	
	// print parsed data
	//printData();
	
	return 0;
}
