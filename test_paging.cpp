#include "paging.hpp"
#include <iostream>

int failed = 0;

void check(bool condition, const std::string &name) {
    if (!condition) {
        std::cout << "KLAIDA: " << name << '\n';
        ++failed;
    }
}

int main() {
    Memory m;

    std::array<std::string, BLOCK_WORDS> pageTable;

    for (int i = 0; i < BLOCK_WORDS; ++i) {
        pageTable[i] = toHex(2 * i + 1);
    }

    m.writeBlock(0x1F, pageTable);
    m.printBlock(0x1F);

    Word ptr = "001F";
    Word badPtr = "0044"; // 0x44 (68) blokas jau bendroj atminty
    Word notHexPtr = "00EJ";

    int virtualAddress1 = 0x12;
    int virtualAddress2 = 0xFF;
    int virtualAddress3 = 0X70;
    
    // realus = 16 * [16 * (16 * a2 + a3) + x1] + x2 

    // realus = 16 * [16 * (16 * 1 + 15) + 1] + 2 = 16 * 3 + 2 = 50
    check(translate(m, ptr, virtualAddress1) == 50, "translate(mem, 001F, 0x12)");

    check(translate(m, ptr, virtualAddress2) == 511, "translate(mem, 001F, 0xFF)");

    check(translate(m, badPtr, virtualAddress1) == -1, "translate(mem, 0044, 0x12)");

    check(translate(m, notHexPtr, virtualAddress1) == -1, "translate(mem, 00EJ, 0x12)");

    m.writeWord(0x1F * BLOCK_WORDS + 7, "HAHA");
    m.printBlock(0x1F);
    check(translate(m, ptr, virtualAddress1) == 50, "translate(mem, 001F, 0x12), kai sugadinta puslapiu lentele");
    check(translate(m, ptr, virtualAddress3) == -1, "translate(mem, 001F, 0x70), kai sugadinta puslapiu lentele");

    std::cout << (failed == 0 ? "test_paging: visi testai praejo\n" : "test_paging: yra klaidu\n");
    return failed == 0 ? 0 : 1;
}