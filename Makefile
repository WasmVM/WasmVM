LINEAR_M:=lm
LINEAR_M_DEST:=Linear_Memory_map.txt
LOADER_TEST:=loader_test
LOADER_SRC:=Test/stuff.wasm
LOADER_DEST:=Linear_Memory_section.txt

.PHONY:all run clean

all: $(LINEAR_M) $(LOADER_TEST)

run: 
	./$(LINEAR_M) 4000 > $(LINEAR_M_DEST)
	./$(LOADER_TEST) -i $(LOADER_SRC) -v > $(LOADER_DEST)

$(LINEAR_M): Module.o main.o 
	g++ -o $(LINEAR_M) Module.o main.o 

$(LOADER_TEST): Module.o loader.o Fetcher.o
	g++ -o $(LOADER_TEST) Module.o loader.o Fetcher.o

loader.o: loader.cpp Module.h commonTypes.h
	g++ -c loader.cpp -std=gnu++11

Fetcher.o: Fetcher.cpp Fetcher.h Module.h
	g++ -c Fetcher.cpp -std=gnu++11

main.o: main.cpp Module.h commonTypes.h
	g++ -c main.cpp -std=gnu++11

Module.o: Module.cpp Module.h
	g++ -c Module.cpp -std=gnu++11

clean:
	rm *.o $(LINEAR_M) $(LINEAR_M_DEST) $(LOADER_TEST) $(LOADER_DEST)