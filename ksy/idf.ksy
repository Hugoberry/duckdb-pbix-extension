meta:
  id: column_data_idf
  file-extension: idf
  endian: le
  license: CC0-1.0
  title: Column Data Storage Format

seq:
  - id: segments
    type: segment
    repeat: eos

types:
  segment:
    seq:
      - id: primary_segment_size
        type: u8
      - id: primary_segment
        type: segment_entry
        repeat: expr
        repeat-expr: primary_segment_size
      - id: sub_segment_size
        type: u8
      - id: sub_segment
        type: u8
        repeat: expr
        repeat-expr: sub_segment_size


  segment_entry:
    seq:
      - id: data_value
        type: u4
      - id: repeat_value
        type: u4