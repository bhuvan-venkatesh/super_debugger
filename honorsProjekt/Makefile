all: super_debugger tryingPtrace collatzTest faultyMalloc test testCamel

# assembly code
asm: collatzTest_asm faultyMalloc_asm test_asm testCamel_asm

# a basic tester
hello: helloworld

helloworld: helloworld.cpp
	g++ -std=c++0x -g -w helloworld.cpp -o helloworld

super_debugger: super_debugger.cpp parseValgrind.cpp
	g++ -std=c++0x -w super_debugger.cpp -o super_debugger
	
tryingPtrace: tryingPtrace.cpp
	g++ -std=c++0x -w tryingPtrace.cpp -o tryingPtrace

# testers
collatzTest: collatzTest.cpp
	g++ -std=c++0x -g -w collatzTest.cpp -o collatzTest
	
collatzTest_asm: collatzTest.cpp
	g++ -S -std=c++0x collatzTest.cpp -o collatzTest.s
	
faultyMalloc: faultyMalloc.c 
	gcc -std=c99 -g -w faultyMalloc.c  -o faultyMalloc
	
faultyMalloc_asm:	faultyMalloc.c 
	gcc -S -w -std=c99 faultyMalloc.c -o faultyMalloc.s
	
test: test.c 
	gcc -std=c99 -g -w test.c -o test
	
test_asm: test.c
	gcc -S -w -std=c99 test.c -o test.s
	
testCamel: testCamel.c 
	gcc -std=c99 -g -w testCamel.c -o testCamel 
	
testCamel_asm: testCamel.c 
	gcc -S -w -std=c99 testCamel.c -o testCamel.s
	
# remove everything	
clean:
	rm -f super_debugger tryingPtrace collatzTest faultyMalloc test testCamel collatzTest.s  faultyMalloc.s test.s testCamel.s helloworld
