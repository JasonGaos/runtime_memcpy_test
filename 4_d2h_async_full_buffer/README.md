# 4_d2h_async_full_buffer

异步 Device-to-Host 全 buffer 逐字节校验测试。

本测试先在同一 Stream 上启动 Device 侧填充 kernel，再通过 `aclrtMemcpyAsync` 拷贝回 Host。`aclrtSynchronizeStream` 返回后在 Host 侧逐字节校验完整 buffer。

## 运行

```bash
source ${install_root}/cann/set_env.sh
export SOC_VERSION=${soc_version}
export ASCENDC_CMAKE_DIR=${install_root}/cann/${arch}-linux/tikcpp/ascendc_kernel_cmake
bash run.sh
```

## 覆盖尺寸

`1`、`3`、`63`、`64`、`257`、`4096`、`4097`、`1048576`、`1048703` 字节。
