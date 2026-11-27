#ifndef ALU_HPP
#define ALU_HPP

#include <cstdint>
#include <string>

using Word = std::string;   // 4 simboliai; skaicius - 4 hex simboliai ("FFF6" = -10)
constexpr int HEX_DIGITS = 4;

constexpr uint8_t FLAG_CF = 1;
constexpr uint8_t FLAG_ZF = 2;
constexpr uint8_t FLAG_SF = 4;
constexpr uint8_t FLAG_OF = 8;

// KONVERSIJOS

// false, jei zodis nera 4 hex simboliai (0-9, A-F)
bool fromHex(const Word &word, uint16_t &out);
Word toHex(uint16_t value);
int toSigned(uint16_t value);

// ARITMETIKA IR LOGIKA

struct AluResult {
    uint16_t value;
    uint8_t  flags;
};

AluResult aluAdd(uint16_t a, uint16_t b);   // ADDA
AluResult aluSub(uint16_t a, uint16_t b);   // SUBA, COMP
AluResult aluMul(uint16_t a, uint16_t b);   // MULA

bool divisionError(uint16_t a, uint16_t b);
AluResult aluDiv(uint16_t a, uint16_t b);   // DIVA, tik jei !divisionError

AluResult aluAnd(uint16_t a, uint16_t b);   // ANDA
AluResult aluOr (uint16_t a, uint16_t b);   // _ORA
AluResult aluXor(uint16_t a, uint16_t b);   // XORA
uint16_t  aluNot(uint16_t a);               // NOTA, zymiu nekeicia

// VALDYMO PERDAVIMAI

// ar valdymo perdavimo komanda op ("JP", "JE", ...) vykdoma, esant zymems flags
bool shouldJump(const std::string &op, uint8_t flags);

#endif // ALU_HPP
