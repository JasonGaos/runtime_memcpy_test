# 1_h2d_sync_full_buffer

同步 Host-to-Device 全 buffer 逐字节校验测试。

本测试会为每个测试尺寸生成确定性 Host 字节模式，通过 `aclrtMemcpy` 拷贝到 Device，再启动 Device 侧校验 kernel 逐字节检查 Device buffer 内容。

## 运行

```bash
source ${install_root}/cann/set_env.sh
export SOC_VERSION=${soc_version}
export ASCENDC_CMAKE_DIR=${install_root}/cann/${arch}-linux/tikcpp/ascendc_kernel_cmake
bash run.sh
```

## 覆盖尺寸

`1`、`3`、`63`、`64`、`257`、`4096`、`4097`、`1048576`、`1048703` 字节。
