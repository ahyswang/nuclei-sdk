#include <riscv_vector.h>
#include "core_feature_base.h"
#include "core_feature_vector.h"

static void __enable_vector(void)
{
    __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_VS);
    __RV_CSR_SET(CSR_MSTATUS, MSTATUS_VS_INITIAL);
}

static void __disable_vector(void)
{
    __RV_CSR_CLEAR(CSR_MSTATUS, MSTATUS_VS);
}

#define q7_t int8_t

void riscv_add_q7(
  const q7_t * pSrcA,
  const q7_t * pSrcB,
        q7_t * pDst,
        uint32_t blockSize)
{
  uint32_t blkCnt;                               /* Loop counter */
  blkCnt = blockSize;                              /* Loop counter */
  size_t l;
  vint8m8_t vx, vy;

  for (; (l = __riscv_vsetvl_e8m8(blkCnt)) > 0; blkCnt -= l)
  {
    vx = __riscv_vle8_v_i8m8(pSrcA, l);
    pSrcA += l;
    vy = __riscv_vle8_v_i8m8(pSrcB, l);
    pSrcB += l;
    __riscv_vse8_v_i8m8(pDst, __riscv_vsadd_vv_i8m8(vy, vx, l), l);
    pDst += l;
  }
}

//make CORE=n307fd  ARCH_EXT=_zve32f_zba_zbb_zbc_zbs_xxldspn1x run_qemu
//D:\Software\Nuclei\gcc\lib\gcc\riscv64-unknown-elf\13.1.1\include\riscv_vector.h
//D:\Software\Nuclei\gcc\lib\clang\17\include\riscv_vector.h
void test_vector_ins()
{
    int8_t x[128] = {0,1,2,3};
    int8_t y[128] = {0,1,2,3};
    int8_t z[128] = {0};
    
    printf("%s:\n", __FUNCTION__);

    __enable_vector();

    printf("vlenb:%d\n", __riscv_vlenb());
    printf("vsetvl:%d\n", __riscv_vsetvl_e8m8(128));

    riscv_add_q7(x, y, z, 128);
    __disable_vector();

    printf("riscv_add_q7, %d %d %d %d\n", z[0], z[1], z[2], z[3]);
}
