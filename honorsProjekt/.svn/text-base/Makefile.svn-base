all: parseValgrind tryingPtrace collatzTest faultyMalloc test testCamel

asm: collatzTest_asm faultyMalloc_asm test_asm testCamel_asm

hello: helloworld

helloworld: helloworld.cpp
	g++ -std=c++0x -g -w helloworld.cpp -o helloworld

parseValgrind: parseValgrind.cpp
	g++ -std=c++0x -w parseValgrind.cpp -o parseValgrind

tryingPtrace: tryingPtrace.cpp
	g++ -std=c++0x -w tryingPtrace.cpp -o tryingPtrace

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
	
clean:
	rm -f parseValgrind tryingPtrace collatzTest faultyMalloc test testCamel collatzTest.s  faultyMalloc.s test.s testCamel.s helloworld
