#include "MemoryManager.h"
#include <iostream>

int main() {
    unsigned int wordSize = 8;
    size_t numberOfWords = 20;
    MemoryManager memoryManager(wordSize, [](int size, void* list) { return 0; });

    memoryManager.initialize(numberOfWords);
    std::cout << "Memory initialized with limit: " << memoryManager.getMemoryLimit() << " bytes\n";

    void* block1 = memoryManager.allocate(16);
    std::cout << "Allocated block1 at: " << block1 << "\n";

    void* block2 = memoryManager.allocate(8);
    std::cout << "Allocated block2 at: " << block2 << "\n";

    void* block3 = memoryManager.allocate(24);
    std::cout << "Allocated block3 at: " << block3 << "\n";

    memoryManager.free(block2);
    std::cout << "Freed block2\n";

    void* block4 = memoryManager.allocate(8);
    std::cout << "Allocated block4 at: " << block4 << "\n";

    memoryManager.shutdown();
    std::cout << "Memory manager shutdown complete.\n";

    return 0;
}
