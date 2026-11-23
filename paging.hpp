#ifndef PAGING_HPP
#define PAGING_HPP

#include "memory.hpp"
#include "alu.hpp"

// virtualu adresa pavercia realiu.
// grazina -1, jei adresas neteisingas (VM tuomet nustato PI = 1)
int translate(Memory &mem, const Word &ptr, int virtualAddress);

#endif // PAGING_HPP