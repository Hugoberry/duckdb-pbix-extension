#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <algorithm>
#include <fstream>
#include <string>
#include <queue>
#include <iomanip>

// Huffman Tree Node definition
struct HuffmanTree {
    uint8_t c;
    HuffmanTree* left;
    HuffmanTree* right;

    HuffmanTree(uint8_t c = 0) : c(c), left(nullptr), right(nullptr) {}
    ~HuffmanTree() {
        delete left;
        delete right;
    }
};


std::vector<uint8_t> decompress_encode_array(const std::vector<uint8_t>& compressed);
HuffmanTree* build_huffman_tree(const std::vector<uint8_t>& encode_array);
std::string decode_substring(const std::string& bitstream, HuffmanTree* tree, uint32_t start_bit, uint32_t end_bit);