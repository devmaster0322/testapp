#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <cstddef>
#include <functional>

class MemoryManager {
public:
    MemoryManager(unsigned int wordSize, std::function<int(int, void*)> allocator); // Constructor
    ~MemoryManager(); // Destructor
    void initialize(size_t numberOfWords); // Initializes memory
    void shutdown(); // Shuts down and releases memory
    void* allocate(size_t sizeInBytes); // Allocates a block of memory
    void free(void* address); // Frees a previously allocated block
    void setAllocator(std::function<int(int, void*)> allocator); // Sets the allocation strategy
    int dumpMemoryMap(char* filename); // Dumps memory map to a file
    unsigned getWordSize(); // Gets the word size
    unsigned getMemoryLimit(); // Gets the memory limit
    void* getMemoryStart(); // Gets the starting address of memory
    void* getBitmap(); // Returns bitmap of allocated memory
    void* getList(); // Placeholder for getting the list of memory holes

private:
    unsigned int wordSize; // Size of each word
    unsigned int memoryLimit; // Limit of memory in bytes
    char* memoryStart; // Starting address of memory
    std::function<int(int, void*)> allocator; // Function pointer to allocation strategy
    std::vector<uint8_t> allocationStatus;

};

#endif // MEMORY_MANAGER_H
