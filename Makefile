CXX = g++
CFLAGS = -std=c++11 -g -DDEBUG

OBJS = Memory.o OperandStack.o Loader.o Core.o Decode.o LocalStack.o ControlInst.o NumericInst.o ParaVarInst.o SysCall.o commonTypes.o

.PHONY:all clean

all: main

main: main.cpp $(OBJS)
	$(CXX) $(CFLAGS) -o main main.cpp $(OBJS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm main *.o 