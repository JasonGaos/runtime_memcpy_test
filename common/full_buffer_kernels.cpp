#include "kernel_operator.h"
#include "full_buffer_kernels.h"

extern "C" __global__ __aicore__ void DeviceFullBufferFill(GM_ADDR devAddr, uint64_t size)
{
    __gm__ uint8_t *devPtr = (__gm__ uint8_t *)devAddr;
    for (uint64_t offset = 0ULL; offset < size; ++offset) {
        devPtr[offset] = FULL_BUFFER_PATTERN_BYTE(offset);
    }
}

void FullBufferFillPattern(uint32_t blockDim, void *stream, void *devPtr, uint64_t size)
{
    DeviceFullBufferFill<<<blockDim, nullptr, stream>>>(devPtr, size);
}

extern "C" __global__ __aicore__ void DeviceFullBufferCheck(GM_ADDR devAddr, uint64_t size, GM_ADDR resultAddr)
{
    __gm__ uint8_t *devPtr = (__gm__ uint8_t *)devAddr;
    __gm__ uint64_t *result = (__gm__ uint64_t *)resultAddr;
    uint64_t mismatchCount = 0ULL;
    uint64_t firstOffset = 0ULL;
    uint64_t firstExpected = 0ULL;
    uint64_t firstActual = 0ULL;

    for (uint64_t offset = 0ULL; offset < size; ++offset) {
        const uint8_t expected = FULL_BUFFER_PATTERN_BYTE(offset);
        const uint8_t actual = devPtr[offset];
        if (actual != expected) {
            if (mismatchCount == 0ULL) {
                firstOffset = offset;
                firstExpected = static_cast<uint64_t>(expected);
                firstActual = static_cast<uint64_t>(actual);
            }
            ++mismatchCount;
        }
    }

    result[0] = mismatchCount;
    result[1] = firstOffset;
    result[2] = firstExpected;
    result[3] = firstActual;
}

void FullBufferCheckPattern(uint32_t blockDim, void *stream, void *devPtr, uint64_t size, void *result)
{
    DeviceFullBufferCheck<<<blockDim, nullptr, stream>>>(devPtr, size, result);
}
