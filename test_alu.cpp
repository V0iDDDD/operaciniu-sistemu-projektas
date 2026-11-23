#include "alu.hpp"
#include <iostream>

int failed = 0;

void check(bool condition, const std::string &name) {
    if (!condition) {
        std::cout << "KLAIDA: " << name << '\n';
        ++failed;
    }
}

int main() {
    uint16_t v = 0;

    // KONVERSIJOS
    check(fromHex("00FF", v) && v == 255, "fromHex(00FF)");
    check(fromHex("FFF6", v) && v == 65526, "fromHex(FFF6)");
    check(!fromHex("Ives", v), "fromHex(Ives) -> false");
    check(!fromHex("00ff", v), "fromHex(00ff) -> false");
    check(!fromHex("AB", v), "fromHex(AB) -> false");
    check(toHex(255) == "00FF", "toHex(255)");
    check(toHex(0) == "0000", "toHex(0)");
    check(toSigned(0xFFF6) == -10, "toSigned(FFF6)");
    check(toSigned(0x7FFF) == 32767, "toSigned(7FFF)");

    // ARITMETIKA
    AluResult r = aluAdd(0x7FFF, 0x0001);
    check(r.value == 0x8000 && r.flags == (FLAG_SF | FLAG_OF), "7FFF + 0001");

    r = aluAdd(0xFFFF, 0x0001);
    check(r.value == 0 && r.flags == (FLAG_CF | FLAG_ZF), "FFFF + 0001");

    r = aluSub(3, 5);
    check(r.value == 0xFFFE && r.flags == (FLAG_CF | FLAG_SF), "3 - 5");

    r = aluMul(0xFFFF, 0xFFFF);
    check(r.value == 1 && r.flags == FLAG_CF, "FFFF * FFFF");

    r = aluDiv(0xFFF9, 2);
    check(r.value == 0xFFFD && r.flags == FLAG_SF, "-7 / 2"); // -3

    check(divisionError(5, 0), "dalyba is 0");
    check(divisionError(0x8000, 0xFFFF), "8000 / FFFF");
    check(!divisionError(0x8000, 1), "8000 / 0001");

    // LOGIKA
    check(aluAnd(0xF000, 0x0FFF).flags == FLAG_ZF, "F000 & 0FFF");
    check(aluXor(0x1234, 0x1234).value == 0, "x ^ x");
    check(aluNot(0x00FF) == 0xFF00, "~00FF");

    // VALDYMO PERDAVIMAI (po COMP)
    uint8_t eq = aluSub(5, 5).flags;
    check(jumpCondition("JE", eq) && !jumpCondition("JN", eq), "JE/JN, kai lygu");

    uint8_t f = aluSub(0x7FFF, 0xFFFF).flags;          
    check(jumpCondition("JG", f), "JG: 32767 > -1 (zenkliskai)");
    check(jumpCondition("JB", f), "JB: 7FFF < FFFF (nezenkliskai)");

    std::cout << (failed == 0 ? "test_alu: visi testai praejo\n" : "test_alu: yra klaidu\n");
    return failed == 0 ? 0 : 1;
}
