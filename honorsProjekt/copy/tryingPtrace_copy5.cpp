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

struct brkpt_info {
  long former_data;
  void* orig_addr;
  void* brkpt_addr;				// 1 more than original
};

/*debugger commands*/
typedef enum {CONTINUE, BREAK, KILL, NEXT, REGISTER, PRINT} cmd;



// vector to store all info about breakpoints
vector< brkpt_info > all_brkpts;

int brkpt_idx;

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

void getRegisters( pid_t pid ) {
	ptrace( PTRACE_GETREGS, pid, NULL, &regs );
}

// prints 27 registers
void printRegisters( pid_t pid ) {
	cout << "\n";
	cout << "Register rax:\t\t" << get_data( pid, ( void* )regs.rax ) << "\n";
	cout << "Register rbx:\t\t" << get_data( pid, ( void* )regs.rbx ) << "\n";
	cout << "Register rcx:\t\t" << get_data( pid, ( void* )regs.rcx ) << "\n";
	cout << "Register rdx:\t\t" << get_data( pid, ( void* )regs.rdx ) << "\n";
	cout << "Register rsi:\t\t" << get_data( pid, ( void* )regs.rsi ) << "\n";
	cout << "Register rdi:\t\t" << get_data( pid, ( void* )regs.rdi ) << "\n";
	cout << "Register rbp:\t\t" << get_data( pid, ( void* )regs.rbp ) << "\n";
	cout << "Register rsp:\t\t" << get_data( pid, ( void* )regs.rsp ) << "\n";
	cout << "Register r8:\t\t" << get_data( pid, ( void* )regs.r8 ) << "\n";
	cout << "Register r9:\t\t" << get_data( pid, ( void* )regs.r9 ) << "\n";
	cout << "Register r10:\t\t" << get_data( pid, ( void* )regs.r10 ) << "\n";
	cout << "Register r11:\t\t" << get_data( pid, ( void* )regs.r11 ) << "\n";
	cout << "Register r12:\t\t" << get_data( pid, ( void* )regs.r12 ) << "\n";
	cout << "Register r13:\t\t" << get_data( pid, ( void* )regs.r13 ) << "\n";
	cout << "Register r14:\t\t" << get_data( pid, ( void* )regs.r14 ) << "\n";
	cout << "Register r15:\t\t" << get_data( pid, ( void* )regs.r15 ) << "\n";
	cout << "Register rip:\t\t" << get_data( pid, ( void* )regs.rip ) << "\n";
	cout << "Register eflags:\t" << get_data( pid, ( void* )regs.eflags ) << "\n";
	cout << "Register cs:\t\t" << get_data( pid, ( void* )regs.cs ) << "\n";
	cout << "Register ss:\t\t" << get_data( pid, ( void* )regs.ss ) << "\n";
	cout << "Register ds:\t\t" << get_data( pid, ( void* )regs.ds ) << "\n";
	cout << "Register es:\t\t" << get_data( pid, ( void* )regs.es ) << "\n";
	cout << "Register fs:\t\t" << get_data( pid, ( void* )regs.fs ) << "\n";
	cout << "Register gs:\t\t" << get_data( pid, ( void* )regs.gs ) << "\n\n";
	/*	
		cout << "\n";
		cout << "Register r15:\t\t" << get_data( pid, ( void* )regs.r15 ) << "\n";
		cout << "Register r14:\t\t" << get_data( pid, ( void* )regs.r14 ) << "\n";
		cout << "Register r13:\t\t" << get_data( pid, ( void* )regs.r13 ) << "\n";
		cout << "Register r12:\t\t" << get_data( pid, ( void* )regs.r12 ) << "\n";
		cout << "Register rbp:\t\t" << get_data( pid, ( void* )regs.rbp ) << "\n";
		cout << "Register rbx:\t\t" << get_data( pid, ( void* )regs.rbx ) << "\n";
		cout << "Register r11:\t\t" << get_data( pid, ( void* )regs.r11 ) << "\n";
		cout << "Register r10:\t\t" << get_data( pid, ( void* )regs.r10 ) << "\n";
		cout << "Register r9:\t\t" << get_data( pid, ( void* )regs.r9 ) << "\n";
		cout << "Register r8:\t\t" << get_data( pid, ( void* )regs.r8 ) << "\n";
		cout << "Register rax:\t\t" << get_data( pid, ( void* )regs.rax ) << "\n";
		cout << "Register rcx:\t\t" << get_data( pid, ( void* )regs.rcx ) << "\n";
		cout << "Register rdx:\t\t" << get_data( pid, ( void* )regs.rdx ) << "\n";
		cout << "Register rsi:\t\t" << get_data( pid, ( void* )regs.rsi ) << "\n";
		cout << "Register rdi:\t\t" << get_data( pid, ( void* )regs.rdi ) << "\n";
		cout << "Register orig_rax:\t" << get_data( pid, ( void* )regs.orig_rax ) << "\n";
		cout << "Register rip:\t\t" << get_data( pid, ( void* )regs.rip ) << "\n";
		cout << "Register cs:\t\t" << get_data( pid, ( void* )regs.cs ) << "\n";
		cout << "Register eflags:\t" << get_data( pid, ( void* )regs.eflags ) << "\n";
		cout << "Register rsp:\t\t" << get_data( pid, ( void* )regs.rsp ) << "\n";
		cout << "Register ss:\t\t" << get_data( pid, ( void* )regs.ss ) << "\n";
	//cout << "Register fs_base:\t" << get_data( pid, ( void* )regs.fs_base ) << "\n";
	//cout << "Register gs_base:\t" << get_data( pid, ( void* )regs.gs_base ) << "\n";
	cout << "Register ds:\t\t" << get_data( pid, ( void* )regs.ds ) << "\n";
	cout << "Register es:\t\t" << get_data( pid, ( void* )regs.es ) << "\n";
	cout << "Register fs:\t\t" << get_data( pid, ( void* )regs.fs ) << "\n";
	cout << "Register gs:\t\t" << get_data( pid, ( void* )regs.gs ) << "\n\n";
	*/	
}

// create breakpoint
void set_brkpt( pid_t pid, void* addr ) {							
	// inject INT 3 = 0xCC --- this is an interrupt, sends signal to debugger
	long new_data = get_data( pid, addr );
	long former_data = new_data;
	new_data = ( new_data & ~0xFF ) | 0xCC;
	set_data( pid, addr, (void*)new_data );

	// need to store the former instruction pointer and data
	brkpt_info storage;
	storage.former_data = former_data;
	storage.orig_addr = addr;
	storage.brkpt_addr = addr + 1;
	all_brkpts.push_back( storage );
}

// set back instruction pointer
void handle_brkpt( pid_t pid, void* instr_ptr, void* data ) {
	set_data( pid, instr_ptr, data );
	set_instr_ptr( pid, instr_ptr );
}

bool is_brkpt( pid_t pid ) {
	void* instr_addr = get_instr_ptr( pid );

	for ( int i = 0; i < all_brkpts.size(); ++i ) {
		if ( all_brkpts[i].brkpt_addr == instr_addr ) {
			brkpt_idx = i;
			return 1;
		}
	}

	return 0;
}

brkpt_info get_brkpt() {
	return all_brkpts[brkpt_idx];
}

void printHelp() {
	cout << "Welcome to sdb\n";
	cout << "Run your program with 'r' or 'run'\n";
	cout << "Set breakpoints with 'break', 'br', or 'b' along with the line number or function\n";
	cout << "Continue your program with 'c' or 'continue'\n";
	cout << "Step through your program line by line with 'n', 'next', 's', or 'step'\n";
	cout << "Stop debugging your program with 'kill', 'q', or 'quit'\n";
	cout << "For more advanced features type 'help' to get the complete functionality of sdb\n\n"; 
}


/* reads command from user */
cmd next_cmd(char **argv) {
	cmd cmd = REGISTER;

	char *cmdstr;
	//if (!(cmdstr = readline("super_debugger -> "))) return cmd;
	//cout << "

	if (!strcmp(cmdstr, "c") || !strcmp(cmdstr, "continue")){
		cmd = CONTINUE;
	}else if (!strcmp(cmdstr, "kill") || !strcmp(cmdstr, "quit") || !strcmp(cmdstr, "q")){   
		cmd = KILL;
	}else if (!strcmp(cmdstr, "n") || !strcmp(cmdstr, "next") || !strcmp(cmdstr, "step") || !strcmp(cmdstr, "s")){
		cmd = NEXT;
	}else if (!strcmp(cmdstr, "reg") || !strcmp(cmdstr, "registers")){
		cmd = REGISTER;
	}else{
		const char *op = strtok(cmdstr, " ");
		const char *arg = strtok(NULL, " ");
		if ( op && arg && (strcmp(op, "break") == 0 || !strcmp(op, "br") || !strcmp(op, "b")))
		{
			cmd = BREAK;
			*argv = strdup(arg);
		}else if ( op && arg && (!strcmp(op, "print") || !strcmp(op, "p")))
		{
			cmd = PRINT;
			*argv = strdup(arg);
		}
	}
	free(cmdstr);
	return cmd;
}


int main( int argc, char* argv[] ) {
	pid_t parent = fork();

	if ( parent == -1 ) {
		cerr << "Forking for main debugger failed\n";			
		exit( 420 );
	}

	if ( parent ) {
		int	status;
		//int insyscall = 0;
		long orig_rax, rax, ins;
		int brkpt = 0;
		int flag = 0;
		char *arg;

		// for our vector
		int brkpt_pos = 0;

		// breakpoints are set to next valid target

		// need array of breakpoint addresses --- might not be in order
		void* first_addr = (void*)0x00000000004007fd;				// fix to be main
		void* second_addr = (void*)0x0000000000400823;		
		void* third_addr = (void*)0x0000000000400845;				
		void* fourth_addr = (void*)0x000000000040088a;			

		// save all address so we can check if it is in a for loop or something

		waitpid( parent, &status, 0 );
		orig_rax = ptrace( PTRACE_PEEKUSER, parent, 8 * ORIG_RAX, NULL );

		// will send SIGTRAP at first execvp
		if ( WIFSTOPPED(status) ) {
			if ( WSTOPSIG(status) == SIGTRAP ) {				// before execve
				//cout << "Child stopped on SIGTRAP - continuing...\n";
				//cout << "The child made a system call " << orig_rax << " (execve)\n";
				printHelp();
				cout << "Program stopped\n(sdb) ";


				//cerr << "The program is not being run\n";

				// first breakpoint
				set_brkpt( parent, first_addr );
				++brkpt;

				// second breakpoint
				set_brkpt( parent, second_addr );
				++brkpt;

				// third breakpoint
				set_brkpt( parent, third_addr );
				++brkpt;

				// fourth breakpoint
				set_brkpt( parent, fourth_addr );
				++brkpt;

				ptrace( PTRACE_CONT, parent, NULL, NULL );
			}
		}

		do {
			waitpid( parent, &status, 0 );

			if ( WIFEXITED(status) || ( WIFSIGNALED(status) && WTERMSIG(status ) == SIGKILL) ) {
				cout << "Process " << parent << " terminated\n";
				break;
			}

			if ( WIFSTOPPED(status) ) {
				switch( next_cmd(&arg) )
				{
					case BREAK: /*set break point*/
						{

							break;
						}

					case PRINT:
						{

							break;
						}

					case NEXT: /*next instruction*/
						{
							//long rip =  ptrace(PTRACE_PEEKUSER, pid, 8 * RIP, NULL);

							/*read two words at eip*/
							//opcode[0] = ptrace(PTRACE_PEEKDATA, pid, rip, NULL);
							//opcode[1] = ptrace(PTRACE_PEEKDATA, pid, rip+sizeof(long), NULL);
							//old_rip = rip;

							/*next instruction*/
							ptrace( PTRACE_SINGLESTEP, parent, NULL, NULL );
							//wait(&status);
							break;
						}

					case REGISTER:			/* all registers */
						{
							getRegisters( parent );
							printRegisters( parent );
							break;
						}
					case CONTINUE:/*continue execution*/
						/*decrement eip*/ 
						ptrace( PTRACE_CONT, parent, NULL, NULL );
						//wait(&status);
						break;

					case KILL: 		/* kill process */
						kill( parent, SIGKILL );
						//wait(&status);
						break;

					default:		/* error */
						fprintf(stderr, "invalid or unknown command\n");
				}
			}

			cout << "Child stopped on address: " << get_instr_ptr( parent ) << "\n";

			if ( flag ) {
				// might want to set all breakpoints
				cout << "Resetting breakpoint to previous " << all_brkpts[brkpt_pos].orig_addr << " \n";
				set_brkpt( parent, (void*)all_brkpts[brkpt_pos].orig_addr );					
				++brkpt;
				flag = 0;
				cout << "\n--------------------------------------------------\n\n";
			}
			else {
				if ( is_brkpt(parent) ) {
					cout << "\n--------------------------------------------------------------------------------\n\n";
					cout << "In breakpoint: " << get_instr_ptr( parent ) << "\n";

					// get the breakpoint info from vector
					brkpt_info curr_brkpt = get_brkpt();

					brkpt_pos = brkpt_idx;

					// restore data to get rid of INT 3 and restore instr pointer
					handle_brkpt( parent, curr_brkpt.orig_addr, (void*)curr_brkpt.former_data );	

					cout << "In breakpoint after reset data: " << get_instr_ptr( parent ) << "\n";

					--brkpt;
					flag = 1;

					ptrace( PTRACE_SINGLESTEP, parent, NULL, NULL );
				}
			}

			if ( !flag )
				ptrace( PTRACE_CONT, parent, NULL, NULL );

		}
		while ( !WIFEXITED(status) );

		// can have option to rerun stuff
		cout << "Program finished - debugger terminating...\n";
	}
	else {
		if ( ptrace(PTRACE_TRACEME, 0, NULL, NULL) ) {
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
