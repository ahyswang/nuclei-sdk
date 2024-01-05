// See LICENSE for license details.
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
//#include "nuclei_sdk_soc.h"
/*
make showflags
riscv64-unknown-elf-gcc -march=rv32imafdc -mabi=ilp32d  main.c -S
riscv64-unknown-elf-objdump -S main.c.o > 1.txt
*/

#define ASSERT assert 

void test_read_reg_0()
{
    printf("%s:0x%x\r\n", __FUNCTION__, test_read_reg_0);
    uint32_t v = 0;
    //asm volatile("add %0, x0, x0":"=r"(v)::"memory");
    asm volatile("mv %0, x0":"=r"(v)::"memory");
    printf("x0:0x%x\r\n", v);
    asm volatile("mv %0, ra":"=r"(v)::"memory");
    printf("ra:0x%x\r\n", v);
    asm volatile("mv %0, sp":"=r"(v)::"memory");
    printf("sp:0x%x\r\n", v);
    // asm volatile("mv %0, pc":"=r"(v)::"memory");
    // printf("pc:0x%x\r\n", v);
}

void test_inline_assembly()
{
#if 1
    {
        uint32_t a=1;
        uint32_t b=2;
        uint32_t c=10;
        asm volatile("add %0, %1, %2"   //op rd, rs1, rs2
            : "=&r"(c) //%0
            : "r"(a), "r"(b) );  //%1,%2
        ASSERT(c==1+2);
    }
#endif 

#if 1
    {
        /*
        # 说明：
        * APP 表示内联汇编开始。
        * APP 前面部分表示输入寄存器读取变量的内存。
        * NO_APP 表示内联汇编结束。
        * #NO_APP 后面部分表示输出寄存器保存到变量的内存。
        
        # 实例：
        ```
        	lw	a3,-28(s0)
            lw	a4,-20(s0)
            lw	a5,-24(s0)
        #APP
        # 40 "main.c" 1
            add a3, a4, a5
        # 0 "" 2
        #NO_APP
            sw	a3,-28(s0)
            sw	a4,-20(s0)
            sw	a5,-24(s0)
        ```
        */
        uint32_t a=1;
        uint32_t b=2;
        uint32_t c=10;
        c = 0;
        asm volatile("add %0, %1, %2" 
            : "=&r"(c), "=&r"(a), "=&r"(b)   //%0,%1,%2
            : "0"(c), "1"(a), "2"(b) );  //%3,%4,%5
        ASSERT(c==1+2);
    }
#endif 

#if 1
    {
        /*
        说明：
        * 验证操作数编号依次增加，输出操作数编号（%0-%1-%2），输入操作数编号（%3-%4-%5）。
        */
        uint32_t a=1;
        uint32_t b=2;
        uint32_t c=10;

        asm volatile("add %0, %4, %5" 
            : "=&r"(c), "=&r"(a), "=&r"(b) 
            : "0"(c), "1"(a), "2"(b) );   
        ASSERT(c==1+2);

        c = 10;
        asm volatile("add %0, %1, %0" 
            : "=&r"(c), "=&r"(a), "=&r"(b)  //%0,%1,%2
            : "0"(c), "1"(a), "2"(b) );  //%3,%4,%5 
        ASSERT(c==1+10);
    }
#endif 

#if 1
    {
        uint32_t a=1;
        uint32_t b=2;
        uint32_t c=10;
        asm volatile("add %0, %2, %3" : "=r"(c): // %0
            "0"(c), // %1 复用寄存器%0
            "r"(a), //%2
            "r"(b) ); //%3
        ASSERT(c == a + b);
    }
#endif

#if 1
    {
        //读取立即数
        uint32_t a=1;
        uint32_t c=10;
        asm volatile("addi %0, %1, %2" : "=r"(c): 
            "r"(a), 
            "i"(2) );
        ASSERT(c == a + 2);
    }
#endif 

}

void test_inline_assembly_2()
{  
#if 1
    {
        /*
            lw	a5,-24(s0)  # b
        #APP
        # 131 "main.c" 1
            mv a5, a5 
        # 0 "" 2
        #NO_APP
            sw	a5,-20(s0) # a
        */
        uint32_t a=1;
        uint32_t b=2;
        asm volatile("mv %0, %1" : "=r"(a): 
            "r"(b));
        ASSERT(a==2);
    }
#endif 

#if 1
    {
        /*
            //分配给输出的寄存器不要占用输入绑定的寄存器，因为这些输入可能还没有使用完。
            lw	a4,-32(s0)
            #APP
            # 149 "main.c" 1
                mv a5, a4
            # 0 "" 2
            #NO_APP
                sw	a5,-28(s0)
        */
        uint32_t a=1;
        uint32_t b=2;
        asm volatile("mv %0, %1" : "=&r"(a):   // %0绑定的寄存器不能分配给其他寄存器作为输入
            "r"(b));
        ASSERT(a==2);
    }
#endif 

}

void test_isa_1()
{
    //
    int C[1] = {0};
    int A[1] = {1};
    int B[1] = {2}; 
    uint32_t N = 1;
    asm volatile(
        "add t6, t4, t5 "
        );
    // asm volatile(
    //     "ld t4 0(%1) \n\t"
    //     "ld t5 0(%2) \n\t"
    //     "add t6, t4, t5 \n\t"
    //     "sd t5 0(%0) \n\t"
    //     : "=r"(C), "=r"(A), "=r"(B), "=r"(N)
    //     : "0"(C), "1"(A), "2"(B), "2"(N)
    //     : "t4", "t5", "t6", "memory");
}

void test_isa_ldsd_1()
{
    {
        int a = 1;
        int b = 2;
        asm volatile(
            "li	t0,2\t\n"
            "lw t0,0(%1)\t\n"
            "sw t0,0(%0)\t\n"
            : 
            : "r"(&a), "r"(&b)
            : "t0", "memory");
        ASSERT(a == 2);
    }
}

void test_simd_1()
{
    /*
    # 说明
    * 支持P指令编译运行。
    ```
    make CORE=n307fd  ARCH_EXT=xxldsp
    make CORE=n307fd  ARCH_EXT=xxldsp run_qemu
    qemu-system-riscv32 -M nuclei_evalsoc,download=ilm -cpu nuclei-n307fd,ext=xxldsp -smp 1 -icount shift=0 -nodefaults -nographic -serial stdio -kernel hello.elf
    ```
    * 支持P指令调试。
    ```
    qemu-system-riscv32 -M nuclei_evalsoc,download=ilm -cpu nuclei-n307fd,ext=xxldsp -smp 1 -icount shift=0 -nodefaults -nographic -serial stdio -s -S
    riscv64-unknown-elf-gdb hello.elf
    target remote localhost:1234
    b main
    set disassemble-next-line on
    disassemble
    info register r5
    ```
    */
    int a = 0x00010002;
    int b = 0x00030004;
    int c = 0;
    asm volatile(
        "add16 %0,%1,%2 \t\n"
        : "=&r"(c)
        : "r"(a), "r"(b)
        : "memory"
    );
    //printf("0x%x\r\n", c);
    ASSERT(0x00040006 == c);
}


void test_isa_loads_stores()
{
    uint32_t r0=0,r1=0,r2=0,r3=0,r4=0,r5=0,r6=0,r7=0;
    {
        //l{b/h/w}
        char buf[] __attribute__((__aligned__(4))) = {0x1, 0x02, 0x03, 0x04};
        asm volatile("lb %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0x01);
        asm volatile("lh %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0x0201);
        asm volatile("lw %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0x04030201);
    }
    {
        //l{b/h}{_/s}
        char buf[] __attribute__((__aligned__(4))) = {0xff, 0xfe, 0xfd, 0xfc}; //-1,-2,-3,-4
        asm volatile("lbu %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0xff);
        asm volatile("lhu %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0xfeff);
        asm volatile("lb %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0xffffffff);
        asm volatile("lh %0, (%1)":"=r"(r0):"r"(buf):"memory");
        ASSERT(r0 == 0xfffffeff);
    }
    {
        //s{b/h/w}
        char buf[] __attribute__((__aligned__(4))) = {0};
        asm volatile("sb %0, (%1)"::"r"(0x01), "r"(buf):"memory");
        ASSERT(buf[0] == 0x01);
        asm volatile("sh %0, (%1)"::"r"(0x0201), "r"(buf):"memory");
        ASSERT(buf[0] == 0x01 && buf[1] == 0x02);
        asm volatile("sw %0, (%1)"::"r"(0x04030201), "r"(buf):"memory");
        ASSERT(buf[0] == 0x01 && buf[1] == 0x02 && buf[2] == 0x03 && buf[3] == 0x04);
    }
    {
        //s{b/h}
        char buf[] __attribute__((__aligned__(4))) = {0};
        asm volatile("sb %0, (%1)"::"r"(0x04030201), "r"(buf):"memory");
        ASSERT(buf[0] == 0x01);
        asm volatile("sb %0, (%1)"::"r"(0xff030201), "r"(buf):"memory"); //trunc
        ASSERT(buf[0] == 0x01);
        asm volatile("sh %0, (%1)"::"r"(0x04030201), "r"(buf):"memory"); 
        ASSERT(buf[0] == 0x01 && buf[1] == 0x02);
        asm volatile("sh %0, (%1)"::"r"(0xffff0201), "r"(buf):"memory"); //trunc
        ASSERT(buf[0] == 0x01 && buf[1] == 0x02);
    }
}

void test_isa_inter_compution()
{
    uint32_t r0=0,r1=0,r2=0,r3=0,r4=0,r5=0,r6=0,r7=0;
    {
        //add{_/i}
        asm volatile("add %0, %1, %2":"=r"(r0):"r"(1), "r"(2):);
        ASSERT(r0 == 1+2);
        asm volatile("addi %0, %1, %2":"=r"(r0):"r"(1), "i"(2):);   //12bit
        ASSERT(r0 == 1+2);
        asm volatile("addi %0, %1, %2":"=r"(r0):"r"(1), "i"(-2):);   //12bit
        ASSERT(r0 == 1-2);
        asm volatile("addi %0, %1, %2":"=r"(r0):"r"(1), "i"(2047):);   //12bit
        ASSERT(r0 == 1 + (2047));
        asm volatile("addi %0, %1, %2":"=r"(r0):"r"(1), "i"(-2048):);   //12bit
        ASSERT(r0 == 1 + (-2048));
    }
    {
        //sub
        asm volatile("sub %0, %1, %2":"=r"(r0):"r"(1), "r"(2):);
        ASSERT(r0 == 1-2);
    }
    {
        //{and/or/xor}{_/i}
        asm volatile("and %0, %1, %2":"=r"(r0):"r"(0b110), "r"(0b101):);
        ASSERT(r0 == (0b110&0b101));
        asm volatile("or %0, %1, %2":"=r"(r0):"r"(0b110), "r"(0b101):);
        ASSERT(r0 == (0b110|0b101));
        asm volatile("xor %0, %1, %2":"=r"(r0):"r"(0b110), "r"(0b101):);
        ASSERT(r0 == (0b110^0b101));

        asm volatile("and %0, %1, %2":"=r"(r0):"r"(0b110), "i"(0b101):);
        ASSERT(r0 == (0b110&0b101));
        asm volatile("or %0, %1, %2":"=r"(r0):"r"(0b110), "i"(0b101):);
        ASSERT(r0 == (0b110|0b101));
        asm volatile("xor %0, %1, %2":"=r"(r0):"r"(0b110), "i"(0b101):);
        ASSERT(r0 == (0b110^0b101));
    }

    {
        //{sll/srl/sra}{_/i}
        asm volatile("sll %0, %1, %2":"=r"(r0):"r"(0b110), "r"(1):);
        ASSERT(r0 == (0b110u<<1));
        asm volatile("srl %0, %1, %2":"=r"(r0):"r"(0b110), "r"(1):);
        ASSERT(r0 == (0b110u>>1));
        asm volatile("srl %0, %1, %2":"=r"(r0):"r"(0xf0000000), "r"(1):);
        ASSERT(r0 == (0xf0000000u>>1));
        asm volatile("sra %0, %1, %2":"=r"(r0):"r"(0xf0000000), "r"(1):);
        ASSERT(r0 == (((signed)0xf0000000)>>1));
    }

    {
        //lui
        asm volatile("lui %0, %1":"=&r"(r0):"i"(0x04030201>>12):);   //upper 20bit
        ASSERT(r0 == (0x04030<<12));
        asm volatile("addi %0, %1, %2":"=r"(r0):"r"(r0),"i"(0x04030201&0xfff):);  //lower 12bit
        ASSERT(r0 == (0x04030201));
        //auipc
    }
    {
        //slt{_/i}{_/u}
        asm volatile("slt %0, %1, %2":"=&r"(r0):"r"(2),"r"(3):);   
        ASSERT(r0 == 1);
        asm volatile("slt %0, %1, %2":"=&r"(r0):"r"(3),"r"(2):);   
        ASSERT(r0 == 0);
        asm volatile("slti %0, %1, %2":"=&r"(r0):"r"(2),"i"(3):);   
        ASSERT(r0 == 1);
        asm volatile("slt %0, %1, %2":"=&r"(r0):"r"(-2),"r"(3):);  
        ASSERT(r0 == 1);
        asm volatile("sltu %0, %1, %2":"=&r"(r0):"r"(-2),"r"(3):);  // 0xfffffffe  0x3
        ASSERT(r0 == 0);
    }
}

void test_isa_misc()
{
    uint32_t r0=0,r1=0,r2=0,r3=0,r4=0,r5=0,r6=0,r7=0;
    {
        //csr{r}{c|s|w}{_/i}
        // asm volatile("csrr %0, %1, %2":"=r"(r0):"r"(1), "r"(2):);  //csr: upper 12bit
        // ASSERT(r0 == 1+2);
        //csrrs rd,csr,x0  read csr
        //csrrw x0,csr,rs  write csr 
    }
    {
        //
        //csrrs rd, cycle[h], x0
        asm volatile("csrrs %0, cycle, x0":"=r"(r0));
        asm volatile(
            "add x0,x0,x0\t\n"
            "add x0,x0,x0\t\n"
            "add x0,x0,x0\t\n"
            "add x0,x0,x0\t\n"
        );
        asm volatile("csrrs %0, cycle, x0":"=r"(r1));
        //asm volatile("csrrs %0, cycleh, x0":"=r"(r1));
        printf("0x%x 0x%x\r\n", r0, r1);
    }
    {
        asm volatile("rdcycleh %1 \r\n rdcycle %0 \r\n ":"=r"(r0),"=r"(r1));
        asm volatile(
            "add x0,x0,x0\t\n"
            "add x0,x0,x0\t\n"
            "add x0,x0,x0\t\n"
            "add x0,x0,x0\t\n"
        );
        asm volatile("rdcycleh %1 \r\n rdcycle %0 \r\n ":"=r"(r2),"=r"(r3));
        printf("start cycles: 0x%x 0x%x\r\n", r0, r1);
        printf("stop cycles: 0x%x 0x%x\r\n", r2, r3);
    }
}

void test_isa_control_transfer()
{
    uint32_t r0=0,r1=0,r2=0,r3=0,r4=0,r5=0,r6=0,r7=0;
    {
        //b{eq|ne}
        r0 = 10;
        asm volatile(
            "beq %2,%3,L1 \t\n"
            "addi %0,%1,1 \t\n"
            "L1:"
        :"=r"(r0):"0"(r0),"r"(2),"r"(2));
        ASSERT(r0 == 10);
        r0 = 10;
        asm volatile(
            "bne %2,%3,L3 \t\n"
            "addi %0,%1,1 \t\n"
            "L3:"
        :"=r"(r0):"0"(r0),"r"(2),"r"(3));
        ASSERT(r0 == 10);
        r0 = 10;
        asm volatile(
            "beq %2,%3,L2 \t\n"
            "addi %0,%1,1 \t\n"
            "L2:"
        :"=r"(r0):"0"(r0),"r"(2),"r"(3));
        ASSERT(r0 == 11);
    }
    {
        //b{ge|lt}{_|i}
        r0 = 10;
        asm volatile(
            "bge %2,%3,L4 \t\n"
            "addi %0,%1,1 \t\n"
            "L4:"
        :"=r"(r0):"0"(r0),"r"(3),"r"(2));
        ASSERT(r0 == 10);
        r0 = 10;
        asm volatile(
            "blt %2,%3,L5 \t\n"
            "addi %0,%1,1 \t\n"
            "L5:"
        :"=r"(r0):"0"(r0),"r"(2),"r"(3));
        ASSERT(r0 == 10);
    }
    {
        //jal{_|r}
        r0 = 10;
        asm volatile(
            "jal x0,L6 \t\n"   //jump to L6, x0 = pc
            "addi %0,%1,1 \t\n"
            "L6:"
        :"=r"(r0):"0"(r0),"r"(2),"r"(3));
        ASSERT(r0 == 10);
        r0 = 10;
        asm volatile(
            "jal %0,L7 \t\n"   //jump to L6, x0 = pc
            "addi %0,%1,1 \t\n"
            "L7:"
        :"=r"(r0):"0"(r0),"r"(2),"r"(3));
        //printf("0x%x\n", r0);  // pc
        /*
asm volatile(
 1ea:	fec42783          	lw	a5,-20(s0)
 1ee:	004002ef          	jal	t0,1f2 <L8>

000001f2 <L8>:
 1f2:	00628067          	jr	6(t0)
 1f6:	0785                	add	a5,a5,1
 1f8:	0785                	add	a5,a5,1
 1fa:	0785                	add	a5,a5,1
 1fc:	fef42623          	sw	a5,-20(s0)
 200:	fed42423          	sw	a3,-24(s0)
 204:	fee42223          	sw	a4,-28(s0)
        */
        r0 = 10;
        asm volatile(
            "jal t0,L8 \t\n"  // pc => t0
            "L8:\t\n"
            "jalr x0,t0,6 \t\n" //jump pc + 4
            "addi %0,%3,1 \t\n"  //t0+4  code = 2bytes
            "addi %0,%3,1 \t\n"  //t0+6  code = 2bytes
            "addi %0,%3,1 \t\n"  //t0+8  code = 2bytes
        :"=r"(r0),"=r"(r1),"=r"(r2):"0"(r0):"t0");
        ASSERT(r0 == 12);
    }
}


int main(void)
{
    printf("Hello World from Nuclei RISC-V Processor!\r\n");
    test_read_reg_0();
    test_inline_assembly();
    test_inline_assembly_2();
    test_isa_1();
    test_isa_ldsd_1();
    test_simd_1();
    test_isa_loads_stores();
    test_isa_inter_compution();
    test_isa_control_transfer();
    test_isa_misc();
    return 0;
}