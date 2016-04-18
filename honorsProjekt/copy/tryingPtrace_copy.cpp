#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

// testing to get the word from write
void get_word( pid_t pid, void* addr, int len ) {
	int i = 0;
	
	while ( i < len ) {
		cout << ( char )get_data( pid, addr +  i /* chars are 1 byte */ );
		++i;
	}
	
	cout << "\n";						// extra space here
}

// create breakpoint
void breakpoint ( pid_t pid ) {
	// save intruction pointer
	void* temp = get_instr_ptr( pid );
	
	// inject INT 3 = 0xCC
	
	
	// set back instruction pointer
	set_instr_ptr( pid, temp );
}


int main( int argc, char* argv[] ) {
	pid_t parent = fork();
	
	if ( parent == -1 ) {
		cout << "Forking for main debugger failed\n";				// can use cerr???
		exit( 420 );
	}
	
	if ( parent ) {
		// will send SIGTRAP at first execvp
		int	status;
		int insyscall = 0;
		long orig_rax, rax;
		long param[3];
		
		do {
			waitpid( parent, &status, 0 );
			orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );
/*		  
		  if ( WIFSTOPPED( status ) && WSTOPSIG( status ) == SIGTRAP ) {
		    //cout << "Child stopped on SIGTRAP - continuing...\n";
		    cout << "The child made a system call " << orig_rax << "\n";			// can determine system call numbers here
		    
		    ptrace( PTRACE_CONT, parent, NULL, NULL );
	    }
*/	 
 

			if ( WIFSTOPPED( status ) ) {
			/*
				if ( WSTOPSIG( status ) == SIGTRAP ) {
					cout << "Child stopped on SIGTRAP - continuing...\n";
					cout << "The child made a system call " << orig_rax << "\n";
				}
			*/	
		    if ( orig_rax == SYS_write ) {
		    	if ( !insyscall ) {
		    		insyscall = 1;
		    		
		    		param[0] = ptrace( PTRACE_PEEKUSER, parent, 8 * RDI, NULL );
		    		param[1] = ptrace( PTRACE_PEEKUSER, parent, 8 * RSI, NULL );
		    		param[2] = ptrace( PTRACE_PEEKUSER, parent, 8 * RDX, NULL );
		    		
		    		get_word( parent, ( void* )param[1], param[2] );
		    		
		    		cout << "Write called with " << param[0] << " " << param[1] << " " << param[2] << "\n";
		    	}
		    	else {
		    		insyscall = 0;
		    		rax = ptrace( PTRACE_PEEKUSER, parent, 8 * RAX, NULL );
		    		cout << "Write returned with " << rax << "\n";
		    	}
		    }
		    
		  	ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
	  		//ptrace( PTRACE_CONT, parent, NULL, NULL );
		  }

/*
			if ( WIFSTOPPED( status ) ) {
				if ( orig_rax == SYS_write ) {
					if ( orig_rax == SYS_write ) {
				  	if ( !insyscall ) {
				  		insyscall = 1;
				  		
				  		ptrace( PTRACE_GETREGS, parent, NULL, &regs );
				  		
				  		cout << "Write called with " << regs.rdi << " " << regs.rsi << " " << regs.rdx << "\n";
				  	}
				  	else {
				  		insyscall = 0;
				  		rax = ptrace( PTRACE_PEEKUSER, parent, 8 * RAX, NULL );
				  		cout << "Write returned with " << rax << "\n";
				  	}
			  	}		  
	  		}
	  		
	  		ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
	  		//ptrace( PTRACE_CONT, parent, NULL, NULL );
		  }   
*/		  
		}
		while ( !WIFEXITED( status ) );
		
		cout << "Child exited - debugger terminating...\n";
	}
	else {
		if ( ptrace( PTRACE_TRACEME, 0, NULL, NULL ) ) {
			cout << "Cannot trace child process for main debugger\n";
			exit( 1337 );
		}
		
		//cout << getpid() << "\n";
		
		int k = execvp( argv[1], &argv[1] );
		//int k = execl("/bin/ls", "ls", NULL);
		
		if ( k == -1 )
			cout << "Execution failed for main debugger\n";
      exit( 1337 );
	}
	
	return 0;
}

/*

void reverse_word( pid_t pid, void* addr, int len ) {
	char rev[len];
	char norm[len];

	//cout << "*** letter by letter\n";
	
	//for ( int i = 0; i < len - 1; ++i ) { 
	//	rev[len - i - 2] = ( char )get_data( pid, addr +  i );
		//cout << rev[len - i - 2];
	//}
	
	
	for ( int i = 0; i < len - 1; ++i ) {
		norm[i] = ( char )get_data( pid, addr +  i );
		//cout << norm[i];
	}
	
	long data = get_data( pid, addr + len - 1 );
	
	norm[len - 1] = 0;

	//rev[len - 1] = '\0';
	
	//cout << "--- reversed word is: " << rev << "\n";	
	
	
	for ( int i = 0; i < len - 1; ++i ) {
		//set_data( pid, addr + i, ( void* )rev[i] );
		set_data( pid, addr + i, ( void* )norm[i] );
		//ptrace( PTRACE_POKEDATA, pid, addr + i, (void*)norm[i] );
	}
	
	set_data( pid, addr + len - 1, (void*)data );				// using hackz
	//set_data( pid, addr, (void*)*norm );
}



