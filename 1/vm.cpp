#include <iostream>
#include <fstream>
#include <stdint.h>
#include <map>
#include <limits>

#define WORDS 16
#define BLOCKS 16

#define CARRY_FLAG 1
#define ZERO_FLAG 2
#define SIGN_FLAG 4
#define OVERFLOW_FLAG 8

using namespace std;

int32_t reg_A;
int32_t reg_B;
uint16_t reg_PC = 0;
uint8_t reg_SF;

int32_t memory[BLOCKS][WORDS];

enum Command
{
    ADDA, SUBA, MULA, DIVA, ANDA, ORA, XORA, NOTA, COMP, JP, JE, JN, JB, Jb, JA, Ja, JL, Jl, JG, Jg, LA, LB, SA, SB, LOCK, ULCK, RS, WS, SWAP, HALT, GI, PI, GW, PW, GM, PM
};

map<string, Command> command_map = {
    // arithmetic
    {"ADDA", Command::ADDA}, {"SUBA", Command::SUBA}, {"MULA", Command::MULA}, {"DIVA", Command::MULA}, {"ANDA", Command::ANDA}, {"_ORA", Command::ORA}, {"XORA", Command::XORA}, {"NOTA", Command::NOTA}, {"COMP", Command::COMP},
    
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

int32_t ascii_to_word(string s)
{
    int32_t result;
    for (int i = 0; i < 4; i++)
        result += ((uint8_t)s[i]) << 8*(3-i);
    return result;
}

string word_to_ascii(int32_t w)
{
    string result;
    for (int i = 0; i < 4; i++)
        result += (char)((w >> 8*(3-i)) % 256);
    return result;
}

int32_t read_memory(uint16_t address)
{
    uint8_t word = address % 16;
    uint8_t block = address >> 4;
    return memory[block][word];
}

int write_memory(uint16_t address, int32_t value)
{
    uint8_t word = address % 16;
    uint8_t block = address >> 4;
    memory[block][word] = value;
    return 0;
}

void parse_program(string program_name)
{
    int segment = 0;
    int address = 0;
    string line;
    ifstream file(program_name);
    while (getline(file, line))
    {
        if (line == "$COD")
            segment = 1;
        else if (line == "$DAT")
            segment = 2;
        else if (line == "$END")
            break;
        else
        {
            if (segment == 1)
            {
                string command = line.substr(0, 4);
                write_memory(address, ascii_to_word(command));
                address++;
            }
            else if (segment == 2)
            {
                address = stoul(line.substr(0, 2), nullptr, 16);
                if (line[5] == '"')     // storing string
                {
                    string text = line.substr(6, line.length() - 6);
                    text = text.substr(0, text.find('"'));
                    while (text.length() > 0)
                    {
                        write_memory(address, ascii_to_word(text.substr(0, 4)));
                        address++;
                        text = text.substr(4);
                    }
                }
                else
                {
                    write_memory(address, stoi(line.substr(5, 8), nullptr, 16));
                }
            }
        }        
    }
    file.close();
}

int main()
{
    // for (int i = 0; i < 16; i++)
    //     for (int j = 0; j < 16; j++)
    //         memory[i][j] = ascii_to_word("....");

    parse_program("program2.txt");

    // for (int i = 0; i < 16; i++)
    // {
    //     if (i < 10) cout << "0" << i << ": |";
    //     else cout << i << ": |";
    //     for (int j = 0; j < 16; j++)
    //         cout << word_to_ascii(memory[i][j]) << '|';
    //     cout << endl;
    // }

    while (true)
    {
        string command_string = word_to_ascii(read_memory(reg_PC));
        reg_PC++;
        int argument = 0;
        Command command;

        if (!command_map.count(command_string))
        {
            if (!command_map.count(command_string.substr(0, 2)))
            {
                cout << "Unrecognized command \"" << command_string << "\", exiting program." << endl;
                return -1;
            }
            command = command_map[command_string.substr(0, 2)];
            argument = stoi(command_string.substr(2, 2), nullptr, 16);
        }
        else command = command_map[command_string];

        cout << "[Executing command " << command_string << "]" << endl;
        switch (command)
        {
            case Command::ADDA:
                reg_A = reg_A + reg_B;
                break;
            case Command::SUBA:
                reg_A = reg_A - reg_B;
                break;
            case Command::MULA:
                reg_A = reg_A * reg_B;
                break;
            case Command::DIVA:
                reg_A = reg_A / reg_B;
                break;
            case Command::ANDA:
                reg_A = reg_A & reg_B;
                break;
            case Command::ORA:
                reg_A = reg_A | reg_B;
                break;
            case Command::XORA:
                reg_A = reg_A ^ reg_B;
                break;
            case Command::NOTA:
                reg_A = ~reg_A ;
                break;
            case Command::COMP:
                // TO-DO
                // int32_t res_signed = reg_B - reg_A;
                // uint32_t res_unsigned = res_signed;
                // if (res == 0)
                //     reg_SF = 0b00000010;
                // else if (res < 0)
                //     reg_SF = 0b00000010;
                break;
            case Command::JP:
                reg_PC = argument;
                break;
            case Command::JE:
                if ((reg_SF & ZERO_FLAG) == ZERO_FLAG)
                    reg_PC = argument;
                break;
            case Command::JN:
                if ((reg_SF & ZERO_FLAG) == 0)
                    reg_PC = argument;
                break;
            case Command::JB:
                if ((reg_SF & CARRY_FLAG) == CARRY_FLAG)
                    reg_PC = argument;
                break;
            case Command::Jb:
                if ((reg_SF & CARRY_FLAG) == CARRY_FLAG || (reg_SF & ZERO_FLAG) == 0)
                    reg_PC = argument;
                break;
            case Command::JA:
                if ((reg_SF & CARRY_FLAG) == 0 && (reg_SF & ZERO_FLAG) == 0)
                    reg_PC = argument;
                break;
            case Command::Ja:
                if ((reg_SF & CARRY_FLAG) == 0)
                    reg_PC = argument;
                break;
            case Command::JL:
                if ((reg_SF & SIGN_FLAG) >> 2 != (reg_SF & OVERFLOW_FLAG) >> 3)
                    reg_PC = argument;
                break;
            case Command::Jl:
                if ((reg_SF & ZERO_FLAG) == ZERO_FLAG || ((reg_SF & SIGN_FLAG) >> 2 != (reg_SF & OVERFLOW_FLAG) >> 3))
                    reg_PC = argument;
                break;
            case Command::JG:
                if ((reg_SF & ZERO_FLAG) == 0 || ((reg_SF & SIGN_FLAG) >> 2 == (reg_SF & OVERFLOW_FLAG) >> 3))
                    reg_PC = argument;
                break;
            case Command::Jg:
                if ((reg_SF & SIGN_FLAG) >> 2 == (reg_SF & OVERFLOW_FLAG) >> 3)
                    reg_PC = argument;
                break;
            case Command::LA:
                reg_A = read_memory(argument);
                break;
            case Command::LB:
                reg_B = read_memory(argument);
                break;
            case Command::SA:
                write_memory(argument, reg_A);
                break;
            case Command::SB:
                write_memory(argument, reg_B);
                break;
            case Command::LOCK:
                // TO-DO
                break;
            case Command::ULCK:
                // TO-DO
                break;
            case Command::RS:
                // TO-DO
                break;
            case Command::WS:
                // TO-DO
                break;
            case Command::SWAP:
            {
                int32_t temp;
                temp = reg_A;
                reg_A = reg_B;
                reg_B = temp;
                break;
            }
            case Command::HALT:
                return 0;
            case Command::GI:
            {
                int32_t input_i;
                while (!(cin >> input_i))
                {
                    // cout << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<int>::max(), '\n');
                }
                write_memory(argument, input_i);
                break;
            }
            case Command::PI:
            {
                int32_t output_i = read_memory(argument);
                cout << output_i;
                break;
            }
            case Command::GW:
            {
                string gw_input = "";
                int32_t input_w = 0;
                while (gw_input.length() <= 0 || gw_input.length() > 8)
                {
                    cin >> gw_input;
                    try
                    {
                        input_w = stoi(gw_input, nullptr, 16);
                    }
                    catch(const std::exception& e)
                    {
                        gw_input = "";
                    }
                }
                write_memory(argument, input_w);
                break;
            }
            case Command::PW:
            {
                int32_t output_w = read_memory(argument);
                cout << hex << output_w;
                break;
            }
            case Command::GM:
            {
                string gm_input = "";
                cin >> gm_input;
                while (gm_input.length() > 0)
                {
                    if (gm_input.length() >= 4)
                    {
                        write_memory(argument, ascii_to_word(gm_input.substr(0, 4)));
                        argument++;
                        gm_input = gm_input.substr(4);
                    }
                    else
                    {
                        for (int i = 0; i < 4 - gm_input.length(); i++)
                            gm_input += '\0';
                        write_memory(argument, ascii_to_word(gm_input));
                        argument++;
                        gm_input = "";
                    }
                }
                write_memory(argument, ascii_to_word("nnnn"));
                break;
            }
            case Command::PM:
            {
                string gm_output = word_to_ascii(read_memory(argument));
                while (gm_output != "nnnn")
                {
                    cout << gm_output;
                    argument++;
                    gm_output = word_to_ascii(read_memory(argument));
                }
                cout << endl;
                break;
            }
        }
    }
}