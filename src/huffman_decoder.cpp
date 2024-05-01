#include "huffman_decoder.h"

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

bool verifyFullBinaryTree(const std::vector<size_t> &bitLengthCount)
{
    size_t n = 0;
    for (int i = MAX_CODEWORD_LENGTH - 1; i >= 0; i--)
    {
        n += bitLengthCount[i];
        printf("n: %d i: %d\n", n,i);
        if ((n & 1) != 0)
            return false;
        n >>= 1;
    }
    printf("n: %d\n", n);
    return n == 1;

/*
    size_t i;
    size_t n;

    n = 0;
    i = MAX_CODEWORD_LENGTH - 1;
    do
    {
        n += bitLengthCount[i];
        if ((n & 1) != 0) // n is odd
        {
            return false;
        }
        n >>= 1;
        --i;
    }
    while (i != 0);

    // n != 1 checks for -> there is at least one symbol with non-zero codeword length
    // uBitLengthCount[0] -> number of symbols with codeword length 0 = 0.
    if (n != 1 || bitLengthCount[0] != 0)
    {
        printf("n: %d\n", n);
        return false;
    }

    return true;
*/
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

size_t fillDecodeTable(const std::shared_ptr<HuffmanNode> &sortedSymbols, std::vector<uint16_t> &decodeTable, size_t rootBits, size_t tailBits)
{
    size_t tableOffset = 0;
    size_t rootLevel = 0;
    std::shared_ptr<HuffmanNode> currentNode = sortedSymbols;

    // For each node in the sorted symbol list
    while (currentNode)
    {
        size_t symbolBits = currentNode->bits;
        size_t nodeLevel = (symbolBits <= rootBits) ? rootBits : symbolBits - tailBits;

        size_t tableSize = 1 << (symbolBits - rootLevel);
        for (size_t i = 0; i < tableSize; ++i)
        {
            uint16_t index = reverseBits(i, symbolBits - rootLevel);
            decodeTable[tableOffset + index] = currentNode->symbol << 4 | (symbolBits - rootLevel);
        }

        // Advance the pointer to the next node in the sorted list
        currentNode = currentNode->next;
        tableOffset += tableSize;
        rootLevel = nodeLevel;
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