#include "paging.hpp"

int translate(Memory &mem, const Word &ptr, int virtualAddress) {
    if (virtualAddress < 0 || virtualAddress > 0xFF) {
        return -1;
    }

    uint16_t tableBlock; // a2 * 16 + a3, kur ptr = a0a1a2a3

    if (!fromHex(ptr, tableBlock)) {
        return -1;
    }

    if (tableBlock >= SHARED_FIRST_BLOCK) {
        return -1;
    }

    int x1 = virtualAddress / BLOCK_WORDS;
    int x2 = virtualAddress % BLOCK_WORDS;

    Word pageTableEntry = mem.readWord(BLOCK_WORDS * tableBlock + x1);

    uint16_t realBlock;

    if (!fromHex(pageTableEntry, realBlock)) {
        return -1;
    }

    if (realBlock >= SHARED_FIRST_BLOCK) {
        return -1;
    }

    return BLOCK_WORDS * realBlock + x2;
}