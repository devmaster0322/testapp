#include <climits>
#include "MemoryManager.h"
#include <string>
#include <cmath>
#include <array>
#include <sstream>
#include <fstream>
#include <vector>
#include <iostream>



// test cases
unsigned int testMemoryLeaksNoShutdown();
unsigned int testSimpleFirstFit();
unsigned int testSimpleBestFit();
unsigned int testComplexBestFit();
unsigned int testNewAllocator();
unsigned int testInvalidAllocate();
unsigned int testRepeatedShutdown();
unsigned int testMaxInitialization();
unsigned int testGetters();
unsigned int testReadingUsingGetMemoryStart();


// helper functions
std::string vectorToString(const std::vector<uint16_t>& vector);
unsigned int testGetBitmap(MemoryManager& memoryManager, uint16_t correctBitmapLength, std::vector<uint8_t> correctBitmap);
unsigned int testGetList(MemoryManager& memoryManager, uint16_t correctListLength, std::vector<uint16_t> correctList);
unsigned int testGetWordSize(MemoryManager& memoryManager, size_t correctWordSize);
unsigned int testGetMemoryLimit(MemoryManager& memoryManager, size_t correctMemoryLimit);
unsigned int testDumpMemoryMap(MemoryManager& memoryManager, std::string fileName, std::string correctFileContents);
/*
int bestFit(int sizeInWords, void* list) {
    int* holeList = static_cast<int*>(list);
    int bestFitOffset = -1;
    int smallestSize = INT_MAX;

    for (int i = 0; i < holeList[0]; ++i) {
        int offset = holeList[2 * i + 1];
        int holeSize = holeList[2 * i + 2];

        if (holeSize >= sizeInWords && holeSize < smallestSize) {
            smallestSize = holeSize;
            bestFitOffset = offset;
        }
    }

    return bestFitOffset;
}

int worstFit(int sizeInWords, void* list) {
    int* holeList = static_cast<int*>(list);
    int worstFitOffset = -1;
    int largestSize = -1;

    for (int i = 0; i < holeList[0]; ++i) {
        int offset = holeList[2 * i + 1];
        int holeSize = holeList[2 * i + 2];

        if (holeSize >= sizeInWords && holeSize > largestSize) {
            largestSize = holeSize;
            worstFitOffset = offset;
        }
    }

    return worstFitOffset;
}
*/
extern int bestFit(int sizeInWords, void* list);
extern int worstFit(int sizeInWords, void* list);


int hopesAndDreamsAllocator(int sizeInWords, void* list)
{
    static int wordOffsetIndex = 2;

    int largestWordOffsets[] = {-1,-1,-1};
    int largestWordOffsetsLength[] = {-1,-1,-1};


    uint16_t* holeList = static_cast<uint16_t*>(list);
    uint16_t holeListlength = *holeList++;

    for(uint16_t i = 1; i < (holeListlength) * 2; i += 2) {
        if(holeList[i] >= sizeInWords) {
            if(holeList[i] > largestWordOffsetsLength[2]) {
                largestWordOffsets[0] = largestWordOffsets[1];
                largestWordOffsets[1] = largestWordOffsets[2];
                largestWordOffsets[2] = holeList[i - 1];

                largestWordOffsetsLength[0] = largestWordOffsetsLength[1];
                largestWordOffsetsLength[1] = largestWordOffsetsLength[2];
                largestWordOffsetsLength[2] = holeList[i];

            }
            else if(holeList[i] > largestWordOffsetsLength[1]) {
                largestWordOffsets[0] = largestWordOffsets[1];
                largestWordOffsets[1] = holeList[i - 1];

                largestWordOffsetsLength[0] = largestWordOffsetsLength[1];
                largestWordOffsetsLength[1] = holeList[i];
            }
            else if(holeList[i] > largestWordOffsetsLength[0]) {
                largestWordOffsets[0] = holeList[i - 1];
                largestWordOffsetsLength[0] = holeList[i];
            }
        }
    }
    int wordOffset = largestWordOffsets[wordOffsetIndex];

    --wordOffsetIndex;
    if(wordOffsetIndex == -1) {
        wordOffsetIndex = 2;
    }

    return wordOffset;
}
// HERE IS THE  CODE - for the main function with debug statements for allocate and free calls

/*
int main() {
    std::cout << "Test Case: First Fit 1" << std::endl;
    
    // Create a MemoryManager instance with word size of 4
    MemoryManager memoryManager(4, bestFit);

    // Initialize memory
    memoryManager.initialize(1024);
    std::cout << "Memory initialized with limit: " << memoryManager.getMemoryLimit() << " bytes" << std::endl;

    // Test allocation and free
    std::cout << "Allocating memory of size 16 bytes..." << std::endl;
    void* allocatedMemory = memoryManager.allocate(16);
    std::cout << "Allocation returned: " << allocatedMemory << std::endl;

    std::cout << "Freeing memory at address: " << allocatedMemory << std::endl;
    memoryManager.free(allocatedMemory);
    std::cout << "Memory freed successfully." << std::endl;

    // Shutdown memory manager
    memoryManager.shutdown();
    std::cout << "Memory manager shutdown complete." << std::endl;

    return 0;
}
*/

/* OLD VERSION
int main()
{
    unsigned int maxScore = 38;
    unsigned int score = 0;
    
    score += testMemoryLeaksNoShutdown(); // 0
    
    score += testSimpleFirstFit(); // 3
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testSimpleBestFit(); // 3
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testComplexBestFit(); // 13
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testNewAllocator(); // 7
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testInvalidAllocate(); // 1
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testRepeatedShutdown(); // 3
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testMaxInitialization(); // 1
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += testGetters(); // 2
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
    
    score += 5 * testReadingUsingGetMemoryStart(); // 1 * 5
    
    std::cout << "Score: " << score << " / " <<  maxScore << std::endl;
}
*/

// neww version
int main()
{
    unsigned int maxScore = 38;
    unsigned int score = 0;
    
    score += testMemoryLeaksNoShutdown(); // 0
    std::cout << "Completed testMemoryLeaksNoShutdown. Score: " << score << " / " << maxScore << std::endl;
    
    score += testSimpleFirstFit(); // 3
    std::cout << "Completed testSimpleFirstFit. Score: " << score << " / " << maxScore << std::endl;
    
    score += testSimpleBestFit(); // 3
    std::cout << "Completed testSimpleBestFit. Score: " << score << " / " << maxScore << std::endl;
    
    score += testComplexBestFit(); // 13
    std::cout << "Completed testComplexBestFit. Score: " << score << " / " << maxScore << std::endl;
    
    score += testNewAllocator(); // 7
    std::cout << "Completed testNewAllocator. Score: " << score << " / " << maxScore << std::endl;
    
    score += testInvalidAllocate(); // 1
    std::cout << "Completed testInvalidAllocate. Score: " << score << " / " << maxScore << std::endl;
    
    score += testRepeatedShutdown(); // 3
    std::cout << "Completed testRepeatedShutdown. Score: " << score << " / " << maxScore << std::endl;
    
    score += testMaxInitialization(); // 1
    std::cout << "Completed testMaxInitialization. Score: " << score << " / " << maxScore << std::endl;
    
    score += testGetters(); // 2
    std::cout << "Completed testGetters. Score: " << score << " / " << maxScore << std::endl;
    
    score += 5 * testReadingUsingGetMemoryStart(); // 1 * 5
    std::cout << "Completed testReadingUsingGetMemoryStart. Final Score: " << score << " / " << maxScore << std::endl;
    
    // Additional debug statements for memory management
    std::cout << "\nStarting additional memory management tests...\n";
    
    MemoryManager memoryManager(4, bestFit);  // Assuming bestFit is the allocator to use
    memoryManager.initialize(1024);  // Initializes 1024 words (4096 bytes for word size 4)
    std::cout << "Memory initialized with limit: " << memoryManager.getMemoryLimit() << " bytes\n";

    // Test allocating 16 bytes
    std::cout << "Allocating 16 bytes...\n";
    void* allocatedMemory = memoryManager.allocate(16);
    std::cout << "Allocated memory returned: " << allocatedMemory << std::endl;

    // Test freeing allocated memory
    std::cout << "Freeing memory at address: " << allocatedMemory << std::endl;
    memoryManager.free(allocatedMemory);
    std::cout << "Memory at address " << allocatedMemory << " freed successfully.\n";

    // Shut down the memory manager
    memoryManager.shutdown();
    std::cout << "Memory manager shutdown complete.\n";

    return 0;
}

//end of new version 

unsigned int testMemoryLeaksNoShutdown()
{
    unsigned int score = 0;

    unsigned int wordSize = 8;
    size_t numberOfWords = 100;

    MemoryManager memoryManager(wordSize, bestFit);
    memoryManager.initialize(numberOfWords);
    uint32_t* testArray1 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray2 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray3 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray4 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray5 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    memoryManager.free(testArray2);

    memoryManager.free(testArray4);
    memoryManager.free(testArray5);
    memoryManager.free(testArray1);
    memoryManager.free(testArray3);
    return score;
}

/* OLD VERSION UNISGNED INT TESTSIMPLEFIRSTFIT
unsigned int testSimpleFirstFit()
{
    std::cout << "Test Case: First Fit 1" << std::endl;

    unsigned int  wordSize = 8;
    size_t numberOfWords = 26;
    MemoryManager memoryManager(wordSize, bestFit);
    memoryManager.initialize(numberOfWords);

    std::cout << "allocating and freeing memory..." << std::endl;

    uint64_t* testArray1 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    uint64_t* testArray2 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    uint64_t* testArray3 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    uint64_t* testArray4 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 6));

    memoryManager.free(testArray1);
    memoryManager.free(testArray3);

    std::vector<uint8_t> correctBitmap{0x00,0xCC,0x0F,0x00};
    uint16_t correctBitmapLength = correctBitmap.size();

    std::vector<uint16_t> correctList = {0, 10, 12, 2, 20, 6};
    uint16_t correctListLength = correctList.size() * 2;

    std::cout << "Testing Memory Manager state after allocations and frees" << std::endl;


    unsigned int score = 0;
    score += testGetBitmap(memoryManager, correctBitmapLength, correctBitmap);
    score += testGetList(memoryManager, correctListLength, correctList);
    score += testDumpMemoryMap(memoryManager, "testSimpleFirstFit.txt", vectorToString(correctList));

    memoryManager.shutdown();

    return score;
}
*/


// NEW 
unsigned int testSimpleFirstFit()
{
    std::cout << "Test Case: First Fit 1" << std::endl;

    unsigned int wordSize = 8;
    size_t numberOfWords = 26;
    MemoryManager memoryManager(wordSize, bestFit);
    memoryManager.initialize(numberOfWords);

    std::cout << "Memory initialized with limit: " << memoryManager.getMemoryLimit() << " bytes" << std::endl;
    std::cout << "Allocating and freeing memory..." << std::endl;

    // Test allocations with debug output
    std::cout << "Allocating 10 words (uint64_t * 10)..." << std::endl;
    uint64_t* testArray1 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    std::cout << "testArray1 allocated at: " << testArray1 << std::endl;

    std::cout << "Allocating 2 words (uint64_t * 2)..." << std::endl;
    uint64_t* testArray2 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    std::cout << "testArray2 allocated at: " << testArray2 << std::endl;

    std::cout << "Allocating 2 words (uint64_t * 2)..." << std::endl;
    uint64_t* testArray3 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    std::cout << "testArray3 allocated at: " << testArray3 << std::endl;

    std::cout << "Allocating 6 words (uint64_t * 6)..." << std::endl;
    uint64_t* testArray4 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 6));
    std::cout << "testArray4 allocated at: " << testArray4 << std::endl;

    // Test freeing memory with debug output
    std::cout << "Freeing testArray1..." << std::endl;
    memoryManager.free(testArray1);
    std::cout << "testArray1 freed." << std::endl;

    std::cout << "Freeing testArray3..." << std::endl;
    memoryManager.free(testArray3);
    std::cout << "testArray3 freed." << std::endl;

    // Check memory state
    std::cout << "Testing Memory Manager state after allocations and frees..." << std::endl;

    std::vector<uint8_t> correctBitmap{0x00, 0xCC, 0x0F, 0x00};
    uint16_t correctBitmapLength = correctBitmap.size();

    std::vector<uint16_t> correctList = {0, 10, 12, 2, 20, 6};
    uint16_t correctListLength = correctList.size() * 2;

    unsigned int score = 0;

    std::cout << "Running testGetBitmap..." << std::endl;
    score += testGetBitmap(memoryManager, correctBitmapLength, correctBitmap);

    std::cout << "Running testGetList..." << std::endl;
    score += testGetList(memoryManager, correctListLength, correctList);

    std::cout << "Running testDumpMemoryMap..." << std::endl;
    score += testDumpMemoryMap(memoryManager, "testSimpleFirstFit.txt", vectorToString(correctList));

    // Shutdown memory manager
    std::cout << "Shutting down memory manager..." << std::endl;
    memoryManager.shutdown();
    std::cout << "Memory manager shutdown complete." << std::endl;

    return score;
}


unsigned int testSimpleBestFit()
{
    std::cout << "Test Case: Best Fit 1" << std::endl;
    unsigned int wordSize = 8;
    size_t numberOfWords = 96;
    MemoryManager memoryManager(8, worstFit);
    memoryManager.initialize(numberOfWords);
    // allocate
    uint32_t* testArray1 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
    uint32_t* testArray2 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
    uint32_t* testArray3 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray4 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
    uint32_t* testArray5 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
    uint32_t* testArray6 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    

    // free specific allocations to create holes
    memoryManager.free(testArray3);
    memoryManager.free(testArray5);
    // change allocator
    memoryManager.setAllocator(bestFit);

    std::cout << "allocating 4 words" << std::endl;

    uint32_t* testArray7 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));


    std::vector<uint8_t> correctBitmap{0x0F, 0xFE, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctList = {4,5,18,78};
    uint16_t correctListLength = correctList.size() * 2;

    std::string correctFileContents = vectorToString(correctList);

    unsigned int score = 0;

    std::cout << "Testing Memory Manager state after allocation" << std::endl;

    score += testGetBitmap(memoryManager, correctBitmap.size(), correctBitmap);
    score += testGetList(memoryManager, correctListLength, correctList);
    score += testDumpMemoryMap(memoryManager, "testSimpleBestFit.txt", vectorToString(correctList));

    memoryManager.shutdown();

    return score;
}


unsigned int testComplexBestFit()
{
    std::cout << "Test Case: Best Fit 2" << std::endl;
    unsigned int wordSize = 4;
    size_t numberOfWords = 96;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords);

    uint32_t* testArray1 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray2 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));
    uint32_t* testArray3 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray4 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 3));
    uint32_t* testArray5 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray6 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 2));
    uint32_t* testArray7 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));
    uint32_t* testArray8 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 1));
    uint32_t* testArray9 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 10));


    unsigned int score = 0;

    std::vector<uint8_t> correctBitmapBeforeFree{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctListBeforeFree = {60, 36};
    uint16_t correctListLengthBeforeFree = correctListBeforeFree.size() * 2;

    std::cout << "Testing Memory Manager state after initial allocations" << std::endl;

    score += testGetBitmap(memoryManager, correctBitmapBeforeFree.size(), correctBitmapBeforeFree);
    score += testGetList(memoryManager, correctListLengthBeforeFree, correctListBeforeFree);

    

    // free specific allocations to create holes
    memoryManager.free(testArray2);
    memoryManager.free(testArray4);
    memoryManager.free(testArray6);
    memoryManager.free(testArray8);

    std::vector<uint8_t> correctBitmapAfterFree{0xFF, 0xC3, 0xFF, 0xF8, 0x9F, 0xFF, 0xFD, 0x0F, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctListAfterFree = {10, 4, 24, 3, 37, 2, 49, 1, 60, 36};
    uint16_t correctListLengthAfterFree = correctListAfterFree.size() * 2;

    std::cout << "Testing Memory Manager state after freeing specific areas " << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfterFree.size(), correctBitmapAfterFree);
    score += testGetList(memoryManager, correctListLengthAfterFree, correctListAfterFree);


    // change allocator
    memoryManager.setAllocator(bestFit);

    std::cout << "Allocating 1 words" <<std::endl;


    uint32_t* testArray10 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 1));

    std::vector<uint8_t> correctBitmapAfter1{0xFF, 0xC3, 0xFF, 0xF8, 0x9F, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter1 = {10, 4, 24, 3, 37, 2, 60, 36};
    uint16_t correctListLengthAfter1 = correctListAfter1.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfter1.size(), correctBitmapAfter1);
    score += testGetList(memoryManager, correctListLengthAfter1, correctListAfter1);

    std::cout << "Allocating 2 words" <<std::endl;


    uint32_t* testArray11 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 2));

    std::vector<uint8_t> correctBitmapAfter2{0xFF, 0xC3, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter2 = {10, 4, 24, 3, 60, 36};
    uint16_t correctListLengthAfter2 = correctListAfter2.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfter2.size(), correctBitmapAfter2);
    score += testGetList(memoryManager, correctListLengthAfter2, correctListAfter2);

    std::cout << "Allocating 3 words" <<std::endl;


    uint32_t* testArray12 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 3));

    std::vector<uint8_t> correctBitmapAfter3{0xFF, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter3 = {10, 4, 60, 36};
    uint16_t correctListLengthAfter3 = correctListAfter3.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfter3.size(), correctBitmapAfter3);
    score += testGetList(memoryManager, correctListLengthAfter3, correctListAfter3);

    uint32_t* testArray13 = static_cast<uint32_t*>(memoryManager.allocate(sizeof(uint32_t) * 4));

    std::cout << "Allocating 4 words" <<std::endl;


    std::vector<uint8_t> correctBitmapAfter4{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter4 = {60, 36};
    uint16_t correctListLengthAfter4 = correctListAfter4.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfter4.size(), correctBitmapAfter4);
    score += testGetList(memoryManager, correctListLengthAfter4, correctListAfter4);
    score += testDumpMemoryMap(memoryManager, "testComplexBestFit.txt", vectorToString(correctListAfter4));

    memoryManager.shutdown();

    return score;
}


unsigned int testNewAllocator()
{
    std::cout << "Test Case: New allocator";
    unsigned int wordSize = 8;
    size_t numberOfWords = 88;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords);


    uint64_t* testArray1 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    uint64_t* testArray2 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 13));
    uint64_t* testArray3 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    uint64_t* testArray4 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 8));
    uint64_t* testArray5 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    uint64_t* testArray6 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 4));
    uint64_t* testArray7 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));

    memoryManager.free(testArray2);
    memoryManager.free(testArray4);
    memoryManager.free(testArray6);

    unsigned int score = 0;

    memoryManager.setAllocator(hopesAndDreamsAllocator);

    std::cout << "Allocating 4 words" <<std::endl;

    uint64_t* testArray8 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 4));

    std::vector<uint8_t> correctBitmapAfter1{0xFF, 0x03, 0x80, 0xFF, 0x01, 0xFE, 0x87, 0xFF, 0x1F, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter1 = {10, 13, 33, 8, 51, 4, 69, 19};
    uint16_t correctListLengthAfter1 = correctListAfter1.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;

    score += testGetBitmap(memoryManager, correctBitmapAfter1.size(), correctBitmapAfter1);
    score += testGetList(memoryManager, correctListLengthAfter1, correctListAfter1);

    std::cout << "Allocating 4 words" <<std::endl;


    uint64_t* testArray9 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 4));
        
    std::vector<uint8_t> correctBitmapAfter2{0xFF, 0x3F, 0x80, 0xFF, 0x01, 0xFE, 0x87, 0xFF, 0x1F, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter2 = {14, 9, 33, 8, 51, 4, 69, 19};
    uint16_t correctListLengthAfter2 = correctListAfter2.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfter2.size(), correctBitmapAfter2);
    score += testGetList(memoryManager, correctListLengthAfter2, correctListAfter2);

    std::cout << "Allocating 4 words" <<std::endl;


    uint64_t* testArray10 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 4));

    std::vector<uint8_t> correctBitmapAfter3{0xFF, 0x3F, 0x80, 0xFF, 0x1F, 0xFE, 0x87, 0xFF, 0x1F, 0x00, 0x00};

    std::vector<uint16_t> correctListAfter3 = {14, 9, 37, 4, 51, 4, 69, 19};
    uint16_t correctListLengthAfter3 = correctListAfter3.size() * 2;

    std::cout << "Testing Memory Manager state\n" << std::endl;


    score += testGetBitmap(memoryManager, correctBitmapAfter3.size(), correctBitmapAfter3);
    score += testGetList(memoryManager, correctListLengthAfter3, correctListAfter3);
    score += testDumpMemoryMap(memoryManager, "testNewAllocator.txt", vectorToString(correctListAfter3));


    memoryManager.shutdown();

    return score;
}


unsigned int testInvalidAllocate()
{
    std::cout << "Test Case: invalid allocation, over the allowed amount" << std::endl;
    unsigned int wordSize = 2;
    size_t numberOfWords = 20;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords);

    uint16_t* testArray1 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * numberOfWords));
    uint16_t* testArray2 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));

    if(testArray2 == nullptr) {
        std::cout << "[CORRECT]\n" << std::endl;
        return 1;
    }
    else {
        std::cout << "[INCORRECT]\n" << std::endl;
        return 0;
    }
}


unsigned int testRepeatedShutdown()
{
    std::cout << "Test Case: repeated shutdown\nGenerating Memory Manager..." << std::endl;
    unsigned int wordSize = 2;
    size_t numberOfWords1 = 10;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords1);

    uint16_t* testArray1 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));
    uint16_t* testArray2 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 2));
    uint16_t* testArray3 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));
    uint16_t* testArray4 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 2));
    uint16_t* testArray5 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));

    memoryManager.free(testArray1);

    std::cout << "shutting down Memory Manager..." << std::endl;
    memoryManager.shutdown();

    std::cout << "initializing Memory Manager..." << std::endl;

    size_t numberOfWords2 = 20;

    memoryManager.initialize(numberOfWords2);

    uint16_t* testArray6 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));
    uint16_t* testArray7 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 2));
    uint16_t* testArray8 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));
    uint16_t* testArray9 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 2));
    uint16_t* testArray10 = static_cast<uint16_t*>(memoryManager.allocate(sizeof(uint16_t) * 1));

    memoryManager.free(testArray7);
    memoryManager.free(testArray8);


    std::vector<uint8_t> correctBitmap{0x71, 0x00, 0x00};

    std::vector<uint16_t> correctList = {1,3,7,13};
    uint16_t correctListLength = correctList.size() * 2;

    unsigned int score = 0;

    std::cout << "Testing Memory Manager state\n" << std::endl;
    score += testGetBitmap(memoryManager, correctBitmap.size(), correctBitmap);
    score += testGetList(memoryManager, correctListLength, correctList);
    score += testDumpMemoryMap(memoryManager, "testRepeatedShutdown.txt", vectorToString(correctList));


    memoryManager.shutdown();

    return score;

}


unsigned int testMaxInitialization()
{
    std::cout << "Test Case: max initialization"  << std::endl;
    unsigned int wordSize = 8;
    size_t numberOfWords = 65535;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords);

    uint64_t* testArray1 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 32768));
    uint64_t* testArray2 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 32767));

    if(testArray1 && testArray2) {
        std::cout << "[CORRECT]\n" << std::endl;
        return 1;
    }
    else {
        std::cout << "[INCORRECT]\n" << std::endl;
        return 0;
    }
}


unsigned int testGetters()
{
    // perfroms simpleFirstFit
    std::cout << "Test Case: testGetters, wordSize = 8, numberOfWords = 26" << std::endl;
    unsigned int wordSize = 8;
    size_t numberOfWords = 26;
    MemoryManager memoryManager(wordSize, bestFit);
    memoryManager.initialize(numberOfWords);

    uint64_t* testArray1 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 10));
    uint64_t* testArray2 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    uint64_t* testArray3 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 2));
    uint64_t* testArray4 = static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 6));

    memoryManager.free(testArray1);
    memoryManager.free(testArray3);

    unsigned int score = 0;
    score += testGetMemoryLimit(memoryManager, wordSize * numberOfWords); 
    score += testGetWordSize(memoryManager, wordSize);

    memoryManager.shutdown();



    return score;   
}


unsigned int testReadingUsingGetMemoryStart()
{
    std::cout << "Test Case: Reading memory using GetMemoryStart" << std::endl;
    unsigned int wordSize = 8;
    size_t numberOfWords = 80;
    MemoryManager memoryManager(wordSize, worstFit);
    memoryManager.initialize(numberOfWords);

    std::vector<uint64_t> arrayContent{1, 12, 13, 14, 15, 2, 22, 23, 24, 25, 3, 32, 33, 34, 35, 4, 42, 43, 44, 45, 5, 52, 53, 54, 55};

    std::vector<uint64_t>::const_iterator arrayContentItr = arrayContent.begin();
    std::vector<uint64_t*> testArrays;

    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    testArrays.push_back(static_cast<uint64_t*>(memoryManager.allocate(sizeof(uint64_t) * 5)));
    
    for(auto testArray: testArrays) {
        for(uint16_t i = 0; i < 5; ++i) {
            testArray[i] = *arrayContentItr++;
        }
    }

    unsigned int score = 0;

    arrayContentItr = arrayContent.begin();

    uint64_t* MemoryManagerContents = static_cast<uint64_t*>(memoryManager.getMemoryStart());
    
    for(auto content: arrayContent) {
        if(content != *MemoryManagerContents++) {
            std::cout << "Expected: " << content << std::endl;
            std::cout << "Got: "  << *(MemoryManagerContents - 1) << std::endl;
            std::cout << "[INCORRECT]\n" << std::endl;
            return score;
        }
    }

    ++score;

    memoryManager.shutdown();
    std::cout << "[CORRECT]\n" << std::endl;
    return score;
}


std::string vectorToString(const std::vector<uint16_t>& vector)
{
    std::string vectorString = "";

    vectorString += "[" + std::to_string(vector[0]) + ", " + std::to_string(vector[1]);
    for (int i = 2; i < vector.size(); i +=2)
    {
        vectorString += "] - [" + std::to_string(vector[i]) + ", " + std::to_string(vector[i + 1]);
    }
    vectorString += "]";

    return vectorString;
}


unsigned int testGetBitmap(MemoryManager& memoryManager, uint16_t correctBitmapLength, std::vector<uint8_t> correctBitmap)
{
    unsigned int score = 0;
    uint8_t* bitmap = static_cast<uint8_t*>(memoryManager.getBitmap());

    // Debug: Check if bitmap is null
    if (!bitmap) {
        std::cout << "Bitmap is null!" << std::endl;
        return 0;
    }

    uint8_t* bitmapEntryPoint = bitmap;

    uint8_t lowerByte = *bitmap++;
    uint8_t higherByte = *bitmap++;
    uint16_t byteStreamLength = (higherByte << 8) | lowerByte;

    // Debug: Print the byte stream length
    std::cout << "Byte stream length: " << byteStreamLength << std::endl;

    std::cout << "Testing getBitmap" << std::endl;
    std::cout << "Expected: " << std::endl;
    std::cout << "[0x" << std::hex << (int)correctBitmap[0];

    for(uint16_t i = 1; i < correctBitmap.size(); ++i) {
        std::cout << ", 0x" << std::hex << (int)correctBitmap[i];
    }
    std::cout << "]" << std::endl;

    std::cout << "\n";
    std::cout << "Got:"  << std::endl;
    std::cout << "[0x" << std::hex << (int)bitmap[0];
    for(uint16_t i = 1; i < byteStreamLength; ++i) {
        std::cout << ", 0x" << std::hex << (int)bitmap[i];
    }
    std::cout << "]" << std::endl;

    for(uint16_t i = 0; i < byteStreamLength; ++i) {
        if(bitmap[i] != correctBitmap[i]) {
            delete [] bitmapEntryPoint;
            std::cout << "[INCORRECT]\n" << std::endl;
            return score;
        }
    }

    ++score;
    delete [] bitmapEntryPoint;
    std::cout << "[CORRECT]\n" << std::endl;
    return score;
}


unsigned int testGetList(MemoryManager& memoryManager, uint16_t correctListLength, std::vector<uint16_t> correctList)
{
    unsigned int score = 0;
    std::cout << std::dec << std::endl;
    uint16_t* list = static_cast<uint16_t*>(memoryManager.getList());
    uint16_t* listEntryPoint = list;

    uint16_t listLength = *list++;

    uint16_t bytesPerEntry = 2;
    uint16_t entriesInList = listLength * bytesPerEntry;

    std::cout << "Testing getList" << std::endl;
    std::cout << "Expected: " << std::endl;
    std::cout << std::dec << "[" << correctList[0];
    for(uint16_t i = 1; i < correctList.size(); ++i) {
        std::cout <<"] - [" << correctList[i];
    }
    std::cout << "]" << std::endl;


    std::cout << "\n";
    std::cout << "Got:"  << std::endl;
    std::cout << std::dec << "[" << list[0];
    for(uint16_t i = 1; i < entriesInList; ++i) {
        std::cout <<"] - [" << list[i];
    }
    std::cout << "]" << std::endl;

    for(uint16_t i = 0; i < entriesInList; ++i) {
        if(list[i] != correctList[i]) {
            delete [] listEntryPoint;
            std::cout << "[INCORRECT]\n" << std::endl;
            return score;
        }
    }

    ++score;

    delete [] listEntryPoint;
    std::cout << "[CORRECT]\n" << std::endl;
    return score;
}

unsigned int testGetWordSize(MemoryManager& memoryManager, size_t correctWordSize)
{
    std::cout << "Testing getWordSize" << std::endl;
    std::cout << "Expected: " << correctWordSize << std::endl;
    std::cout << "Got:" << memoryManager.getWordSize() << std::endl;
    if(memoryManager.getWordSize() == correctWordSize) {
        std::cout << "[CORRECT]\n" << std::endl;
        return 1;
    }
    else {
        std::cout << "[INCORRECT]\n" << std::endl;
        return 0;
    }
}

unsigned int testGetMemoryLimit(MemoryManager& memoryManager, size_t correctMemoryLimit)
{
    std::cout << "Testing getMemoryLimit" << std::endl;
    std::cout << "Expected: " << correctMemoryLimit << std::endl;
    std::cout << "Got:" << memoryManager.getMemoryLimit() << std::endl;
    if(memoryManager.getMemoryLimit() == correctMemoryLimit) {
        std::cout << "[CORRECT]\n" << std::endl;
        return 1;
    }
    else {
        std::cout << "[INCORRECT]\n" << std::endl;
        return 0;
    }
}

unsigned int testDumpMemoryMap(MemoryManager& memoryManager, std::string fileName, std::string correctFileContents)
{
    std::cout << "Testing dumpMemoryMap" << std::endl;
  
    memoryManager.dumpMemoryMap((char*)fileName.c_str());
    std::ifstream testFile(fileName);
    if (testFile.is_open()) {
        std::string line;
        std::getline(testFile, line);
        testFile.close();
        std::cout << "Expected: " << correctFileContents << std::endl;
        std::cout << "Got:" << line << std::endl;
        if(line == correctFileContents) {
            std::cout << "[CORRECT]\n" << std::endl;
            return 1;
        }
        else {
            std::cout << "[INCORRECT]\n" << std::endl;
            return 0;
        }
    }
    return 0;
}
/*
int main() {
    // Example basic test for initialize, allocate, and shutdown
    MemoryManager memoryManager(4, [](int size, void* list) { return 0; });
    memoryManager.initialize(1024);
    std::cout << "Memory initialized with limit: " << memoryManager.getMemoryLimit() << " bytes\n";

    void* allocatedMemory = memoryManager.allocate(16);
    if (allocatedMemory) {
        std::cout << "Memory allocated successfully.\n";
    } else {
        std::cout << "Failed to allocate memory.\n";
    }

    memoryManager.shutdown();
    std::cout << "Memory shutdown complete.\n";

    return 0;
}
*/
