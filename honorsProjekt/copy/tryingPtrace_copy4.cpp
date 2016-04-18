#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include <sys/stat.h>
#include <fcntl.h>

#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/reg.h>

using namespace std;

// struct for registers
struct user_regs_struct regs;

// struct to access stuff
struct user* user_space;

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
	
	/*
	while ( i < len ) {
		long k = get_data( pid, addr + i );
		i += sizeof( long );				// 8
		
		if ( i <= len ) {
			while ( k ) {
				cout << ( char )k;
				k >>= sizeof( long );
			}
		}
		else {
			int remain = len % sizeof( long );
			
			while( remain-- ) {
				cout << ( char )k;
				k >>= sizeof( long );
			}
		}
	}
	*/
	
	cout << "\n";						// extra space here
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
	
	set_data( pid, addr + len - 1, ( void* )data );				// using hackz
}


// create breakpoint
pair< long, void* > set_brkpt( pid_t pid, void* addr ) {							
	// save intruction pointer
	//void* temp = get_instr_ptr( pid );
	void* temp = addr;
	//cout << temp << "\n";
	
	// inject INT 3 = 0xCC --- this is an interrupt, sends signal to debugger
	long new_data = get_data( pid, temp );
	long former_data = new_data;
	new_data <<= (new_data >> 8);
	new_data |= 0xCC;
	set_data( pid, temp, ( void* )new_data );
	
	// need to store the former instruction pointer so we return it
	pair< long, void* > p;
	p = make_pair( former_data, temp );
	return p;
}

// set back instruction pointer
void handle_brkpt( pid_t pid, void* instr_ptr ) {
	set_instr_ptr( pid, instr_ptr );
}


int main( int argc, char* argv[] ) {
	pid_t parent = fork();
	
	if ( parent == -1 ) {
		cerr << "Forking for main debugger failed\n";			
		exit( 420 );
	}
	
	if ( parent ) {
		// will send SIGTRAP at first execvp
		int	status;
		int insyscall = 0;
		long orig_rax, rax, ins;
		long param[3];
		int brkpt = 0;
		pair< long, void* > brkpt_info;
		
		// vector to store all info about breakpoints
		vector< pair< long, void* > > all_brkpts;
		
		// for our vector
		int brkpt_pos = 0;
		
		// need array of breakpoint addresses
		void* addr = ( void* )0x000000000040070d;				// fix to be main
		void* sec_addr = ( void* )0x0000000000400775;			// before malloc
		
		waitpid( parent, &status, 0 );
		orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );
		
		if ( WIFSTOPPED( status ) ) {
			if ( WSTOPSIG( status ) == SIGTRAP ) {				// before execve
				cout << "Child stopped on SIGTRAP - continuing...\n";
				cout << "The child made a system call " << orig_rax << "\n";
				
				// first breakpoint
				brkpt_info = set_brkpt( parent, addr );
				all_brkpts.push_back( brkpt_info );
				++brkpt;
				
				// second breakpoint
				brkpt_info = set_brkpt( parent, sec_addr );
				all_brkpts.push_back( brkpt_info );
				++brkpt;
				
				ptrace( PTRACE_CONT, parent, NULL, NULL );
			}
		}
		
		do {
			waitpid( parent, &status, 0 );
			orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );

			//cout << orig_rax << "\n";

			if ( WIFSTOPPED( status ) ) {
				usleep( 10000 );
				cout << get_instr_ptr( parent ) << "\n";
				
				if ( brkpt ) {
					cout << "In breakpoint: " << get_instr_ptr( parent ) << "\n";
					
					// get the breakpoint info from vector
					brkpt_info = all_brkpts[brkpt_pos++];
					
					// restore data to get rid of INT 3
					set_data( parent, brkpt_info.second, ( void* )brkpt_info.first );
					
					// do not need currently since the instruction pointer goes back
					handle_brkpt( parent, brkpt_info.second );	
					
					cout << "In breakpoint after reset data: " << get_instr_ptr( parent ) << "\n";
					
					--brkpt;
				}
				
				ptrace( PTRACE_CONT, parent, NULL, NULL );
		  }  
		}
		while ( !WIFEXITED( status ) );
		
		/*
		do {
			waitpid( parent, &status, 0 );
			orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );

			//cout << orig_rax << "\n";

			if ( WIFSTOPPED( status ) ) {
				//cout << get_instr_ptr( parent ) << "\n";
			
				//usleep(10000);
			
				if ( brkpt && !insyscall ) {
					cout << "In breakpoint: " << get_instr_ptr( parent ) << "\n";
					
					// restore data to get rid of INT 3
					set_data( parent, brkpt_info.second, ( void* )brkpt_info.first );
					
					// do not need currently since the instruction pointer goes back
					handle_brkpt( parent, brkpt_info.second );						
					brkpt = 0;
				}
			
		    if ( orig_rax == SYS_write ) {	    
		    	if ( !insyscall ) {
		    		insyscall = 1;
		    		
		    		brkpt_info = set_brkpt( parent, NULL );
		    		brkpt = 1;
		    		cout << "Former instruction pointer: " << get_instr_ptr( parent ) << "\n";
		    		//handle_brkpt( parent, former_addr );
		    		
		    		//ptrace( PTRACE_CONT, parent, NULL, NULL );
		    	}
		    	else {
		    		insyscall = 0;
		    		//cout << "escaped\n";
		    	}
		  	}
		  	
		  	ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
		  }  
		}
		while ( !WIFEXITED( status ) );
		*/
		/*
		do {
			waitpid( parent, &status, 0 );
			orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL ); 		// same as below
			//orig_rax = ptrace( PTRACE_PEEKUSER, parent, &user_space -> regs.orig_rax, NULL ); 	

			if ( insyscall ) {
				void* addr = get_instr_ptr( parent );
    		ins = get_data( parent, addr );
    		cout << "Instruction pointer: " << addr << " executed: " << ins << "\n";
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
		/*
		do {
			waitpid( parent, &status, 0 );
			ptrace( PTRACE_GETREGS, parent, NULL, &regs );

			if ( insyscall ) {
    		ins = ptrace( PTRACE_PEEKTEXT, parent, regs.rip, NULL );
    		//ins = get_data( parent, ( void* )regs.rip );			// same as above
    		cout << "Instruction pointer: " << regs.rip << " executed: " << ins << "\n";
			}

    	if ( regs.orig_rax == SYS_write ) {	    
    		insyscall = 1;
    		ptrace( PTRACE_SINGLESTEP, parent, NULL, NULL );
    	}
    	else {
    		ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
    	}	  	
		}
		while ( !WIFEXITED( status ) );
		*/
		/*
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
		    		param[2] = ptrace( PTRACE_PEEKUSER, parent, 8 * RDX, NULL );	// same as below
		    		
		    		//param[0] = ptrace( PTRACE_PEEKUSER, parent, &user_space -> regs.rdi, NULL );
		    		//param[1] = ptrace( PTRACE_PEEKUSER, parent, &user_space -> regs.rsi, NULL );
		    		//param[2] = ptrace( PTRACE_PEEKUSER, parent, &user_space -> regs.rdx, NULL );
		    		
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
		*/
		/*
		do {
			waitpid( parent, &status, 0 );
			ptrace( PTRACE_GETREGS, parent, NULL, &regs );

			if ( WIFSTOPPED( status ) ) {
		    if ( regs.orig_rax == SYS_write ) {	    
		    	if ( !insyscall ) {
		    		insyscall = 1;
		    		//get_word( parent, ( void* )regs.rsi, regs.rdx );
		    		//reverse_word( parent, ( void* )regs.rsi, regs.rdx );
		    		
		    		//cout << "Write called with " << regs.rdi << " " << regs.rsi << " " << regs.rdx << "\n";
		    	}
		    	else {
		    		insyscall = 0;
		    	}
		  	}
		  	
		  	ptrace( PTRACE_SYSCALL, parent, NULL, NULL );
		  }  
		}
		while ( !WIFEXITED( status ) );
		*/
		
		cout << "Child exited - debugger terminating...\n";
	}
	else {
		if ( ptrace( PTRACE_TRACEME, 0, NULL, NULL ) ) {
			cerr << "Cannot trace child process for main debugger\n";
			exit( 1337 );
		}
		
		int k = execvp( argv[1], &argv[1] );
		
		if ( k == -1 )
			cerr << "Execution failed for main debugger\n";
      exit( 1337 );
	}	
	
	return 0;
}
