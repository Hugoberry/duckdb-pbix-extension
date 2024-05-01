#pragma once

#include <vector>
#include <memory>



// Constants
constexpr size_t MAX_CODEWORD_LENGTH = 16;

// Data Structures
struct HuffmanNode {
    uint16_t symbol = 0;
    uint16_t bits = 0;
    std::shared_ptr<HuffmanNode> next;
};

using HuffmanTable = std::vector<uint16_t>;

// Function Prototypes
std::shared_ptr<HuffmanNode> sortSymbols(const std::vector<uint8_t>& bitLengthTable, std::vector<HuffmanNode>& nodes);
bool verifyFullBinaryTree(const std::vector<size_t>& bitLengthCount);
size_t fillDecodeTable(const std::shared_ptr<HuffmanNode>& sortedSymbols, HuffmanTable& decodeTable, size_t rootBits, size_t tailBits);
void createDecodeTables(const std::vector<uint8_t>& bitLengthTable, HuffmanTable& decodeTable, size_t rootBits, size_t tailBits);
uint16_t huffmanDecodeSymbol(const std::string& bitBuffer, size_t& offset, size_t bitBufferSize, const std::vector<uint16_t>& decodeTable, size_t rootLookupBits, size_t tailLookupBits);
uint16_t readBits(const std::string& bitBuffer, size_t& offset, size_t bitCount);