Module.o: Module.cpp Module.h
	g++ -c Module.cpp -std=gnu++11

clean:
	rm *.o 