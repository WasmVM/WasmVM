EXEC:=lm
LOADER_TEST:=loader_test

all: $(EXEC) $(LOADER_TEST)

$(EXEC): Module.o main.o 
	g++ -o $(EXEC) Module.o main.o 

$(LOADER_TEST): Module.o loader.o 
	g++ -o $(LOADER_TEST) Module.o loader.o

loader.o: loader.cpp Module.h commonTypes.h
	g++ -c loader.cpp -std=gnu++11

main.o: main.cpp Module.h commonTypes.h
	g++ -c main.cpp -std=gnu++11

Module.o: Module.cpp Module.h
	g++ -c Module.cpp -std=gnu++11

clean:
	rm *.o $(EXEC) $(LOADER_TEST)