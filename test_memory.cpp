// test_memory.cpp -- greitas Memory patikrinimas
#include "memory.hpp"
#include <iostream>

int main() {
    Memory m;
    m.writeWord(5, "JP00");      
    m.writeWord(4, "FFF6");
    m.writeWord(7, "EI");
    m.writeWord(BLOCK_WORDS * 5 + 12, "LABA");
    m.writeWord(BLOCK_WORDS * 5 + 13, " DIE");
    m.writeWord(BLOCK_WORDS * 5 + 14, "NA");
    m.printBlock(0);
    m.printBlock(5);

    Block block5 = m.readBlock(5);
    m.writeBlock(10, block5);
    m.printBlock(10);

    std::cout << "Realus adresas " << 0 * BLOCK_WORDS + 7 << ": " << m.readWord(0 * BLOCK_WORDS + 7) << '\n';
    std::cout << "Realus adresas " << 5 * BLOCK_WORDS + 12 << ": " << m.readWord(5 * BLOCK_WORDS + 12) << '\n';
    std::cout << "Atmintis: " << MEMORY_BYTES << " baitai, " << TOTAL_WORDS << " zodziai, " << TOTAL_BLOCKS << " blokai\n";
}
