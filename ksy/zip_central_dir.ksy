meta:
  id: pbix
  title: PBIX archive file
  file-extension: pbix
  endian: le
  bit-endian: le
seq:
  - id: sections
    type: pk_section
    repeat: eos
types:
  pk_section:
    seq:
      - id: magic
        contents: 'PK'
      - id: section_type
        type: u2
      - id: body
        type:
          switch-on: section_type
          cases:
            0x0201: central_dir_entry
            0x0403: local_file
            0x0605: end_of_central_dir
            0x0807: data_descriptor
  data_descriptor:
    seq:
      - id: data_descriptor_obs
        size: 12
  local_file:
    seq:
      - id: header
        type: local_file_header
      - id: body
        size: header.len_body_compressed
  local_file_header:
    seq:
      - id: header_trimmed
        size: 14
      - id: len_body_compressed
        type: u4
      - id: len_body_uncompressed
        type: u4
      - id: len_file_name
        type: u2
      - id: len_extra
        type: u2
      - id: file_name
        size: len_file_name
      - id: extra
        size: len_extra
  central_dir_entry:
    seq:
      - id: header_obs
        size: 12
      - id: crc32
        type: u4
      - id: len_body_compressed
        type: u4
      - id: len_body_uncompressed
        type: u4
      - id: len_file_name
        type: u2
      - id: len_extra
        type: u2
      - id: len_comment
        type: u2
      - id: disk_number_start
        type: u2
      - id: int_file_attr
        type: u2
      - id: ext_file_attr
        type: u4
      - id: ofs_local_header
        type: s4
      - id: file_name
        type: str
        size: len_file_name
        encoding: UTF-8
      - id: extra
        size: len_extra
      - id: comment
        size: len_comment
  end_of_central_dir:
    seq:
      - id: header_obs
        size: 16
      - id: len_comment
        type: u2
      - id: comment
        size: len_comment
