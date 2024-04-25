#include <stdlib.h>


#include "ctest.h"

CTEST(rv32m, rv32m_1)
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

CTEST(rv32m, rv32m_mul)
{
#if 1
    {
        //mul
        //mulh{_/u/su}
        uint32_t a=(0x3<<16)|(0x1);
        uint32_t b=(0x4<<16)|(0x2);
        uint32_t c=0x0;
        asm volatile("mul %0, %1, %2"   //op rd, rs1, rs2
            : "=&r"(c) //%0
            : "r"(a), "r"(b) );  //%1,%2
        ASSERT_EQUAL(c, (a*b)&0xffffff);

        asm volatile("mulh %0, %1, %2"   //op rd, rs1, rs2
            : "=&r"(c) //%0
            : "r"(a), "r"(b) );  //%1,%2
        ASSERT_EQUAL(c, ((uint64_t)a*(uint64_t)b)>>32);
    }
#endif 
}

CTEST(rv32m, rv32m_div)
{
#if 1
    {
        //{div/rem}{_/u}
        uint32_t a=0x17;
        uint32_t b=0x4;
        uint32_t c=0x0;
        asm volatile("div %0, %1, %2"   //op rd, rs1, rs2
            : "=&r"(c) //%0
            : "r"(a), "r"(b) );  //%1,%2
        ASSERT_EQUAL(c, a/b);

        asm volatile("rem %0, %1, %2"   //op rd, rs1, rs2
            : "=&r"(c) //%0
            : "r"(a), "r"(b) );  //%1,%2
        ASSERT_EQUAL(c, a%b);
    }
#endif 
}