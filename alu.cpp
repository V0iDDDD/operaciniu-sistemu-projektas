#include "alu.hpp"


// KONVERSIJOS

bool fromHex(const Word &word, uint16_t &out) {
    if (word.size() != HEX_DIGITS) {
        return false;
    }

    uint16_t result = 0;
    for (int i = 0; i < HEX_DIGITS; ++i) {
        if (word[i] >= '0' && word[i] <= '9') {
            result = result * 16 + (word[i] - '0');
        }

        else if (word[i] >= 'A' && word[i] <= 'F') {
            result = result * 16 + (word[i] - 'A' + 10);
        }

        else {
            return false;
        }
    }

    out = result;
    return true;
}

Word toHex(uint16_t value) {
    Word hexWord(HEX_DIGITS, '0');
    const std::string hexDigits = "0123456789ABCDEF";

    for (int i = 0; i < HEX_DIGITS; ++i) {
        hexWord[HEX_DIGITS - 1 - i] = hexDigits[value % 16];
        value = value / 16;
    }
    return hexWord;
}

int toSigned(uint16_t value) {
    return value >= 0x8000 ? (int)value - 65536 : value;
}


// ARITMETIKA IR LOGIKA

static bool isNegative(uint16_t x) {
    return x >= 0x8000;
}

// ZF ir SF visoms komandoms skaiciuojami vienodai
static uint8_t zeroAndSignFlags(uint16_t r) {
    uint8_t flags = 0;
    if (r == 0) {
        flags |= FLAG_ZF;
    }
    if (isNegative(r)) {
        flags |= FLAG_SF;
    }
    return flags;
}

AluResult aluAdd(uint16_t a, uint16_t b) {
    uint32_t sum = (uint32_t)a + (uint32_t)b;   // platesnis tipas, kad matytume CF
    uint16_t r = (uint16_t)sum;

    uint8_t flags = zeroAndSignFlags(r);

    if (sum > 0xFFFF) {
        flags |= FLAG_CF;
    }

    if (isNegative(a) == isNegative(b) && isNegative(r) != isNegative(a)) {
        flags |= FLAG_OF;
    }

    return {r, flags};
}

AluResult aluSub(uint16_t a, uint16_t b) {
    uint16_t r = a - b;

    uint8_t flags = zeroAndSignFlags(r);

    if (a < b) {
        flags |= FLAG_CF;
    }

    if (isNegative(a) != isNegative(b) && isNegative(a) != isNegative(r)) {
        flags |= FLAG_OF;
    }

    return {r, flags};
}

AluResult aluMul(uint16_t a, uint16_t b) {
    uint32_t unsignedProduct = (uint32_t)a * (uint32_t)b;   // CF
    int signedProduct = toSigned(a) * toSigned(b);          // OF

    uint16_t r = (uint16_t)unsignedProduct;

    uint8_t flags = zeroAndSignFlags(r);

    if (unsignedProduct > 0xFFFF) {
        flags |= FLAG_CF;
    }

    if (signedProduct < -32768 || signedProduct > 32767) {
        flags |= FLAG_OF;
    }

    return {r, flags};
}

bool divisionError(uint16_t a, uint16_t b) {
    return b == 0x0000 || (a == 0x8000 && b == 0xFFFF);
}

AluResult aluDiv(uint16_t a, uint16_t b) {
    int quotient = toSigned(a) / toSigned(b);
    uint16_t r = (uint16_t)quotient;
    return {r, zeroAndSignFlags(r)};
}

AluResult aluAnd(uint16_t a, uint16_t b) {
    uint16_t r = a & b;
    return {r, zeroAndSignFlags(r)};
}

AluResult aluOr(uint16_t a, uint16_t b) {
    uint16_t r = a | b;
    return {r, zeroAndSignFlags(r)};
}

AluResult aluXor(uint16_t a, uint16_t b) {
    uint16_t r = a ^ b;
    return {r, zeroAndSignFlags(r)};
}

uint16_t aluNot(uint16_t a) {
    return ~a;
}


// VALDYMO PERDAVIMAI

bool shouldJump(const std::string &op, uint8_t flags) {
    bool cf = (flags & FLAG_CF) != 0;
    bool zf = (flags & FLAG_ZF) != 0;
    bool sf = (flags & FLAG_SF) != 0;
    bool of = (flags & FLAG_OF) != 0;

    if (op == "JP") return true;

    if (op == "JE") return zf;                     // A == B
    if (op == "JN") return !zf;                    // A != B

    // Nezenkliniai
    if (op == "JB") return cf;                     // A <  B
    if (op == "Jb") return cf || zf;               // A <= B
    if (op == "JA") return !cf && !zf;             // A >  B
    if (op == "Ja") return !cf;                    // A >= B

    // Zenkliniai
    if (op == "JL") return sf != of;               // A <  B
    if (op == "Jl") return zf || sf != of;         // A <= B
    if (op == "JG") return !zf && sf == of;        // A >  B
    if (op == "Jg") return sf == of;               // A >= B

    return false;
}
