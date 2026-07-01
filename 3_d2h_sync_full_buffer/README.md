# 3_d2h_sync_full_buffer

同步 Device-to-Host 全 buffer 逐字节校验测试。

本测试先在 Host 侧生成确定性字节模式，通过 H2D 同步拷贝预置 Device buffer，再使用 `aclrtMemcpy` 执行待测 D2H 拷贝，并在 Host 侧逐字节校验完整 buffer。

## 运行

```bash
source ${install_root}/cann/set_env.sh
bash run.sh
```

## 覆盖尺寸

`1`、`3`、`63`、`64`、`257`、`4096`、`4097`、`1048576`、`1048703` 字节。
