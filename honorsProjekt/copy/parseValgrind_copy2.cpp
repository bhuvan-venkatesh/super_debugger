#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

/*
 *		Must have object file and normal file in the same location	
 *
 *		Should have a modern version of valgrind/linux installed
 *
 *		Option to run with -f or -force to force continue
 *		./parseValgrind -f ./yourProgram
 */ 

// Compile with: g++ -std=c++0x -w parseValgrind.cpp -o parseValgrind

struct errors {
  string errorMessage;
  vector< tuple<string, string, int> > description;
};

vector<string> memory;
vector<errors> totalErrors;
tuple<string, string, int> desLines;
vector<errors> dividedErrors[9];

string listOfErrors[] = {"Segfault", "Invalid Read", "Invalid Write", "Invalid Free", "Mismatched Free ~ Always use malloc with free and new with delete", "Source and Destination Overwrite Each Other", "Conditional Jump", "A System Call Parameter is Uninitialised", "Your Function is Uninitialized, Value Might Be Too Big or Negative"}; 

/*
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

void printMem() {
  for (int i = 0; i < memory.size(); ++i)
    cout << memory[i] << "\n";
}

void printDes(tuple<string, string, int> t) { 
  cout << "    Function: " << get<0>(t) << "    File: " << get<1>(t) << "    Line#: " << get<2>(t) << "\n"; 
}

void printErrors(errors t) {
  cout << t.errorMessage << "\n";
  for (auto it = (t.description).begin(); it != (t.description).end(); ++it)
    printDes(*it);
  cout << "\n";
}

void printCategory() {
  for (int i = 0; i < 9; ++i) {
    if (dividedErrors[i].size()) {
      cout << "--------  " << listOfErrors[i] << "  -------- \n";
      for (int j = 0; j < dividedErrors[i].size(); ++j)
        printErrors(dividedErrors[i][j]);

      cout << "\n";
    }
  }
}

char **strsplit(char *str, const char *delim, size_t *numtokens) {
  size_t tokens_alloc = 1;
  size_t tokens_used = 0;
  char **tokens = (char**)calloc(tokens_alloc, sizeof(char*));
  char *token, *strtok_ctx;

  for (token = strtok_r(str, delim, &strtok_ctx); token != NULL; token = strtok_r(NULL, delim, &strtok_ctx)) {
    // check if we need to allocate more space for tokens
    if (tokens_used == tokens_alloc) {
      tokens_alloc *= 2;
      tokens = (char**)realloc(tokens, tokens_alloc * sizeof(char *));
    }

    tokens[tokens_used++] = strdup(token);
  }

  // cleanup
  if (tokens_used == 0) {
    free(tokens);
    tokens = NULL;
  } 
  else {
    tokens = (char**)realloc(tokens, tokens_used * sizeof(char *));
  }

  *numtokens = tokens_used;
  tokens = (char**)realloc(tokens, sizeof(char *) * (tokens_used + 1));
  tokens[tokens_used] = NULL;
  return tokens;
}

void parse(char* str) {
  vector<string> errorLines;
  string empty = "";
  string funct;
  string file;
  int lineNum;
  errorLines.push_back(empty);
  int end = 0;
  int place;

  size_t len;
  char** parseMe = strsplit(str, "\n", &len);

  string line;
  for (int i = 5; i < len; ++i) {
    line = parseMe[i];
    line.erase(0, line.find(" ") + 1);

    if (!line.find("HEAP SUMMARY")) 
      place = end;

    errorLines.push_back(line);
    ++end;
  }

  for (int i = 0; i < len; ++i)
    free(parseMe[i]);
  free(parseMe);

  int i = 0;
  while (i != place) {
    if (errorLines[i].empty()) {
      errors temp;
      while (errorLines[i].empty())
        ++i;

      if (i >= place)
        break;

      temp.errorMessage = errorLines[i];
      ++i;

      while (!errorLines[i].empty()) {
        int k = errorLines[i].find(":");
        if (k != -1) {
          errorLines[i].erase(0, k + 2);
          if (errorLines[i][0] != '_') {
            int w = errorLines[i].find(" ");
            funct = errorLines[i].substr(0, w);			
            errorLines[i].erase(0, w + 2);

            int c = errorLines[i].find(":");
            file = errorLines[i].substr(0, c);
            errorLines[i].erase(0, c + 1);
            if (errorLines[i][0] >= '0' && errorLines[i][0] <= '9') {
              int lineNum = stoi(errorLines[i].substr(0, errorLines[i].length() - 1));
              desLines = make_tuple(funct, file, lineNum);
              (temp.description).push_back(desLines);
            }
            else {
              desLines = make_tuple(funct, "C Lib Call", -1);
              (temp.description).push_back(desLines);
            }
          }
        }

        ++i;
      }

      totalErrors.push_back(temp);
    }
  }

  for (; i < errorLines.size(); ++i) {
    if (errorLines[i].find("LEAK SUMMARY") != -1) {
      while (i < errorLines.size()) {
        if (errorLines[i].find("Rerun with") != -1) 
          i = errorLines.size();
        else memory.push_back(errorLines[i]);

        ++i;
      }
    }
    else {
      memory.push_back(errorLines[i]);

      if (errorLines[i].find("All heap blocks") != -1) 
        i = errorLines.size();
    }
  }	

  // print stuff
/*	
  for (int i = 0; i < totalErrors.size(); ++i) 
  printErrors(totalErrors[i]);	

  printMem();
*/	 
}

void examine() {
  for (int i = 0; i < totalErrors.size(); ++i) {
    if (totalErrors[i].errorMessage.find("(SIGSEGV)") != -1)
      dividedErrors[0].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Invalid read"))
      dividedErrors[1].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Invalid write"))
      dividedErrors[2].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Invalid free"))
      dividedErrors[3].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Mismatched free"))
      dividedErrors[4].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Source and destination"))
      dividedErrors[5].push_back(totalErrors[i]);
    else if (totalErrors[i].errorMessage.find("has a fishy") != -1)
      dividedErrors[8].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Conditional jump"))
      dividedErrors[6].push_back(totalErrors[i]);
    else if (!totalErrors[i].errorMessage.find("Syscall param"))
      dividedErrors[7].push_back(totalErrors[i]);
  }

  // add print stuff here
  //printCategory();
}

void handleNows() {
  if (dividedErrors[5].size() + dividedErrors[7].size() + dividedErrors[8].size()) {
    cout << "\n----------------------------------------------------------------------------------\n----------------------------------------------------------------------------------\n\n";

    cout << "\t\e[1m***\tFix these errors before continuing   ***\n\t***\tIf you want to force continue your program:   ***\n\t***\trerun with the -f or -force flag eg ./debug -f ./yourProgram   ***\e[0m\n\n";

    cout << "----------------------------------------------------------------------------------\n----------------------------------------------------------------------------------\n\n";

    if (dividedErrors[5].size()) {
      for (int i = 0; i < dividedErrors[5].size(); ++i) {
        string err = dividedErrors[5][i].errorMessage;
        string s = err.substr(34, err.find("(") - 34);
        cout << "The source and destination of the function: \e[1m\e[4m" << s << "\e[0m\e[0m overlap\n"; 
        vector< tuple<string, string, int> > errDescript = dividedErrors[5][i].description;

        cout << "\tYour error is in the function: \e[1m\e[4m" << s << "\e[0m\e[0m\n";
        for (int j = 0; j < errDescript.size(); ++j) {
          if (get<2>(errDescript[j]) != -1) {
            cout << "\tThis is on line: \e[1m\e[4m" << get<2>(errDescript[j]) << "\e[0m\e[0m, of the file: \e[1m\e[4m" << get<1>(errDescript[j]) << "\e[0m\e[0m, in: \e[1m\e[4m" << get<0>(errDescript[j]) << "\e[0m\e[0m\n\n\n";
            break;
          }
        }
      }
    }

    if (dividedErrors[7].size()) {
      for (int i = 0; i < dividedErrors[7].size(); ++i) {
        string err = dividedErrors[7][i].errorMessage;
        string s = err.substr(14, err.find("points to uni") - 15);
        cout << "Parameter(s) to the system call of the function: \e[1m\e[4m" << s << "\e[0m\e[0m are not initialized\n"; 
        vector< tuple<string, string, int> > errDescript = dividedErrors[7][i].description;

        int count = 0;				
        cout << "\tYour error is in the function: \e[1m\e[4m" << s << "\e[0m\e[0m\n";	
        for (int j = 0; j < errDescript.size(); ++j) {
          if (count == 1 && get<2>(errDescript[j]) != -1) {
            cout << "\n\tYou declared your parameter on line: \e[1m\e[4m" << get<2>(errDescript[j]) << "\e[0m\e[0m, of the file: \e[1m\e[4m" << get<1>(errDescript[j]) << "\e[0m\e[0m, in: \e[1m\e[4m" << get<0>(errDescript[j]) << "\e[0m\e[0m"; 
            break;
          }

          if (get<2>(errDescript[j]) != -1) {
            cout << "\tThis is on line: \e[1m\e[4m" << get<2>(errDescript[j]) << "\e[0m\e[0m, of the file: \e[1m\e[4m" << get<1>(errDescript[j]) << "\e[0m\e[0m, in: \e[1m\e[4m" << get<0>(errDescript[j]) << "\e[0m\e[0m";
            ++count;
          }
        }

        cout << "\n\n\n";
      }
    }

    if (dividedErrors[8].size()) {
      for (int i = 0; i < dividedErrors[8].size(); ++i) {
        string err = dividedErrors[8][i].errorMessage;
        string s = err.substr(28, err.find("has a fishy") - 29);
        string val = err.substr(err.find(":") + 2);
        cout << "The value you passed into the function: \e[1m\e[4m" << s << "\e[0m\e[0m is too big or negative\nThe value \e[1m\e[4m" << s << "\e[0m\e[0m recieved is " << val << "\n"; 
        vector< tuple<string, string, int> > errDescript = dividedErrors[8][i].description;

        cout << "\tYour error is in the function: \e[1m\e[4m" << s << "\e[0m\e[0m\n";
        for (int j = 0; j < errDescript.size(); ++j) {
          if (get<2>(errDescript[j]) != -1) {
            cout << "\tThis is on line: \e[1m\e[4m" << get<2>(errDescript[j]) << "\e[0m\e[0m, of the file: \e[1m\e[4m" << get<1>(errDescript[j]) << "\e[0m\e[0m, in: \e[1m\e[4m" << get<0>(errDescript[j]) << "\e[0m\e[0m\n\n";
            break;
          }
        }
      }
    }
  }
}

int main(int argc, char* argv[]) {
  int readMe[2];

  if (pipe(readMe) == -1) {
    cout << "Creational of pipe for valgrind failed\n";
    exit(420);	
  }

  pid_t parent = fork();

  if (parent == -1) {
    cout << "Forking failed for valgrind process\n";
    exit(420);
  }

  if (parent) {
    // no need to wait anymore -- piping output ???
    close(readMe[1]);				// parent does not write

    int status;
    int k = waitpid(parent, &status, 0);

    if (k == -1) {
      cout << "Child process for valgrind failed\n";
      exit(666);	
    }

    char str[65536];

    int bytesread = read(readMe[0], str, sizeof(str));	

    if (bytesread == -1)
      exit(666);

    close(readMe[0]);	

    // now parse the text file
    parse(str);

    // now do stuff  ---  vector should be filled by now

    // break up errors (if any) to different categories
    examine();

    int flag = 1;
    if ((!strcmp(argv[1], "-f")) || (!strcmp(argv[1], "-force")))
      flag = 0;

    // check if any handle-nows are happening --- these should also be top priority 
    // these errors might lead to further errors later on
    if (flag) {
      handleNows();
      return 0;
    }		

    // now run the program
/*
    int gdbFD[2];	

    if (pipe(gdbFD) == -1) {
      cout << "Creational of pipe for gdb failed\n";
      exit(666);
    }

    pid_t gdbParent = fork();

    if (gdbParent == -1) {
      cout << "Forking failed for gdb process\n";
      exit(666);
    }

    if (gdbParent) {
      close(gdbFD[1]);


    }
    else {
      close(gdbFD[0]);
      dup2(gdbFD[1], 0);			// redirect stdin

      char* gdb_args[argc + 2];
      gdb_args[0] = "gdb";
      gdb_args[1] = "--args";

      for (int i = 2; i < argc + 1; ++i)
        gdb_args[i] = argv[i - 1];
      gdb_args[argc + 1] = NULL;

      int k = execvp(gdb_args[0], gdb_args);

      if (k == -1)
        exit(241);
    }
*/    
  }
  else {
    close(readMe[0]);				// child does not read
    dup2(readMe[1], 2);				// redirect stderr	

    int m = 1;
    if ((!strcmp(argv[1], "-f")) || (!strcmp(argv[1], "-force")))
      ++m;

    char* valgrind_args[argc + 1];
    valgrind_args[0] = "valgrind";

    int j = 1;						// mad skillz handlez
    int p = m - j;
    for (int i = m; i < argc + p; ++i, ++j)
      valgrind_args[j] = argv[i];
    valgrind_args[argc] = NULL;

    int k = execvp(valgrind_args[0], valgrind_args);

    if (k == -1)
      exit(1337);
  }

  return 0;
}
