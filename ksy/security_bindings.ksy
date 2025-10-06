meta:
  id: security_bindings
  endian: le

seq:
  - id: version
    type: u4

  # "id + version"
  - id: id_and_ver
    type: guid_with_u32

  - id: provider_guid
    type: guid_le

  - id: reserved0
    type: u4

  - id: total_or_hdr_size
    type: u4

  - id: ver_major_or_flags
    type: u2
  - id: ver_minor
    type: u1
  - id: ver_patch
    type: u1

  - id: flags
    type: u4

  - id: small_flags
    type: u2

  - id: blob1_len
    type: u4
  - id: blob1
    size: blob1_len

  # Hash params?
  - id: hash_params
    type: hash_params_t

  - id: blob2_len
    type: u4
  - id: blob2
    size: blob2_len

  - id: blob3_len
    type: u4
  - id: blob3
    size: blob3_len

  - id: hash_len
    type: u4
  - id: hash_sha1
    size: hash_len

types:
  guid_with_u32:
    seq:
      - id: guid
        type: guid_le
      - id: version_u32
        type: u4

  hash_params_t:
    seq:
      - id: reserved
        type: u4
      - id: alg_id
        type: u4     # 0x8004 == CALG_SHA1
      - id: hash_bits
        type: u4     # 160 for SHA-1

  guid_le:
    seq:
      - id: time_low
        type: u4
      - id: time_mid
        type: u2
      - id: time_hi_and_version
        type: u2
      - id: clock_seq_hi_and_reserved
        type: u1
      - id: clock_seq_low
        type: u1
      - id: node
        size: 6
