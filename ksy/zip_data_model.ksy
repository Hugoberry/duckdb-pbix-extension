meta:
  id: zip_end_of_file
  endian: le
  license: CC0-1.0

instances:
  end_of_central_dir:
    type: end_of_central_dir
    pos: _root._io.size - 22  #assume no comments
  
  central_dir:
    pos: end_of_central_dir.ofs_central_dir
    type: central_dir_entry
    repeat: until
    repeat-until: _.file_name== "DataModel"
#     repeat: expr
#     repeat-expr: end_of_central_dir.num_central_dir_entries_total
  
types:
  end_of_central_dir:
    seq:
      - id: eocd_magic
        size: 4
        contents: [P, K, 5, 6]
      - id: disk_of_end_of_central_dir
        type: u2
      - id: disk_of_central_dir
        type: u2
      - id: num_central_dir_entries_on_disk
        type: u2
      - id: num_central_dir_entries_total
        type: u2
      - id: len_central_dir
        type: u4
      - id: ofs_central_dir
        type: u4
      - id: len_comment
        type: u2

  central_dir_entry:
    seq:
      - id: cd_magic
        size: 4
        contents: [P, K, 1, 2]
      - id: version_made_by
        type: u2
      - id: version_needed_to_extract
        type: u2
      - id: flags
        type: u2
      - id: compression_method
        type: u2
      - id: file_mod_time
        size: 4
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
        type: str
        size: len_comment
        encoding: UTF-8
    instances:
      local_header:
        pos: ofs_local_header
        type: local_file_header
        
  local_file_header:
    seq:
      - id: lfh_magic
        size: 4
        contents: [P, K, 3, 4]
      - id: version
        type: u2
      - id: flags
        size: 2
      - id: compression_method
        type: u2
      - id: file_mod_time
        size: 4
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
      - id: file_name
        type: str
        size: len_file_name
        encoding: UTF-8
      - id: extra
        size: len_extra