LINEAR_M:=lm
LINEAR_M_DEST:=Linear_Memory_map.txt
LOADER_SRC:=../Test/0x1/Test.wasm
LOADER_DEST:=Linear_Memory_section.txt

CFLAGS:= -g -std=gnu++11

.PHONY:all run clean

all: $(LINEAR_M)

run: 
	./$(LINEAR_M) 4000 > $(LINEAR_M_DEST)
	./$(LINEAR_M) -i ../TestData/0x1/Test.wasm -v > Linear_Memory_section.txt

$(LINEAR_M): Memory.o Dumper.cpp Fetcher.o
	g++ $(CFLAGS) -o $(LINEAR_M) Dumper.cpp Memory.o Fetcher.o

Fetcher.o: Fetcher.cpp Fetcher.h ../Memory.h
	g++ $(CFLAGS) -c Fetcher.cpp 

Memory.o: ../Memory.cpp ../Memory.h
	g++ $(CFLAGS) -c ../Memory.cpp 

clean:
	rm *.o $(LINEAR_M) $(LINEAR_M_DEST) Dumper