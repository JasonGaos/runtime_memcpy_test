# Full Buffer Memory Copy Tests

本目录是 `runtime/example/1_basic_features/memory` 中四个 Host/Device 拷贝样例的增强版测试集合，独立放在 `runtime/` 同级目录，不修改原仓源码。

## 覆盖内容

- `1_h2d_sync_full_buffer`：同步 Host-to-Device，覆盖 `aclrtMemcpy(..., ACL_MEMCPY_HOST_TO_DEVICE)`。
- `2_h2d_async_full_buffer`：异步 Host-to-Device，覆盖 `aclrtMemcpyAsync(..., ACL_MEMCPY_HOST_TO_DEVICE, stream)`。
- `3_d2h_sync_full_buffer`：同步 Device-to-Host，覆盖 `aclrtMemcpy(..., ACL_MEMCPY_DEVICE_TO_HOST)`。
- `4_d2h_async_full_buffer`：异步 Device-to-Host，覆盖 `aclrtMemcpyAsync(..., ACL_MEMCPY_DEVICE_TO_HOST, stream)`。

所有测试都会覆盖以下字节数：

```text
1, 3, 63, 64, 257, 4096, 4097, 1048576, 1048703
```

其中包含奇数字节、64 字节边界、页大小边界、1MB 大小和非整齐大尺寸。

## 相比原版样例的增强

原版样例主要用于演示 API 调用流程：申请 Host/Device 内存、写入一个 `int` 值、执行拷贝，再比较日志中的 `Source data` 和 `Destination data`。

本目录的测试做了以下增强：

- 全 buffer 校验：不只检查第一个 `int`，而是逐字节检查每一个 byte。
- 确定性数据模式：Host 侧生成固定 byte pattern，便于定位任何偏移处的错误。
- 奇数字节覆盖：增加非偶数、非对齐大小，避免只验证整齐长度。
- 边界覆盖：同时覆盖小尺寸、64 字节边界、页边界、1MB 和非整齐大 buffer。
- 毒化初值：Device buffer 先用 `aclrtMemset` 写入固定毒化字节，Host 目的 buffer 先写入期望 pattern 的按位取反，避免未写入区域偶然通过。
- 更严格同步：异步测试把待测拷贝提交到同一个 stream，并显式 `aclrtSynchronizeStream` 后再校验或清理。
- 失败诊断更直接：出现 mismatch 时输出 mismatch 总数、首个错误 offset、期望字节和实际字节。
- API 返回值检查更完整：初始化、设备设置、stream、malloc/free、memcpy、sync 都检查返回值。
- 无自定义 AscendC kernel：测试只依赖 ACL runtime 和 Host 侧逐字节校验，避免校验 kernel 自身影响结果。

## 校验策略

H2D 测试流程：

1. Host buffer 填充确定性 byte pattern。
2. 通过同步或异步接口拷贝到 Device。
3. 将 Device buffer 读回到另一个 Host buffer。
4. Host 侧逐字节校验读回内容。
5. 若 mismatch 数量非 0，测试失败并打印首个错误位置。

D2H 测试流程：

1. Host buffer 填充确定性 byte pattern。
2. 通过 H2D 同步拷贝预置 Device buffer 内容。
3. 通过同步或异步 D2H 接口拷贝到另一个 Host buffer。
4. Host 侧逐字节检查完整 buffer。
5. 若 mismatch 数量非 0，测试失败并打印首个错误位置。

说明：H2D 测试需要一次 D2H 读回作为观测手段，D2H 测试需要一次 H2D 预置作为数据准备。四个测试组合运行时，可以区分 H2D 和 D2H 路径问题；单个 D2H 测试失败时，也应结合 H2D 测试结果判断是否是预置阶段受影响。

## 运行方式

在已安装 CANN 且可访问 Ascend 设备的环境中执行：

```bash
source ${install_root}/cann/set_env.sh
bash runtime_memcpy_test/run_all.sh
```

也可以单独运行某一个测试：

```bash
bash runtime_memcpy_test/1_h2d_sync_full_buffer/run.sh
bash runtime_memcpy_test/2_h2d_async_full_buffer/run.sh
bash runtime_memcpy_test/3_d2h_sync_full_buffer/run.sh
bash runtime_memcpy_test/4_d2h_async_full_buffer/run.sh
```

## 成功与失败输出

成功时每个尺寸会输出：

```text
[PASS] h2d_sync_full_buffer size=4096 all bytes match
```

失败时会输出类似：

```text
[FAIL] h2d_sync_full_buffer size=4097 mismatch_count=1 first_offset=128 expected=0x3a actual=0xff
```

该信息表示完整 buffer 中发现错误，并给出首个错误字节的位置与内容。
