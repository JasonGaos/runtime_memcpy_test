#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "full_buffer_test_utils.h"

namespace {
constexpr const char *kTestName = "h2d_async_full_buffer";

bool RunOneSize(size_t size, aclrtStream stream)
{
    bool ok = true;
    bool cleanupOk = true;
    uint8_t *hostSrc = nullptr;
    uint8_t *hostDst = nullptr;
    uint8_t *devPtr = nullptr;

    std::printf("[INFO] %s size=%zu start\n", kTestName, size);
    FB_CHECK_ACL(aclrtMallocHost(reinterpret_cast<void **>(&hostSrc), size));
    FB_CHECK_ACL(aclrtMallocHost(reinterpret_cast<void **>(&hostDst), size));
    FB_CHECK_ACL(aclrtMalloc(reinterpret_cast<void **>(&devPtr), size, ACL_MEM_MALLOC_HUGE_FIRST));
    FB_CHECK_ACL(aclrtMemset(devPtr, size, full_buffer::kDevicePoisonByte, size));

    if (ok) {
        full_buffer::FillHostPattern(hostSrc, size);
        full_buffer::FillHostInversePattern(hostDst, size);
    }
    FB_CHECK_ACL(aclrtMemcpyAsync(devPtr, size, hostSrc, size, ACL_MEMCPY_HOST_TO_DEVICE, stream));
    FB_CHECK_ACL(aclrtSynchronizeStream(stream));
    FB_CHECK_ACL(aclrtMemcpy(hostDst, size, devPtr, size, ACL_MEMCPY_DEVICE_TO_HOST));

    if (ok) {
        ok = full_buffer::ReportHostVerifyResult(kTestName, size,
            full_buffer::VerifyHostPattern(hostDst, size));
    }

    if (devPtr != nullptr) {
        FB_CLEANUP_ACL(aclrtFree(devPtr));
    }
    if (hostDst != nullptr) {
        FB_CLEANUP_ACL(aclrtFreeHost(hostDst));
    }
    if (hostSrc != nullptr) {
        FB_CLEANUP_ACL(aclrtFreeHost(hostSrc));
    }
    return ok && cleanupOk;
}
} // namespace

int32_t main()
{
    bool ok = true;
    aclrtStream stream = nullptr;

    FB_CHECK_ACL(aclInit(nullptr));
    FB_CHECK_ACL(aclrtSetDevice(static_cast<int32_t>(full_buffer::kDeviceId)));
    FB_CHECK_ACL(aclrtCreateStream(&stream));

    if (ok) {
        bool allSizesOk = true;
        for (const size_t size : full_buffer::TestSizes()) {
            if (!RunOneSize(size, stream)) {
                allSizesOk = false;
            }
        }
        ok = allSizesOk;
    }

    ok = full_buffer::CleanupRuntime(stream) && ok;
    if (ok) {
        std::printf("[SUCCESS] %s passed all full-buffer byte checks\n", kTestName);
        return 0;
    }
    std::fprintf(stderr, "[FAILURE] %s failed\n", kTestName);
    return 1;
}
