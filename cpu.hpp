#ifndef CPU_HPP
#define CPU_HPP

#include "alu.hpp"

constexpr uint8_t SI_GI = 1;
constexpr uint8_t SI_GW = 2;
constexpr uint8_t SI_GM = 3;
constexpr uint8_t SI_PI = 4;
constexpr uint8_t SI_PW = 5;
constexpr uint8_t SI_PM = 6;
constexpr uint8_t SI_RS = 7;
constexpr uint8_t SI_WS = 8;
constexpr uint8_t SI_HALT = 9;
constexpr uint8_t SI_LOCK = 10;
constexpr uint8_t SI_ULCK = 11;

constexpr uint8_t PI_ADDRESS = 1;
constexpr uint8_t PI_OPCODE = 2;
constexpr uint8_t PI_DIVISION = 3;
constexpr uint8_t PI_ASSIGNMENT = 4;

constexpr uint8_t MODE_USER = 0;
constexpr uint8_t MODE_SUPER = 1;

constexpr uint8_t TI_START = 10;
constexpr uint8_t TICKS_DEFAULT = 1;
constexpr uint8_t TICKS_SI = 2; // IO ir bendra atmintis

struct Cpu {
    Word PTR = "0000", A = "0000", B = "0000";
    uint16_t PC = 0;
    uint8_t SF = 0, MODE = MODE_USER, PI = 0, SI = 0, TI = TI_START;
};

#endif // CPU_HPP
