# TEST_RISCV

## 步骤

编译运行P指令
```
make CORE=n307fd  ARCH_EXT=xxldsp run_qemu
```

编译运行V指令
```
make CORE=n307fd  ARCH_EXT=_zve32f_zba_zbb_zbc_zbs_xxldspn1x run_qemu
```