#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/reg.h>

using namespace std;

struct user_regs_struct regs;

// Can have option to compile their file manually so they do not need to know compiler flags 

// helpers to get stuff
void* get_instr_ptr( pid_t pid ) {
	return ( void* )ptrace( PTRACE_PEEKUSER, pid, 8 * RIP, NULL );
}

void set_instr_ptr( pid_t pid, void* addr ) {
	ptrace( PTRACE_POKEUSER, pid, 8 * RIP, addr );
}

void* get_stack_ptr( pid_t pid ) {
	return ( void* )ptrace( PTRACE_PEEKUSER, pid, 8 * RSP, NULL );
}

void* get_ret_addr( pid_t pid, void* stack_pointer ) {
	return ( void* )ptrace( PTRACE_PEEKTEXT, pid, stack_pointer, NULL );
}

long get_data( pid_t pid, void* addr ) {
	return ptrace( PTRACE_PEEKDATA, pid, addr, NULL );
}

void set_data( pid_t pid, void* addr, void* data ) {
	ptrace( PTRACE_POKEDATA, pid, addr, data );
}

void step( pid_t pid ) {
	ptrace( PTRACE_SINGLESTEP, pid, NULL, NULL );
}

long get_instr( pid_t pid ) {
	return ptrace( PTRACE_PEEKDATA, pid, 8 * RIP, NULL );
}


// testing to get the word from write
void get_word( pid_t pid, void* addr, int len ) {
	int i = 0;
	
	cout << "--- ptraced word is: ";
	while ( i < len ) {
		cout << ( char )get_data( pid, addr +  i /* chars are 1 byte */ );
		++i;
	}
	
	//cout << "\n";						// extra space here
}

void reverse_word( pid_t pid, void* addr, int len ) {
	char rev[len];

	for ( int i = 0; i < len - 1; ++i ) 
		rev[len - i - 2] = ( char )get_data( pid, addr +  i );
	
	long data = get_data( pid, addr + len - 1 );

	rev[len - 1] = '\0';
	
	//cout << "--- reversed word is: " << rev << "\n";	
	
	for ( int i = 0; i < len - 1; ++i ) 
		set_data( pid, addr + i, ( void* )rev[i + 1] );
	
	set_data( pid, addr + len - 1, (void*)data );				// using hackz
}


// create breakpoint
void breakpoint( pid_t pid ) {
	// save intruction pointer
	void* temp = get_instr_ptr( pid );
	
	// inject INT 3 = 0xCC
	
	
	// set back instruction pointer
	set_instr_ptr( pid, temp );
}


int main( int argc, char* argv[] ) {
	string temp = argv[1];
	temp = temp.substr( 2 ); 
	temp += "_copy.dump";		
	const char* str = temp.c_str();

	pid_t objdump = fork();

	if ( !objdump ) {	
		int fd = open( str, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR );
	 	dup2(fd, 1);   // make stdout go to file
    close(fd);     // fd no longer needed - the dup'ed handles are sufficient
    
		char* objdump_args[5];
	 
		objdump_args[0] = "objdump";
		objdump_args[1] = "-S";
		objdump_args[2]	= "-d";
		objdump_args[3]	= argv[1];
		objdump_args[4] = NULL; 
		
		int k = execvp( objdump_args[0], objdump_args );
		
		if ( k == -1 ) {
			cout << "Execution for object dump failed\n";
			exit( 420 );
		}
	}
	
	int stat;
	waitpid( objdump, &stat, 0 );
	
	// open file
	
	
	pid_t parent = fork();
	
	if ( parent == -1 ) {
		cout << "Forking for main debugger failed\n";				// can use cerr???
		exit( 420 );
	}
	
	if ( parent ) {
		// will send SIGTRAP at first execvp
		int	status;
		int insyscall = 0;
		long orig_rax, rax, ins;
		long param[3];
		
		do {
			waitpid( parent, &status, 0 );
			orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );

			//cout << orig_rax << "\n";

			if ( WIFSTOPPED( status ) ) {
		    if ( orig_rax == SYS_write ) {	    
		    	if ( !insyscall ) {
		    		insyscall = 1;
		    		
		    		param[0] = ptrace( PTRACE_PEEKUSER, parent, 8 * RDI, NULL );
		    		param[1] = ptrace( PTRACE_PEEKUSER, parent, 8 * RSI, NULL );
		    		param[2] = ptrace( PTRACE_PEEKUSER, parent, 8 * RDX, NULL );
		    		
		    		get_word( parent, ( void* )param[1], param[2] );
		    		reverse_word( parent, ( void* )param[1], param[2] );
		    		
		    		//cout << "Write called with " << param[0] << " " << param[1] << " " << param[2] << "\n";
		    	}
		    	else {
		    		insyscall = 0;
		    		//rax = ptrace( PTRACE_PEEKUSER, parent, 8 * RAX, NULL );
		    		//cout << "Write returned with " << rax << "\n";
		    	}
		  	}
		  	
		  	ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
		  }  
		}
		while ( !WIFEXITED( status ) );
		
		/*
		do {
			waitpid( parent, &status, 0 );
			orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );

			if ( insyscall ) {
				ins = get_instr( parent );
				
				cout << RIP << " " << ins << "\n";
			}
	
	    if ( orig_rax == SYS_write ) {
	    	insyscall = 1;
	   		step( parent );
	  	}
	  	else {
	  		ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
	  	} 
		}
		while ( !WIFEXITED( status ) );
		*/
		
		cout << "Child exited - debugger terminating...\n";
	}
	else {
		if ( ptrace( PTRACE_TRACEME, 0, NULL, NULL ) ) {
			cout << "Cannot trace child process for main debugger\n";
			exit( 1337 );
		}
		
		int k = execvp( argv[1], &argv[1] );
		
		if ( k == -1 )
			cout << "Execution failed for main debugger\n";
      exit( 1337 );
	}	
	
	return 0;
}
