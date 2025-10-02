#pragma once

#include <stdint.h> // For uint32_t, uint8_t types
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Declare the function that computes the CRC32
uint32_t Crc32(const uint8_t *pData, size_t uSize, uint32_t uCrc);

#ifdef __cplusplus
}
#endif

