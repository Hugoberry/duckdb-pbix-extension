#include <vector>
#include <string>
#include <algorithm>

#include <vector>
#include <queue>

struct HuffmanNode {
  uint8_t symbol;
  int frequency;
  HuffmanNode* left;
  HuffmanNode* right;

  HuffmanNode(uint8_t symbol, int frequency) : symbol(symbol), frequency(frequency), left(nullptr), right(nullptr) {}

  bool operator<(const HuffmanNode& other) const {
    return frequency > other.frequency;
  }
};


std::string DecodeBitStream(const std::vector<uint8_t> &huffmanTree, const std::string &bitStream, uint8_t characterSet);
std::vector<uint8_t> ReconstructHuffmanTree(const std::vector<uint8_t>& encodedArray, uint32_t uiDecodeBits);