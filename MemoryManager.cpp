#include "MemoryManager.h"
#include <iostream>
#include <fstream>
#include <climits> // new: included to access INT_MAX for bestFit function

// Constructor initializing word size and allocator function
MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void*)> allocator)
    : wordSize(wordSize), allocator(allocator), memoryStart(nullptr), memoryLimit(0) {
}

// Destructor to shut down memory manager when object is destroyed
MemoryManager::~MemoryManager() {
    shutdown();
}

// Initializes memory with a specified number of words
void MemoryManager::initialize(size_t sizeInWords) {
    if (sizeInWords > 65536) { // new: checks the maximum word limit
        std::cout << "Initialization failed: Exceeds maximum word limit of 65536." << std::endl;
        return;
    }

    if (memoryStart != nullptr) {
        delete[] memoryStart;
    }

    memoryStart = new char[sizeInWords * wordSize];
    memoryLimit = sizeInWords * wordSize;
    allocationStatus.resize(sizeInWords, false); // new: initializes all words as free
}

// Shuts down the memory manager and releases resources
void MemoryManager::shutdown() {
    if (memoryStart != nullptr) {
        delete[] memoryStart;
        memoryStart = nullptr;
        memoryLimit = 0;
    }
}

void* MemoryManager::allocate(size_t sizeInBytes) {
    size_t wordsNeeded = (sizeInBytes + wordSize - 1) / wordSize;

    // Debug output to check the parameters
    std::cout << "Calling allocator with wordsNeeded: " << wordsNeeded << std::endl;
    std::cout << "Allocation status address: " << static_cast<void*>(allocationStatus.data()) << std::endl;

    int offset = allocator(wordsNeeded, allocationStatus.data());

    // Debug output to check the result from allocator
    std::cout << "Allocator returned offset: " << offset << std::endl;

    if (offset < 0 || (offset + wordsNeeded) * wordSize > memoryLimit) return nullptr;

    for (size_t i = 0; i < wordsNeeded; ++i)
        allocationStatus[offset + i] = true;

    return static_cast<char*>(memoryStart) + (offset * wordSize);
}

// Frees a previously allocated block
void MemoryManager::free(void* address) {
    if (address == nullptr || address < memoryStart || address >= (static_cast<char*>(memoryStart) + memoryLimit)) {
        return; // does nothing if the address is invalid or out of range
    }

    size_t offset = (static_cast<char*>(address) - static_cast<char*>(memoryStart)) / wordSize;
    size_t i = offset;

    while (i < allocationStatus.size() && allocationStatus[i]) { // new: frees blocks until unallocated block
        allocationStatus[i] = false;
        ++i;
    }
}

// Sets the allocator function to either bestFit or worstFit
void MemoryManager::setAllocator(std::function<int(int, void*)> allocator) {
    this->allocator = allocator;
}

// Dumps the memory map to a file (stubbed functionality)
int MemoryManager::dumpMemoryMap(char* filename) {
    std::ofstream outfile(filename);
    if (!outfile) {
        return -1;
    }
    outfile << "Memory map output (to be implemented)" << std::endl; // new: placeholder for dumping memory map
    outfile.close();
    return 0;
}

// Placeholder function for getting a list of memory holes
void* MemoryManager::getList() {
    return nullptr;
}

// Generates a bitmap representing allocated and free blocks
void* MemoryManager::getBitmap() {
    int numWords = memoryLimit / wordSize;
    int bitmapSize = (numWords + 7) / 8;

    uint8_t* bitmap = new uint8_t[bitmapSize](); // new: initializes bitmap array to all 0s

    for (int i = 0; i < numWords; ++i) {
        if (allocationStatus[i]) { // checks if each word is allocated
            int byteIndex = i / 8;
            int bitIndex = i % 8;
            bitmap[byteIndex] |= (1 << bitIndex); // sets the bit for allocated words
        }
    }

    return bitmap;
}

// Returns the word size
unsigned MemoryManager::getWordSize() {
    return wordSize;
}

// Returns the starting address of memory
void* MemoryManager::getMemoryStart() {
    return memoryStart;
}

// Returns the memory limit in bytes
unsigned MemoryManager::getMemoryLimit() {
    return memoryLimit;
}

// Allocation strategy for finding the smallest available block
int bestFit(int sizeInWords, void* list) {
    int smallestFitSize = INT_MAX; // new: tracks the smallest fit size
    int bestOffset = -1;

    uint16_t* holeList = static_cast<uint16_t*>(list);
    uint16_t holeListLength = *holeList++;

    for (uint16_t i = 0; i < holeListLength; ++i) {
        uint16_t offset = holeList[2 * i];
        uint16_t size = holeList[2 * i + 1];

        if (size >= sizeInWords && size < smallestFitSize) { // checks for smallest fitting block
            smallestFitSize = size;
            bestOffset = offset;
        }
    }

    return bestOffset;
}

// Allocation strategy for finding the largest available block
int worstFit(int sizeInWords, void* list) {
    int largestFitSize = -1; // new: tracks the largest fit size
    int worstOffset = -1;

    uint16_t* holeList = static_cast<uint16_t*>(list);
    uint16_t holeListLength = *holeList++;

    for (uint16_t i = 0; i < holeListLength; ++i) {
        uint16_t offset = holeList[2 * i];
        uint16_t size = holeList[2 * i + 1];

        if (size >= sizeInWords && size > largestFitSize) { // checks for largest fitting block
            largestFitSize = size;
            worstOffset = offset;
        }
    }

    return worstOffset;
}
