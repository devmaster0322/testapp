CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: libMemoryManager.a

libMemoryManager.a: MemoryManager.o
	ar rcs libMemoryManager.a MemoryManager.o

MemoryManager.o: MemoryManager.cpp MemoryManager.h
	$(CXX) $(CXXFLAGS) -c MemoryManager.cpp -o MemoryManager.o

clean:
	rm -f *.o *.a
