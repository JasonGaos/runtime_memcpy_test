#include <cstddef>
#include <cstdint>
#include <cstdio>

#include "full_buffer_test_utils.h"

namespace {
constexpr const char *kTestName = "d2h_sync_full_buffer";

bool RunOneSize(size_t size, aclrtStream stream)
{
    bool ok = true;
    bool cleanupOk = true;
    uint8_t *hostPtr = nullptr;
    uint8_t *devPtr = nullptr;

    std::printf("[INFO] %s size=%zu start\n", kTestName, size);
    FB_CHECK_ACL(aclrtMallocHost(reinterpret_cast<void **>(&hostPtr), size));
    FB_CHECK_ACL(aclrtMalloc(reinterpret_cast<void **>(&devPtr), size, ACL_MEM_MALLOC_HUGE_FIRST));

    if (ok) {
        FullBufferFillPattern(full_buffer::kKernelBlockDim, stream, devPtr, static_cast<uint64_t>(size));
    }
    FB_CHECK_ACL(aclrtSynchronizeStream(stream));
    FB_CHECK_ACL(aclrtMemcpy(hostPtr, size, devPtr, size, ACL_MEMCPY_DEVICE_TO_HOST));

    if (ok) {
        ok = full_buffer::ReportHostVerifyResult(kTestName, size,
            full_buffer::VerifyHostPattern(hostPtr, size));
    }

    if (devPtr != nullptr) {
        FB_CLEANUP_ACL(aclrtFree(devPtr));
    }
    if (hostPtr != nullptr) {
        FB_CLEANUP_ACL(aclrtFreeHost(hostPtr));
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
