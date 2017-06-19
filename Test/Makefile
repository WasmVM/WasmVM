
CFLAGS:= -g -std=gnu++11
CXX:= g++

Tests:= LocalStack NumericInst

.PHONY:all clean

all:
	@echo "Available tests:"
	@echo $(Tests)

LocalStack: Test_LocalStack.cpp ../LocalStack.cpp
	$(CXX) $(CFLAGS) -o LocalStack Test_LocalStack.cpp ../LocalStack.cpp

NumericInst: Test_NumericInst.cpp ../NumericInst.cpp ../OperandStack.cpp
	$(CXX) $(CFLAGS) -o NumericInst Test_NumericInst.cpp ../NumericInst.cpp ../OperandStack.cpp

clean:
	rm *.o $(Tests)