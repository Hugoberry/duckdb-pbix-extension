meta:
  id: abf_mt_xpress9
  endian: le

seq:
  - id: signature
    size: 102
    type: str
    encoding: UTF-16LE
    doc: This backup was created using multithreaded Xprs9.
  - id: multi_thread
    type: thread_distribution
  - id: chunks
    type: chunk
    repeat: eos
    
types:
  thread_distribution:
    seq:
    - id: tail_block_count 
      type: u8
    - id: head_block_count 
      type: u8
    - id: head_thread_count 
      type: u8
    - id: tail_thread_count 
      type: u8
    - id: chunk_uncompressed_size
      type: u8
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
      type: u4
    - id: zero
      type: u4
    - id: session_signature
      type: u4
    - id: block_index
      type: u4
    - id: crc32
      type: u4
   

