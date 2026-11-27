#ifndef VIRTUAL_MACHINE_HPP
#define VIRTUAL_MACHINE_HPP

#include "memory.hpp"
#include "cpu.hpp"
#include <cstdint>

enum class Command // kad komandos nesikirstu su registru vardais
{
    ADDA, SUBA, MULA, DIVA, COMP,
    ANDA, ORA, XORA, NOTA, 
    JP, JE, JN, JB, Jb, JA, Ja, JL, Jl, JG, Jg, 
    LA, LB, SA, SB, 
    LOCK, ULCK, RS, WS, 
    SWAP, HALT, 
    GI, PI, GW, PW, GM, PM
};

// iskaido zodi: komanda ir (jei yra) adresas xy. 
// 0 - viskas gerai, priesingu atveju - kuri PI nustatyti
uint8_t decode(const Word &word, Command &cmd, uint16_t &xy);

void step(Memory &mem, Cpu &cpu);

#endif // VIRTUAL_MACHINE_HPP