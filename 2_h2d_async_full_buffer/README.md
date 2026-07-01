# 2_h2d_async_full_buffer

异步 Host-to-Device 全 buffer 逐字节校验测试。

本测试会为每个测试尺寸生成确定性 Host 字节模式，通过 `aclrtMemcpyAsync` 在同一 Stream 上拷贝到 Device，并在同一 Stream 上启动 Device 侧校验 kernel。`aclrtSynchronizeStream` 返回后读取紧凑校验结果。

## 运行

```bash
source ${install_root}/cann/set_env.sh
export SOC_VERSION=${soc_version}
export ASCENDC_CMAKE_DIR=${install_root}/cann/${arch}-linux/tikcpp/ascendc_kernel_cmake
bash run.sh
```

## 覆盖尺寸

`1`、`3`、`63`、`64`、`257`、`4096`、`4097`、`1048576`、`1048703` 字节。
