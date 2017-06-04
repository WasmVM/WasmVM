EXEC:=lm

$(EXEC): Module.o main.o 
	g++ -o $(EXEC) Module.o main.o 

main.o: main.cpp Module.h commonTypes.h
	g++ -c main.cpp -std=gnu++11

Module.o: Module.cpp Module.h
	g++ -c Module.cpp -std=gnu++11

clean:
	rm *.o $(EXEC)