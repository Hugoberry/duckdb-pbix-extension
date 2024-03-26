meta:
  id: abf_xpress9
  file-extension: idf
  endian: le

seq:
  - id: signature
    size: 102
    type: str
    encoding: UTF-16LE
    doc: This backup was created using Xpress9 compression.
  - id: chunks
    type: chunk
    repeat: eos
types:
  chunk:
    seq:
    - id: uncompressed
      type: u4
    - id: compressed
      type: u4
    - id: node
      type: node
      size: compressed

  node:
    seq:
    - id: header
      type: header
    - id: segments
      size: _parent.compressed - 32
      
  header:
   seq:
    - id: xpress_magic
      contents: [0x2a, 0xd7, 0x86, 0x4e]
      doc: 0x4e86d72a [0x2a, 0xd7, 0x86, 0x4e]
    - id: orig_size
      type: u4
    - id: encoded_size
      type: u4  
    - id: huffman_table_flags
      type: huffman_flags
    - id: zero
      type: u4
    - id: session_signature
      type: u4
    - id: block_index
      type: u4
    - id: crc32
      type: u4
   
  huffman_flags:
    seq:
    - id: huffman_table_length
      type: b13
      doc: Length of encoded Huffman tables in bits
    - id: log2_window_size
      type: b3
      doc: log2 of window size (16 .. 23)
    - id: mtf_entries
      type: b2
      enum: mtf_entries_enum
      doc: Number of MTF entries
    - id: min_ptr_match_length_flag
      type: b1
      doc: Min pointer match length flag (0 for length 3, 1 for length 4)
    - id: min_mtf_match_length_flag
      type: b1
      doc: Min MTF match length flag (0 for length 2, 1 for length 3)
    - id: reserved_zero
      type: b12
      doc: reserved, must be 0

enums:
  mtf_entries_enum:
    0b00: no_entries
    0b01: two_entries
    0b10: four_entries
    0b11: reserved
