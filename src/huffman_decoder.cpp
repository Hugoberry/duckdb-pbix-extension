#include "huffman_decoder.h"
#include <iostream>

std::vector<uint8_t> ReconstructHuffmanTree(const std::vector<uint8_t>& encodedArray, uint32_t uiDecodeBits) {
    // 1. Build frequency table from encodedArray (assuming 256 symbols)
    std::vector<int> frequencyTable(256, 0);
    for (int i = 0; i < 256; ++i) {
        int codewordLength = encodedArray[i / 2] >> (4 * (i % 2)) & 0xF;
        if (codewordLength > 0) {
            frequencyTable[i] = 1 << (16 - codewordLength); // Estimate frequency
            printf("Frequency: %d\n", frequencyTable[i]);
        }
    }

    // 2. Create leaf nodes and add them to the priority queue
    std::priority_queue<HuffmanNode> nodeQueue;
    for (int i = 0; i < 256; ++i) {
        if (frequencyTable[i] > 0) {
            nodeQueue.push(HuffmanNode(i, frequencyTable[i]));
        }
    }

    // 3. Build Huffman tree
    HuffmanNode* root = nullptr;
    while (nodeQueue.size() > 1) {
        HuffmanNode* left = new HuffmanNode(nodeQueue.top());
        nodeQueue.pop();
        HuffmanNode* right = new HuffmanNode(nodeQueue.top());
        nodeQueue.pop();

        HuffmanNode* parent = new HuffmanNode(0, left->frequency + right->frequency); // Symbol is irrelevant for internal nodes
        parent->left = left;
        parent->right = right;
        nodeQueue.push(*parent);
    }

    // Assign the remaining single node in the queue to root
    if (!nodeQueue.empty()) {
        root = new HuffmanNode(nodeQueue.top());
        nodeQueue.pop();
    }

    // 4. Traverse the tree to generate codeword lengths
    std::vector<uint8_t> codewordLengths(256, 0);
    std::function<void(HuffmanNode*, int)> traverse = [&](HuffmanNode* node, int depth) {
        if (!node) return;
        if (!node->left && !node->right) {
            codewordLengths[node->symbol] = depth;
        } else {
            traverse(node->left, depth + 1);
            traverse(node->right, depth + 1);
        }
    };

    traverse(root, 0);

    // 5. Create the new encoded array based on codewordLengths and uiDecodeBits
    std::vector<uint8_t> newEncodedArray(128, 0);
    for (int i = 0; i < 256; ++i) {
        int codewordLength = codewordLengths[i];
        if (codewordLength > 0 && codewordLength <= uiDecodeBits) {
            newEncodedArray[i / 2] |= codewordLength << (4 * (i % 2));
        }
    }

    // 6. Deallocate the Huffman tree (implementation omitted for brevity)

    return newEncodedArray;
}

std::string DecodeBitStream(const std::vector<uint8_t> &huffmanTree, const std::string &bitStream, uint8_t characterSet)
{
    std::string decodedString;
    uint32_t currentTreeIndex = 0;

    for (char bit : bitStream)
    {
        std::cout << static_cast<int>(bit);
        // Traverse the Huffman tree based on the bit value
        currentTreeIndex = bit == '0' ? huffmanTree[currentTreeIndex * 2] : huffmanTree[currentTreeIndex * 2 + 1];

        // If reached a leaf node, decode the character
        if (currentTreeIndex >= 256)
        {
            uint8_t decodedByte = currentTreeIndex - 256;
            // Add the character set byte if single character set mode
            if (characterSet != 0)
            {
                decodedByte |= characterSet << 8;
            }
            decodedString += static_cast<char>(decodedByte);
            currentTreeIndex = 0; // Reset to root for next character
        }
    }
    std::cout << std::endl;

    return decodedString;
}