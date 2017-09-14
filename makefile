CC = gcc -c -g
CFLAGS = -Wall -std=c99

yess: main.o fetchStage.o decodeStage.o executeStage.o memoryStage.o writebackStage.o memory.o registers.o tools.o dump.o loader.o
	gcc main.o fetchStage.o decodeStage.o executeStage.o memoryStage.o writebackStage.o memory.o registers.o tools.o dump.o loader.o -o yess 

main.o: main.c tools.h dump.h loader.h instructions.h executeStage.h forwarding.h status.h bubbling.h

fetchStage.o: fetchStage.c fetchStage.h tools.h instructions.h

memoryStage.o: memoryStage.c memoryStage.h tools.h instructions.h memory.h forwarding.h status.h bubbling.h

decodeStage.o: decodeStage.c decodeStage.h tools.h instructions.h forwarding.h status.h bubbling.h

executeStage.o: executeStage.c executeStage.h tools.h instructions.h forwarding.h status.h bubbling.h

writebackStage.o: writebackStage.c writebackStage.h tools.h instructions.h forwarding.h status.h bubbling.h

memory.o: memory.c memory.h tools.h instructions.h

registers.o: registers.c registers.h tools.h instructions.h

tools.o: tools.c tools.h

dump.o: dump.h fetchStage.h decodeStage.h memoryStage.h executeStage.h writebackStage.h registers.h memory.h instructions.h

loader.o: loader.c loader.h

clean: *.o 
	rm -rf *.o
