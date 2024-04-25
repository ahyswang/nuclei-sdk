#include <stdlib.h>
#include <math.h>

#include "ctest.h"

#define ASSERT_EQUAL_F(a, b) ASSERT_EQUAL(fabs((a)-(b))<=1.0e-10f, 1)


CTEST(rv32fd, load_store)
{
#if 1
    {
        //f{l/s}{w/d}
        float a=1;
        float b=2;
        float c=10;
        asm volatile("flw %0, 0(%1)"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "r"(&a));  //%1
        ASSERT_EQUAL(c == 1, 1);
    }
#endif 

    {
        //f{l/s}{w/d}
        float a=1;
        float b=2;
        float c=10;
        asm volatile("fsw %0, 0(%1)"   //op rd, rs1, rs2
            : 
            : "f"(c), "r"(&a)
            : "memory");  //%1
        ASSERT_EQUAL(a == 10, 1);
    }
}

CTEST(rv32fd, computation)
{
#if 1
    {
        //fadd{.s/.d}
        float a=1;
        float b=2;
        float c=10;
        asm volatile("fadd.s %0, %1, %2"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "f"(a), "f"(b) );  //%1,%2
        ASSERT_EQUAL(c == 1+2, 1);
    }
    {
        //mul{.s/.d}
        float a=1;
        float b=2;
        float c=10;
        asm volatile("fmul.s %0, %1, %2"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "f"(a), "f"(b) );  //%1,%2
        ASSERT_EQUAL(c == 1.0*2.0, 1);
    }
    {
        //div{.s/.d}
        float a=1;
        float b=2;
        float c=10;
        asm volatile("fdiv.s %0, %1, %2"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "f"(a), "f"(b) );  //%1,%2
        ASSERT_EQUAL(c == 1.0/2.0, 1);
    }
    {
        //div{.s/.d}
        float a=1;
        float b=2;
        float c=10;
        asm volatile("fsqrt.s %0, %1"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "f"(b));  //%1,%2
        //printf("%f\n", c);
        ASSERT_EQUAL_F(c, sqrtf(b));
    }
#endif 
}

CTEST(rv32fd, round_mode)
{
#if 1
    {
        //fadd.s ft0，ft1，ft2，rtz，
        //fadd{.s/.d}
        float a=0.1;
        float b=0.2;
        float c=10;
        asm volatile("fadd.s %0, %1, %2, rtz"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "f"(a), "f"(b) );  //%1,%2
        ASSERT_EQUAL(fabs(c-0.3f)<1.0e-6f, 1);
    }
    {
        //fadd.s ft0，ft1，ft2，rtz，
        //fadd{.s/.d}
        float a=0.1;
        float b=0.5;
        float c=10;
        asm volatile("fadd.s %0, %1, %2, rtz"   //op rd, rs1, rs2
            : "=&f"(c) //%0
            : "f"(a), "f"(b) );  //%1,%2
        //printf("%f\n", c);
        ASSERT_EQUAL(fabs(c-0.6f)<1.0e-6f, 1);
    }
#endif 
}