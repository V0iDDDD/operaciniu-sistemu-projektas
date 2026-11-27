#include "virtual_machine.hpp"
#include "paging.hpp"
#include <array>
#include <string>
#include <iostream>

int main() {
    Memory mem;
    Block pageTable;
    Cpu cpu;

    for (int i = 0; i < BLOCK_WORDS; ++i) {
        pageTable[i] = toHex(2 * i + 1);
    }

    mem.writeBlock(0x1F, pageTable);

    cpu.PTR = "001F";

    mem.writeWord(translate(mem, cpu.PTR, BLOCK_WORDS), "0005");
    mem.writeWord(translate(mem, cpu.PTR, BLOCK_WORDS + 1), "0003");

    std::string commands[4] = {"LA10", "LB11", "ADDA", "HALT"};
    for (int i = 0; i < 4; ++i) {
        mem.writeWord(translate(mem, cpu.PTR, i), commands[i]);
    }

    std::cout << "Reali atmintis (vaizduojami tik netusti blokai): " << "\n";
    mem.printBlock(1);
    mem.printBlock(3);
    mem.printBlock(0x1F);
    std::cout << "\n";

    std::cout << "VM darbo eiga:" << "\n";
    int currStep = 0;
    while (cpu.SI == 0 && cpu.PI == 0) {
        std::cout << "Zingsnis " << currStep << ": " << "PC = " << cpu.PC << ", SI = " << (int)cpu.SI << ", PI = " << (int)cpu.PI << ", A = " << cpu.A << ", B = " << cpu.B << "\n";
        step(mem, cpu);
        ++currStep;
    }
    std::cout << "Zingsnis " << currStep << ": " << "PC = " << cpu.PC << ", SI = " << (int)cpu.SI << ", PI = " << (int)cpu.PI << ", A = " << cpu.A << ", B = " << cpu.B << "\n";
    std::cout << "\n";

    std::cout << "Virtuali atmintis (vaizduojami tik netusti blokai): " << "\n";
    std::cout << "(Virtualus blokas 0) ";
    mem.printBlock(1);
    std::cout << "(Virtualus blokas 1) ";
    mem.printBlock(3);
    return 0;
}