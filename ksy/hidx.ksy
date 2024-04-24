meta:
  id: column_data_hidx
  file-extension: hidx
  endian: le

seq:
# Required Elements for All Files That Use Hashing
  - id: hash_algorithm
    type: s4
  - id: hash_entry_size
    type: u4
  - id: hash_bin_size
    type: u4
  - id: local_entry_count
    type: u4
  - id: c_bins
    type: u8

# Records and Hash Statistics
  - id: number_of_records
    type: s8
  - id: current_mask
    type: s8
  - id: hash_stats
    type: u1
  - id: hash_statistics
    type: hash_statistics_type
    if: hash_stats != 0

# Hash Bin Entries
  - id: hash_bin_entries
    type: hash_bin
    repeat: expr
    repeat-expr: c_bins
    size: hash_bin_size
    
# Overflow Hash Entries
  - id: overflow_hash_entries_count
    type: u8
  - id: overflow_hash_entries
    type: hash_entry
    repeat: expr
    repeat-expr: overflow_hash_entries_count

types:
  hash_statistics_type:
    seq:
      - id: number_of_elements
        type: u8
      - id: number_of_bins
        type: u8
      - id: number_of_used_bins
        type: u8
      - id: fast_access_elements
        type: u8
      - id: locals_size_per_bin
        type: u8
      - id: maximum_chain
        type: u8
      - id: element_count
        type: u8
      - id: element_size
        type: u4
      - id: histogram_vector
        type: 
          switch-on: element_size
          cases:
            4: u4
            8: u8
        repeat: expr
        repeat-expr: element_count
        
  hash_bin:
    seq:
      - id: m_rg_chain
        type: u8
      - id: m_count
        type: u4
      - id: m_rg_local_entries
        type: hash_entry
        repeat: expr
        repeat-expr: _root.local_entry_count
      - id: padding
        type: u4

  hash_entry:
    seq:
      - id: m_hash
        type: u4
      - id: m_key
        type: u4

