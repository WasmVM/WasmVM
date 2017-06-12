
CFLAGS:= -g -std=gnu++11
CXX:= g++

Tests:= LocalStack

.PHONY:all clean

all:
	@echo "Available tests:"
	@echo $(Tests)

LocalStack: Test_LocalStack.cpp ../LocalStack.cpp
	$(CXX) $(CFLAGS) -o Test_LocalStack Test_LocalStack.cpp ../LocalStack.cpp

clean:
	rm *.o