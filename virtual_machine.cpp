#include "virtual_machine.hpp"
#include "alu.hpp"
#include "paging.hpp"
#include <map>

uint8_t decode(const Word &word, Command &cmd, uint16_t &xy) {
    static const std::map<std::string, Command> command_map = {
    // arithmetic
    {"ADDA", Command::ADDA}, {"SUBA", Command::SUBA}, {"MULA", Command::MULA}, {"DIVA", Command::DIVA}, {"ANDA", Command::ANDA}, {"_ORA", Command::ORA}, {"XORA", Command::XORA}, {"NOTA", Command::NOTA}, {"COMP", Command::COMP},
    
    // jumps
    {"JP", Command::JP}, {"JE", Command::JE}, {"JN", Command::JN},
    {"JB", Command::JB}, {"Jb", Command::Jb}, {"JA", Command::JA}, {"Ja", Command::Ja},
    {"JL", Command::JL}, {"Jl", Command::Jl}, {"JG", Command::JG}, {"Jg", Command::Jg},
    
    // virtual memory
    {"LA", Command::LA}, {"LB", Command::LB}, {"SA", Command::SA}, {"SB", Command::SB},
    
    // shared memory
    {"LOCK", Command::LOCK}, {"ULCK", Command::ULCK}, {"RS", Command::RS}, {"WS", Command::WS},
    
    // other
    {"SWAP", Command::SWAP}, {"HALT", Command::HALT},
    
    // input/output
    {"GI", Command::GI}, {"PI", Command::PI}, {"GW", Command::GW}, {"PW", Command::PW}, {"GM", Command::GM}, {"PM", Command::PM}
    };

    if (command_map.count(word)) {
        cmd = command_map.find(word)->second;
        xy = 0;
    }
    else if (command_map.count(word.substr(0, WORD_SIZE / 2))) {
        cmd = command_map.find(word.substr(0, WORD_SIZE / 2))->second;
        if (!fromHex("00" + word.substr(WORD_SIZE / 2, WORD_SIZE), xy)) {
            return PI_ADDRESS;
        }
    }
    else {
        return PI_OPCODE;
    }

    return 0;
}

void step(Memory &mem, Cpu &cpu) {
    int realAddressOfCommand = translate(mem, cpu.PTR, cpu.PC);
    if (realAddressOfCommand == -1) {
        cpu.PI = PI_ADDRESS;
        return;
    }

    Word word = mem.readWord(realAddressOfCommand);
    ++cpu.PC;

    uint16_t xy;
    Command command;
    uint8_t error = decode(word, command, xy);
    if (error != 0) {
        cpu.PI = error;
        return;
    }

    uint16_t A_val, B_val;
    bool isNumA = fromHex(cpu.A, A_val); 
    bool isNumB = fromHex(cpu.B, B_val); 


    switch (command) {
        case Command::ADDA:
        case Command::SUBA:
        case Command::MULA:
        case Command::COMP:
        case Command::ANDA:
        case Command::ORA:
        case Command::XORA:
        {
            if (!isNumA || !isNumB) {
                cpu.PI = PI_ASSIGNMENT;
                return;
            }

            AluResult r;

            if (command == Command::ADDA) r = aluAdd(A_val, B_val);
            else if (command == Command::SUBA) r = aluSub(A_val, B_val);
            else if (command == Command::MULA) r = aluMul(A_val, B_val);
            else if (command == Command::COMP) r = aluSub(A_val, B_val);
            else if (command == Command::ANDA) r = aluAnd(A_val, B_val);
            else if (command == Command::ORA) r = aluOr(A_val, B_val);
            else if (command == Command::XORA) r = aluXor(A_val, B_val);

            
            if (command != Command::COMP) {
                cpu.A = toHex(r.value);
            }
            cpu.SF = r.flags;

            break;
        }
        
        case Command::DIVA:
        {
            if (!isNumA || !isNumB) {
                cpu.PI = PI_ASSIGNMENT;
                return;
            }

            if (divisionError(A_val, B_val)) {
                cpu.PI = PI_DIVISION;
                return;
            }

            AluResult r = aluDiv(A_val, B_val);
            
            cpu.A = toHex(r.value);
            cpu.SF = r.flags;

            break;
        }

        case Command::NOTA:
        {
            if (!isNumA) {
                cpu.PI = PI_ASSIGNMENT;
                return;
            }
            cpu.A = toHex(aluNot(A_val));

            break;
        }

        case Command::LA:
        case Command::LB:
        case Command::SA:
        case Command::SB:
        case Command::GI:
        case Command::GW:
        case Command::GM:
        case Command::PI:
        case Command::PW: 
        case Command::PM:
        {
            int realAddress= translate(mem, cpu.PTR, xy);
            // PM ir GM tik pirmo zodzio adresas validuojamas
            if (realAddress == -1) {
                cpu.PI = PI_ADDRESS;
                return;
            }

            if (command == Command::LA) cpu.A = mem.readWord(realAddress);
            else if (command == Command::LB) cpu.B = mem.readWord(realAddress);
            else if (command == Command::SA) mem.writeWord(realAddress, cpu.A);
            else if (command == Command::SB) mem.writeWord(realAddress, cpu.B);
            else if (command == Command::GI) cpu.SI = SI_GI;
            else if (command == Command::GW) cpu.SI = SI_GW;
            else if (command == Command::GM) cpu.SI = SI_GM;
            else if (command == Command::PI) cpu.SI = SI_PI;
            else if (command == Command::PW) cpu.SI = SI_PW;
            else if (command == Command::PM) cpu.SI = SI_PM;
        
            break;
        }

        case Command::RS:
        case Command::WS:
            if (xy >= SHARED_BLOCKS * BLOCK_WORDS) {
                cpu.PI = PI_ADDRESS;
            }

            // int realAddress = xy + SHARED_FIRST_BLOCK * BLOCK_WORDS;

            // cpu.A = mem.readWord(realAddress);
            else if (command == Command::RS) cpu.SI = SI_RS;
            else if (command == Command::WS) cpu.SI = SI_WS;
            break;

        case Command::LOCK:
            cpu.SI = SI_LOCK;
            break;
        
        case Command::ULCK:
            cpu.SI = SI_ULCK;
            break;

        case Command::SWAP:
        {
            Word temp = cpu.A;
            cpu.A = cpu.B;
            cpu.B = temp;
            break;
        }
            
        case Command::HALT:
            cpu.SI = SI_HALT;
            break;

        case Command::JP:
        case Command::JE:
        case Command::JN:
        case Command::JB:
        case Command::Jb:
        case Command::JA:
        case Command::Ja:
        case Command::JL:
        case Command::Jl:
        case Command::JG:
        case Command::Jg:
            if (shouldJump(word.substr(0, WORD_SIZE / 2), cpu.SF)) {
                cpu.PC = xy;
            }
            break;
    }
}