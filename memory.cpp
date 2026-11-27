#include "memory.hpp"
#include <stdexcept>
#include <iostream>

Memory::Memory() {
    for (int i = 0; i < MEMORY_BYTES; ++i) {
        bytes[i] = 0;
    }
}

std::string Memory::readWord(int wordAddress) {
    if (wordAddress < 0 || wordAddress > TOTAL_WORDS - 1) {
        throw std::out_of_range("readWord: blogas adresas");  
    }
   
    int firstByteAddress = wordAddress * WORD_SIZE;
    std::string word(WORD_SIZE, '\0');
    for (int i = 0; i < WORD_SIZE; ++i) {
        word[i] = bytes[i + firstByteAddress];
    }

    return word;
    
}

void Memory::writeWord(int wordAddress, const std::string &word) {
    if (wordAddress < 0 || wordAddress > TOTAL_WORDS - 1) {
        throw std::out_of_range("writeWord: blogas adresas");  
    }

    if (word.size() > WORD_SIZE) {
        throw std::out_of_range("writeWord: per ilgas zodis"); 
    }
    
    std::string paddedWord(WORD_SIZE, '\0');

    for (int i = 0; i < (int)word.size(); ++i) {
        paddedWord[i] = word[i];
    }

    int firstByteAddress = wordAddress * WORD_SIZE;
    for (int i = 0; i < WORD_SIZE; ++i) {
        bytes[i + firstByteAddress] = paddedWord[i];
    }
}

Block Memory::readBlock(int blockNumber) {
    if (blockNumber < 0 || blockNumber > TOTAL_BLOCKS - 1) {
        throw std::out_of_range("readBlock: blogas bloko numeris"); 
    }

    int firstWordAddress = blockNumber * BLOCK_WORDS;
    Block block;
    for (int i = 0; i < BLOCK_WORDS; ++i) {
        block[i] = readWord(firstWordAddress + i);
    }

    return block;
}

void Memory::writeBlock(int blockNumber, const Block &block) {
    if (blockNumber < 0 || blockNumber > TOTAL_BLOCKS - 1) {
        throw std::out_of_range("writeBlock: blogas bloko numeris"); 
    }

    int firstWordAddress = blockNumber * BLOCK_WORDS;
    for (int i = 0; i < BLOCK_WORDS; ++i) {
        writeWord(firstWordAddress + i, block[i]);
    }
}

void Memory::printBlock(int blockNumber){
    Block block = readBlock(blockNumber);
    if (blockNumber < 10) std::cout << '0';
    std::cout << blockNumber << ": ";
    for (int i = 0; i < BLOCK_WORDS; ++i) {
        for (int j = 0; j < WORD_SIZE; ++j) {
            char c = block[i][j];

            if (c == '\0') {
                std::cout << '.';
            } 
            else if (c == ' ') {
                std::cout << '_';
            }
            else {
                std::cout << c;
            }
        }
        std::cout << ' ';
    }
    std::cout << '\n';
}