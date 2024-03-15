// inline void DecompressAndProcessPbix(DataChunk &args, ExpressionState &state, Vector &result) {
//     auto &name_vector = args.data[0];

//     ifstream entryStream(name_vector, std::ios::binary);
//     entryStream.seekg(102, std::ios::beg);
//     XPress9Wrapper xpress9Wrapper;
//     xpress9Wrapper.Initialize();
//     vector<uint8_t> allDecompressedData;
//     entryStream.seekg(0, std::ios::end);
//     auto totalSize = entryStream.tellg();
//     entryStream.seekg(102, std::ios::beg);
//     while (entryStream.tellg() < totalSize) {
//         uint32_t uncompressedSize;
//         uint32_t compressedSize;

//         entryStream.read(reinterpret_cast<char*>(&uncompressedSize), sizeof(uint32_t));
//         entryStream.read(reinterpret_cast<char*>(&compressedSize), sizeof(uint32_t));

//         vector<uint8_t> compressedBuffer(compressedSize);
//         vector<uint8_t> decompressedBuffer(uncompressedSize);

//         entryStream.read(reinterpret_cast<char*>(compressedBuffer.data()), compressedSize);
//         uint32_t totalDecompressedSize = xpress9Wrapper.Decompress(compressedBuffer.data(), compressedSize, decompressedBuffer.data(), decompressedBuffer.size());

//         if (totalDecompressedSize != uncompressedSize) {
//             return "Decompressed size does not match expected size.";
//         }

//         allDecompressedData.insert(allDecompressedData.end(), decompressedBuffer.begin(), decompressedBuffer.end());
//     }
//     UnaryExecutor::Execute<string_t, string_t>(
//         name_vector, result, args.size(),
//         [&](string_t name) {
//             std::string pbix_file = name.GetString();
//             std::vector<uint8_t> pbix_buffer = AbfParser::read_file(pbix_file);
//             std::vector<uint8_t> decompressed_buffer = Xpress9Wrapper::decompress(pbix_buffer);
//             AbfParser::process_data(decompressed_buffer);
//             return StringVector::AddString(result, "Pbix "+name.GetString()+" 🐥");;
//         });
// }
