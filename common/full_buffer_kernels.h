#ifndef FULL_BUFFER_KERNELS_H
#define FULL_BUFFER_KERNELS_H

#include <cstdint>

#define FULL_BUFFER_PATTERN_BYTE(offset) \
    (static_cast<uint8_t>((((static_cast<uint64_t>(offset)) * 37ULL) + \
        (((static_cast<uint64_t>(offset)) >> 3U) * 17ULL) + 0x5AULL + \
        (((static_cast<uint64_t>(offset)) & 0xFFULL) ^ ((static_cast<uint64_t>(offset)) >> 8U))) & 0xFFULL))

namespace full_buffer {
inline uint8_t ExpectedPatternByte(uint64_t offset)
{
    return FULL_BUFFER_PATTERN_BYTE(offset);
}
} // namespace full_buffer

void FullBufferFillPattern(uint32_t blockDim, void *stream, void *devPtr, uint64_t size);
void FullBufferCheckPattern(uint32_t blockDim, void *stream, void *devPtr, uint64_t size, void *result);

#endif // FULL_BUFFER_KERNELS_H
