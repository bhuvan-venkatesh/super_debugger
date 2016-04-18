all: super_debugger collatzTest faultyMalloc test testCamel

# assembly code
asm: collatzTest_asm faultyMalloc_asm test_asm testCamel_asm

# a basic tester
hello: helloworld

helloworld: helloworld.cpp
	g++ -std=c++0x -g -w helloworld.cpp -o helloworld

super_debugger: super_debugger.cpp parseValgrind.cpp parseObjDump.cpp tryingPtrace.cpp
	g++ -std=c++0x -g -w super_debugger.cpp -o super_debugger 
	
#tryingPtrace: tryingPtrace.cpp
#	g++ -std=c++0x -w tryingPtrace.cpp -o tryingPtrace

# testers
collatzTest: testers/collatzTest.cpp
	g++ -std=c++0x -g -w testers/collatzTest.cpp -o collatzTest
	
collatzTest_asm: testers/collatzTest.cpp
	g++ -S -std=c++0x testers/collatzTest.cpp -o collatzTest.s
	
faultyMalloc: testers/faultyMalloc.c 
	gcc -std=c99 -g -w testers/faultyMalloc.c  -o faultyMalloc
	
faultyMalloc_asm:	testers/faultyMalloc.c 
	gcc -S -w -std=c99 testers/faultyMalloc.c -o faultyMalloc.s
	
test: testers/test.c 
	gcc -std=c99 -g -w testers/test.c -o test
	
test_asm: testers/test.c
	gcc -S -w -std=c99 testers/test.c -o test.s
	
testCamel: testers/testCamel.c 
	gcc -std=c99 -g -w testers/testCamel.c -o testCamel 
	
testCamel_asm: testers/testCamel.c 
	gcc -S -w -std=c99 testers/testCamel.c -o testCamel.s
	
# remove everything	
clean:
	rm -f super_debugger tryingPtrace collatzTest faultyMalloc test testCamel helloworld

# remove all the assembly
clean_asm:	
	collatzTest.s faultyMalloc.s test.s testCamel.s
