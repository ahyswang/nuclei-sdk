#include "ctest.h"
#include "nuclei_sdk_soc.h"

static void print_vecotr_csr()
{
    {
        uint32_t r = 0;
        asm volatile("csrr %0, vstart":"=&r"(r));
        CTEST_LOG("vstart=0x%x", r);
        asm volatile("csrr %0, vxsat":"=&r"(r));
        CTEST_LOG("vxsat=0x%x", r);
        asm volatile("csrr %0, vxrm":"=&r"(r));
        CTEST_LOG("wxrm=0x%x", r);
        asm volatile("csrr %0, vcsr":"=&r"(r));
        CTEST_LOG("vcsr=0x%x", r);
        asm volatile("csrr %0, vl":"=&r"(r));
        CTEST_LOG("vl=0x%x", r);
        asm volatile("csrr %0, vtype":"=&r"(r));
        CTEST_LOG("vtype=0x%x", r);
        asm volatile("csrr %0, vlenb":"=&r"(r));
        CTEST_LOG("vlenb=0x%x", r);
    }
}

CTEST(rv32v, rv32v_1)
{
#if 1
    {
        uint32_t a=1;
        uint32_t b=2;
        uint32_t c=10;
        asm volatile("add %0, %1, %2"   //op rd, rs1, rs2
            : "=&r"(c) //%0
            : "r"(a), "r"(b) );  //%1,%2
        ASSERT_EQUAL(c, 1+2);
    }
#endif 
}

CTEST(rv32v, csr_mstatus)
{
#ifdef __riscv_vector
    __disable_vector();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_VS, 0);
    __enable_vector();
    ASSERT_EQUAL(__RV_CSR_READ(CSR_MSTATUS) & MSTATUS_VS, MSTATUS_VS_INITIAL);
    CTEST_LOG("__riscv_vector enable\n");
#endif
}

CTEST(rv32v, vector_add_sample)
{
#ifdef __riscv_vector
    __enable_vector();
    uint64_t cur_cycle;
    
    {
        // v{add}{.vv/.vs}
        uint32_t n = 16;
        uint32_t x[16] = {1,2,3,4};
        uint32_t y[16] = {11,12,13,14};
        uint32_t z[16] = {0,0,0,0};
        cur_cycle = __get_rv_cycle();
        asm volatile(
            "vsetvli t0, %0, e32, ta, ma\n"
            "vle32.v v0,(%1)\n"
            "vle32.v v1,(%2)\n"
            "vadd.vv v2, v0, v1\n"   //op rd, rs1, rs2
            "vse32.v v2,(%3)\n"
            : 
            : "r"(n), "r"(x), "r"(y), "r"(z) 
            : "memory");  //%0,%1,%2
        cur_cycle = __get_rv_cycle() - cur_cycle;
        // for (int i = 0; i < n; i++) {
        //     //CTEST_LOG("%d,\t", z[i]);
        //     ASSERT_EQUAL(z[i], x[i]+y[i]);
        // }
    }
    CTEST_LOG("cost:%d(cycles)", (uint32_t)(cur_cycle));
    __disable_vector();
#endif  
}


CTEST(rv32v, vsetvli)
{
#ifdef __riscv_vector
    __enable_vector();
    
    {
        uint32_t n = 1024;
        uint32_t n2 = 0;
        asm volatile(
            "vsetvli %0, %1, e8, ta, ma\n"
            : "=r"(n2)
            : "r"(n)
            : "memory");    
        CTEST_LOG("e8, n2 = %d", n2);
        
        //print_vecotr_csr();
    }
    
    {
        uint32_t n = 1024;
        uint32_t n2 = 0;
        asm volatile(
            "vsetvli %0, %1, e8,m2, ta, ma\n"
            : "=r"(n2)
            : "r"(n)
            : "memory");    
        CTEST_LOG("e8,m2 n2 = %d", n2);
        
        //print_vecotr_csr();
    }
    {
        uint32_t n = 1024;
        uint32_t n2 = 0;
        asm volatile(
            "vsetvli %0, %1, e8,mf2, ta, ma\n"
            : "=r"(n2)
            : "r"(n)
            : "memory");    
        CTEST_LOG("e8,mf2 n2 = %d", n2);

        
        //print_vecotr_csr();
    }
    __disable_vector();
#endif  
}


CTEST(rv32v, vector_load_unit_stride)
{
#ifdef __riscv_vector
    __enable_vector();

#if 1
    {
        //vle8.v vd, (rs1), vm # 8-bit unit-stride load
        uint32_t n = 16;
        uint8_t x[16] = {1,2,3,4,5,6,7,8};
        uint8_t y[16] = {11,12,13,14,15,16,17,18};
        uint8_t z[16] = {0,0,0,0,0,0,0,0};
        asm volatile(
            "vsetvli t0, %0, e8, ta, ma\n"
            "vle8.v v0,(%1)\n"
            "vse8.v v0,(%3)\n"
            : 
            : "r"(n), "r"(x), "r"(y), "r"(z) 
            : "memory");  //%0,%1,%2
        for (int i = 0; i < n; i++) {
            //CTEST_LOG("%d, ", z[i]);
            ASSERT_EQUAL(z[i], x[i]);
        }
    }
#endif 

#if 0
    {
        uint32_t n = 16;
        uint8_t x[16] = {1,2,3,4,5,6,7,8};
        uint8_t y[16] = {11,12,13,14,15,16,17,18};
        uint8_t z[16] = {0,0,0,0,0,0,0,0};
        uint8_t m[16] = {1,0,1,0,1,0,1,0};
        asm volatile(
            "vsetvli t0, %0, e8, ta, ma\n"
            "vlm.v v0,(%4)\n"
            "vle8.v v1,(%1),vm\n"   //illegal
            "vse8.v v1,(%3),vm\n"
            : 
            : "r"(n), "r"(x), "r"(y), "r"(z), "r"(m) 
            : "memory");  //%0,%1,%2
        for (int i = 0; i < n; i++) {
            //CTEST_LOG("%d, ", z[i]);
            ASSERT_EQUAL(z[i], x[i]);
        }
    }
#endif 
    __disable_vector();
#endif  
}

CTEST(rv32v, vector_load_strided)
{
#ifdef __riscv_vector
    __enable_vector();

#if 1
    {
        //vlse8.v vd, (rs1), rs2, vm # 8-bit strided load
        uint32_t n = 16, s = 2;
        uint8_t x[16*2] = {1,2,3,4,5,6,7,8};
        uint8_t y[16*2] = {11,12,13,14,15,16,17,18};
        uint8_t z[16*2] = {0,0,0,0,0,0,0,0};
        asm volatile(
            "vsetvli t0, %0, e8, ta, ma\n"
            "vlse8.v v0,(%1),%4\n"
            "vse8.v v0,(%3)\n"
            : 
            : "r"(n), "r"(x), "r"(y), "r"(z), "r"(s) 
            : "memory");  //%0,%1,%2
        for (int i = 0; i < n; i++) {
            //CTEST_LOG("%d, ", z[i]);
            ASSERT_EQUAL(z[i], x[2*i]);
        }
    }
#endif
    __disable_vector();
#endif
}

CTEST(rv32v, vector_load_index)
{
#ifdef __riscv_vector
    __enable_vector();

#if 1
    {
        //vluxei8.v vd, (rs1), vs2, vm # unordered 8-bit indexed load of SEW data
        uint32_t n = 16;
        uint8_t x[16*2] = {1,2,3,4,5,6,7,8};
        uint8_t y[16*2] = {11,12,13,14,15,16,17,18};
        uint8_t z[16*2] = {0,0,0,0,0,0,0,0};
        uint8_t index[16*2] = {1,3,5,7};
        asm volatile(
            "vsetvli t0, %0, e8, ta, ma\n"  //SEW
            "vle8.v v0,(%4)\n"
            "vluxei8.v v1, (%1), v0\n"
            "vse8.v v1,(%3)\n"
            : 
            : "r"(n), "r"(x), "r"(y), "r"(z), "r"(index) 
            : "memory");  //%0,%1,%2
        // for (int i = 0; i < n; i++) {
        //     CTEST_LOG("%d, ", z[i]);
        // }
        ASSERT_EQUAL(z[0], 2);
        ASSERT_EQUAL(z[1], 4);
        ASSERT_EQUAL(z[2], 6);
        ASSERT_EQUAL(z[3], 8);
    }
#endif
    __disable_vector();
#endif
}

CTEST(rv32v, vector_unit_stride_seg_load)
{
#ifdef __riscv_vector
    __enable_vector();

#if 1
    {
        //vlseg8e8.v vd, (rs1), vm # Load eight vector registers with eight byte fields.
        uint32_t n = 16;
        uint8_t x[16*2] = {1,2,3,4,5,6,7,8};  
        uint8_t y[16*2] = {11,12,13,14,15,16,17,18};
        uint8_t z[16*2] = {0,0,0,0,0,0,0,0};
        uint8_t index[16*2] = {1,3,5,7};
        asm volatile(
            "vsetvli t0, %0, e8, ta, ma\n"  
            "vlseg2e8.v v0, (%1)\n"  //load complex values.
            "vse8.v v0,(%3)\n"  //save real values.
            "vse8.v v1,(%4)\n"  //save imaginary values.
            : 
            : "r"(n), "r"(x), "r"(y), "r"(z), "r"(z + n) 
            : "memory");  //%0,%1,%2
        for (int i = 0; i < n; i++) {
            //CTEST_LOG("%d, %d", z[i], z[i+n]);
            ASSERT_EQUAL(z[i], x[2*i]);
            ASSERT_EQUAL(z[i+n], x[2*i+1]);
        }
    }
#endif
    __disable_vector();
#endif
}