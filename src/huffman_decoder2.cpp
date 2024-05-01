#include "huffman_decoder2.h"

std::shared_ptr<HuffmanNode> sortSymbols(const std::vector<uint8_t> &bitLengthTable, std::vector<HuffmanNode> &nodes)
{
    std::vector<std::vector<std::shared_ptr<HuffmanNode>>> chains(MAX_CODEWORD_LENGTH);
    size_t symbolCount = 0;

    for (size_t i = 0; i < bitLengthTable.size(); ++i)
    {
        uint8_t bitLength = bitLengthTable[i];
        if (bitLength != 0)
        {
            nodes[symbolCount] = {static_cast<uint16_t>(i), static_cast<uint16_t>(bitLength), nullptr};
            chains[bitLength].push_back(std::make_shared<HuffmanNode>(nodes[symbolCount]));
            symbolCount++;
        }
    }

    std::shared_ptr<HuffmanNode> head = nullptr;
    for (int i = MAX_CODEWORD_LENGTH - 1; i >= 0; --i)
    {
        for (auto &node : chains[i])
        {
            node->next = head;
            head = node;
        }
    }

    return head;
}

bool verifyFullBinaryTree(const std::vector<size_t>& bitLengthCount) {
    size_t n = 0;
    for (int i = MAX_CODEWORD_LENGTH - 1; i > 0; --i) {
        n += bitLengthCount[i];
        if (n % 2 != 0) return false;
        n /= 2;
    }
    return n == 1;
}

// Utility function to reverse bits (e.g., 3-bit 101 -> 101 in reverse)
uint16_t reverseBits(uint16_t num, size_t bitCount)
{
    uint16_t reversed = 0;
    for (size_t i = 0; i < bitCount; ++i)
    {
        reversed = (reversed << 1) | (num & 1);
        num >>= 1;
    }
    return reversed;
}

size_t fillDecodeTable(const std::shared_ptr<HuffmanNode> &sortedSymbols, 
                      std::vector<uint16_t> &decodeTable, 
                      size_t rootBits, 
                      size_t tailBits) {

    size_t tableOffset = 0;
    size_t currentLevel = 0; // Track the current level in the tree

    std::shared_ptr<HuffmanNode> currentNode = sortedSymbols;

    while (currentNode) {
        size_t symbolBits = currentNode->bits;

        // Determine the level of the node more accurately
        while (currentLevel < symbolBits && currentLevel < rootBits) {
            currentLevel++; 
        }

        size_t tableSize = 1 << (symbolBits - currentLevel);

        for (size_t i = 0; i < tableSize; ++i) {
            uint16_t index = reverseBits(i, symbolBits - currentLevel);
            uint16_t value = (currentNode->symbol << 4) | (symbolBits - currentLevel);
            decodeTable[tableOffset + index] = value;
        }

        currentNode = currentNode->next;
        tableOffset += tableSize;

        // Adjust currentLevel if needed after processing the node
        while (currentLevel > 0 && 
               (currentNode == nullptr || currentNode->bits < currentLevel)) {
            currentLevel--;
        }
    }

    return tableOffset;
}

void createDecodeTables(const std::vector<uint8_t> &bitLengthTable, HuffmanTable &decodeTable, size_t rootBits, size_t tailBits)
{
    std::vector<HuffmanNode> nodes(bitLengthTable.size());
    std::shared_ptr<HuffmanNode> sortedSymbols = sortSymbols(bitLengthTable, nodes);

    std::vector<size_t> bitLengthCount(MAX_CODEWORD_LENGTH, 0);
    for (const auto &node : nodes)
    {
        bitLengthCount[node.bits]++;
    }

    if (!verifyFullBinaryTree(bitLengthCount))
    {
        throw std::runtime_error("Invalid Huffman tree");
    }

    fillDecodeTable(sortedSymbols, decodeTable, rootBits, tailBits);
}


// Helper function to read a specific number of bits from the bit buffer
uint16_t readBits(const std::string& bitBuffer, size_t& offset, size_t bitCount) {
    uint16_t result = 0;
    for (size_t i = 0; i < bitCount; ++i) {
        result = (result << 1) | ((bitBuffer[offset / 8] >> (7 - offset % 8)) & 1);
        ++offset;
    }
    return result;
}

// Function to decode a Huffman-encoded symbol from the bit buffer
uint16_t huffmanDecodeSymbol(const std::string& bitBuffer, size_t& offset, size_t bitBufferSize, const std::vector<uint16_t>& decodeTable, size_t rootLookupBits, size_t tailLookupBits) {
    uint16_t index = readBits(bitBuffer, offset, rootLookupBits);
    uint16_t symbolValue = decodeTable[index];

    while (symbolValue & (1 << 15)) {
        offset += (symbolValue & 0x0F);
        size_t nextTableOffset = (symbolValue >> 4) & ~(1 << 15);
        index = readBits(bitBuffer, offset, tailLookupBits);
        symbolValue = decodeTable[nextTableOffset + index];
    }

    offset += (symbolValue & 0x0F);
    return symbolValue >> 4;
}