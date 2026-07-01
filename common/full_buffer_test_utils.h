#ifndef FULL_BUFFER_TEST_UTILS_H
#define FULL_BUFFER_TEST_UTILS_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "acl/acl.h"

namespace full_buffer {
constexpr uint32_t kDeviceId = 0U;
constexpr int32_t kDevicePoisonByte = 0xA5;

inline std::array<size_t, 9U> TestSizes()
{
    return {1U, 3U, 63U, 64U, 257U, 4096U, 4097U, 1024U * 1024U, (1024U * 1024U) + 127U};
}

struct HostVerifyResult {
    size_t mismatchCount = 0U;
    size_t firstOffset = 0U;
    uint8_t expected = 0U;
    uint8_t actual = 0U;
};

inline uint8_t ExpectedByte(size_t offset)
{
    return static_cast<uint8_t>((((static_cast<uint64_t>(offset)) * 37ULL) +
        (((static_cast<uint64_t>(offset)) >> 3U) * 17ULL) + 0x5AULL +
        (((static_cast<uint64_t>(offset)) & 0xFFULL) ^
        ((static_cast<uint64_t>(offset)) >> 8U))) & 0xFFULL);
}

inline bool CheckAclError(aclError ret, const char *expr, const char *file, int line)
{
    if (ret == ACL_SUCCESS) {
        return true;
    }
    std::fprintf(stderr, "[ERROR] %s:%d: %s failed, ret=%d\n", file, line, expr, static_cast<int32_t>(ret));
    return false;
}

inline void FillHostPattern(uint8_t *buffer, size_t size)
{
    for (size_t offset = 0U; offset < size; ++offset) {
        buffer[offset] = ExpectedByte(offset);
    }
}

inline void FillHostInversePattern(uint8_t *buffer, size_t size)
{
    for (size_t offset = 0U; offset < size; ++offset) {
        buffer[offset] = static_cast<uint8_t>(ExpectedByte(offset) ^ 0xFFU);
    }
}

inline HostVerifyResult VerifyHostPattern(const uint8_t *buffer, size_t size)
{
    HostVerifyResult result;
    for (size_t offset = 0U; offset < size; ++offset) {
        const uint8_t expected = ExpectedByte(offset);
        const uint8_t actual = buffer[offset];
        if (actual != expected) {
            if (result.mismatchCount == 0U) {
                result.firstOffset = offset;
                result.expected = expected;
                result.actual = actual;
            }
            ++result.mismatchCount;
        }
    }
    return result;
}

inline bool ReportHostVerifyResult(const char *testName, size_t size, const HostVerifyResult &result)
{
    if (result.mismatchCount == 0U) {
        std::printf("[PASS] %s size=%zu all bytes match\n", testName, size);
        return true;
    }
    std::fprintf(stderr,
        "[FAIL] %s size=%zu mismatch_count=%zu first_offset=%zu expected=0x%02x actual=0x%02x\n",
        testName, size, result.mismatchCount, result.firstOffset, static_cast<uint32_t>(result.expected),
        static_cast<uint32_t>(result.actual));
    return false;
}

inline bool CleanupRuntime(aclrtStream stream)
{
    bool ok = true;
    if (stream != nullptr) {
        ok = CheckAclError(aclrtSynchronizeStream(stream), "aclrtSynchronizeStream(stream)", __FILE__, __LINE__) && ok;
        ok = CheckAclError(aclrtDestroyStream(stream), "aclrtDestroyStream(stream)", __FILE__, __LINE__) && ok;
    }
    ok = CheckAclError(aclrtResetDeviceForce(static_cast<int32_t>(kDeviceId)), "aclrtResetDeviceForce(kDeviceId)",
        __FILE__, __LINE__) && ok;
    ok = CheckAclError(aclFinalize(), "aclFinalize()", __FILE__, __LINE__) && ok;
    return ok;
}
} // namespace full_buffer

#define FB_CHECK_ACL(call) \
    do { \
        if (ok && !::full_buffer::CheckAclError((call), #call, __FILE__, __LINE__)) { \
            ok = false; \
        } \
    } while (0)

#define FB_CLEANUP_ACL(call) \
    do { \
        cleanupOk = ::full_buffer::CheckAclError((call), #call, __FILE__, __LINE__) && cleanupOk; \
    } while (0)

#endif // FULL_BUFFER_TEST_UTILS_H
