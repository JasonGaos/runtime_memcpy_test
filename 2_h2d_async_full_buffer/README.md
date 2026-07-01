# 2_h2d_async_full_buffer

异步 Host-to-Device 全 buffer 逐字节校验测试。

本测试会为每个测试尺寸生成确定性 Host 字节模式，通过 `aclrtMemcpyAsync` 在 Stream 上拷贝到 Device，调用 `aclrtSynchronizeStream` 等待完成后，把 Device buffer 读回另一个 Host buffer，并在 Host 侧逐字节校验完整内容。

## 运行

```bash
source ${install_root}/cann/set_env.sh
bash run.sh
```

## 覆盖尺寸

`1`、`3`、`63`、`64`、`257`、`4096`、`4097`、`1048576`、`1048703` 字节。
