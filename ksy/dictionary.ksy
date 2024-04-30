meta:
  id: column_data_dictionary
  file-extension: dictionary
  endian: le

seq:
  - id: dictionary_type
    type: s4
    enum: dictionary_types
  - id: hash_information
    type: hash_info
  - id: data
    type:
      switch-on: dictionary_type
      cases:
        'dictionary_types::xm_type_string': string_data
        'dictionary_types::xm_type_long': number_data
        'dictionary_types::xm_type_real': number_data

types:
  hash_info:
    seq:
      - id: hash_elements
        type: s4
        repeat: expr
        repeat-expr: 6

  string_data:
    seq:
      - id: page_layout_information
        type: page_layout
      - id: dictionary_pages
        type: dictionary_page
      - id: dictionary_record_handles_vector_info
        type: dictionary_record_handles_vector

  page_layout:
    seq:
      - id: store_string_count
        type: s8
      - id: f_store_compressed
        type: s1
      - id: store_longest_string
        type: s8
      - id: store_page_count
        type: s8

  dictionary_page:
    seq:
      - id: page_mask
        type: u8
      - id: page_contains_nulls
        type: u1
      - id: page_start_index
        type: u8
      - id: page_string_count
        type: u8
      - id: page_compressed
        type: u1
      - id: string_store_begin_mark
        contents: [0xDD, 0xCC, 0xBB, 0xAA]
      - id: string_store
        type:
          switch-on: page_compressed
          cases:
            0: uncompressed_strings
            1: compressed_strings
      - id: string_store_end_mark
        contents: [0xCD,0xAB,0xCD,0xAB]
        

  uncompressed_strings:
    seq:
      - id: remaining_store_available
        type: u8
      - id: buffer_used_characters
        type: u8
      - id: allocation_size
        type: u8
      - id: uncompressed_character_buffer
        type: str
        size: allocation_size
        encoding: UTF-16LE

  compressed_strings:
    seq:
      - id: store_total_bits
        type: u4
      - id: character_set_type_identifier
        type: u4
      - id: allocation_size
        type: u8
      - id: character_set_used
        type: u1
      - id: ui_decode_bits
        type: u4
      - id: encode_array
        size: 128
      - id: ui64_buffer_size
        type: u8
      - id: compressed_string_buffer
        size: allocation_size

  dictionary_record_handles_vector:
    seq:
      - id: element_count
        type: u8
      - id: element_size
        type: u4
      - id: vector_of_record_handle_structures 
        type:
          switch-on: element_size
          cases:
            4: u4
            8: u8
        repeat: expr
        repeat-expr: element_count

  number_data:
    seq:
      - id: vector_of_vectors_info
        type: vector_of_vectors

  vector_of_vectors:
    seq:
      - id: element_count
        type: u8
      - id: element_size
        type: u4
      - id: values
        type:
          switch-on: data_type_id
          cases:
            '"int32"': s4
            '"int64"': s8
            '"float64"': f8
        repeat: expr
        repeat-expr: element_count
    instances:
      is_int32:
        value: element_size == 4
      is_int64:
        value: element_size == 8 and _root.dictionary_type == dictionary_types::xm_type_long
      is_float64:
        value: element_size == 8 and _root.dictionary_type == dictionary_types::xm_type_real
      data_type_id:
        value: |
          is_int32 ? "int32" :
          is_int64 ? "int64" :
          "float64"

enums:
  dictionary_types:
    -1: xm_type_invalid
    0: xm_type_long
    1: xm_type_real
    2: xm_type_string
