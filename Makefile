CXX = g++
CFLAGS = -std=c++11

OBJS = Memory.o OperandStack.o Loader.o Core.o

.PHONY:all clean

all: main

main: main.cpp $(OBJS)
	$(CXX) $(CFLAGS) -o main main.cpp  $(OBJS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm main *.o 