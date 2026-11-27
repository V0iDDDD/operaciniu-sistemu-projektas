#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <cstdint>
#include <string>
#include <array>

constexpr int WORD_SIZE     = 4;    
constexpr int BLOCK_WORDS   = 16;   
constexpr int USER_BLOCKS   = 70;   
constexpr int SHARED_BLOCKS = 2;
constexpr int SUPER_BLOCKS  = 16;

constexpr int TOTAL_BLOCKS = USER_BLOCKS + SUPER_BLOCKS;          
constexpr int TOTAL_WORDS  = TOTAL_BLOCKS * BLOCK_WORDS;          
constexpr int MEMORY_BYTES = TOTAL_WORDS * WORD_SIZE;             
 
constexpr int SHARED_FIRST_BLOCK = USER_BLOCKS - SHARED_BLOCKS;  
constexpr int SUPER_FIRST_BLOCK  = USER_BLOCKS;   

using Block = std::array<std::string, BLOCK_WORDS>;
 
class Memory {
public:
    Memory();

    std::string readWord(int wordAddress);
    void writeWord(int wordAddress, const std::string &word);

    Block readBlock(int blockNumber);
    void writeBlock(int blockNumber, const Block &block);
    void printBlock(int blockNumber);
    
private:
    uint8_t bytes[MEMORY_BYTES];
};

#endif // MEMORY_HPP