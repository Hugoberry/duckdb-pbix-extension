#include "huffman_decoder.h"



// Function to generate the full 256-byte Huffman array from the compact 128-byte encode_array
std::vector<uint8_t> decompress_encode_array(const std::vector<uint8_t>& compressed) {
    std::vector<uint8_t> full_array(256, 0);

    for (size_t i = 0; i < compressed.size(); i++) {
        uint8_t byte = compressed[i];
        full_array[2 * i] = byte & 0x0F;         // Lower nibble
        full_array[2 * i + 1] = (byte >> 4) & 0x0F; // Upper nibble
    }

    return full_array;
}

// Function to generate Huffman codes based on codeword lengths
std::unordered_map<uint8_t, std::string> generate_codes(const std::vector<uint8_t>& lengths) {
    std::unordered_map<uint8_t, std::string> codes;
    std::vector<std::pair<int, uint8_t>> sorted_lengths;

    // Collect only the non-zero lengths and their associated symbols
    for (auto i = 0; i < 256; i++) {
        if (lengths[i] != 0){
            sorted_lengths.emplace_back(lengths[i], i);
        }
    }
    // Sort by length first, then by character
    std::sort(sorted_lengths.begin(), sorted_lengths.end(), [](const auto& a, const auto& b) {
        return a.first != b.first ? a.first < b.first : a.second < b.second;
    });

    int code = 0;
    int last_length = 0;

    for (const auto& [length, character] : sorted_lengths) {
        if (last_length != length) {
            code <<= (length - last_length); // Shift code by difference in lengths
            last_length = length;
        }

        // Generate the code string representation up to 15 bits
        codes[character] = std::bitset<15>(code).to_string().substr(15 - length);
        code++;
    }

    return codes;
}

// Print Huffman codes
void print_huffman_codes(const std::unordered_map<uint8_t, std::string>& codes) {
    std::cout << "Huffman Codes:\n";
    for (const auto& [character, code] : codes) {
        std::cout << (char)character << ": " << code << '\n';
    }
}

// Build Huffman tree based on generated codes
HuffmanTree* build_huffman_tree(const std::vector<uint8_t>& encode_array) {
    auto codes = generate_codes(encode_array);
// print_huffman_codes(codes);
    HuffmanTree* root = new HuffmanTree;

    for (const auto& [character, code] : codes) {
        HuffmanTree* node = root;
        for (char bit : code) {
            if (bit == '0') {
                if (!node->left) node->left = new HuffmanTree;
                node = node->left;
            } else {
                if (!node->right) node->right = new HuffmanTree;
                node = node->right;
            }
        }
        node->c = character;
    }

    return root;
}
// Decode a bitstream from start to end bit positions using the Huffman tree
std::string decode_substring(const std::string& bitstream, HuffmanTree* tree, uint32_t start_bit, uint32_t end_bit) {
    std::string result;
    const HuffmanTree* node = tree;
    uint32_t total_bits = end_bit - start_bit;

    // Adjust bit position calculation for little endian byte order
    for (uint32_t i = 0; i < total_bits; ++i) {
        uint32_t bit_pos = start_bit + i;
        uint32_t byte_pos = bit_pos / 8;
        uint32_t bit_offset = bit_pos % 8;

        // Convert byte index for little endian (pair-wise)
        byte_pos = (byte_pos & ~0x01) + (1 - (byte_pos & 0x01));

        if (!node->left && !node->right) {
            result += (char)(node->c);
            node = tree; // Reset to the root node
        }

        // Traverse the Huffman tree based on the current bit
        if (bitstream[byte_pos] & (1 << (7 - bit_offset))) {  // Adjusting bit offset to read from MSB to LSB
            node = node->right;
        } else {
            node = node->left;
        }
    }

    // Append the last character if the final node is a leaf
    if (!node->left && !node->right) {
        result += (char)(node->c);
    }

    return result;
}


// Print Huffman tree in a readable format
void print_huffman_tree(HuffmanTree* node, int indent = 0) {
    if (node == nullptr) return;

    if (node->right) print_huffman_tree(node->right, indent + 4);

    if (indent) std::cout << std::setw(indent) << ' ';
    if (!node->left && !node->right) std::cout << node->c << '\n';
    else std::cout << "⟨\n";

    if (node->left) print_huffman_tree(node->left, indent + 4);
}