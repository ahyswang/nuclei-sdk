CC      := clang
CXX     := clang++
OBJDUMP := llvm-objdump
OBJCOPY := llvm-objcopy
# use gnu gdb to debug or upload
GDB     := riscv64-unknown-elf-gdb
AR      := llvm-ar
SIZE    := llvm-size
OPENOCD := openocd

ifeq ($(NOGC),1)
GC_CFLAGS =
GC_LDFLAGS =
else
GC_CFLAGS = -ffunction-sections -fdata-sections
GC_LDFLAGS = -Wl,--gc-sections -Wl,--check-sections
endif

ifeq ($(SIMULATION),1)
COMMON_FLAGS += -DCFG_SIMULATION
endif

ifeq ($(BANNER),0)
COMMON_FLAGS += -DNUCLEI_BANNER=0
endif

# Handle standard c library selection variable STDCLIB
ifneq ($(findstring newlib,$(STDCLIB)),)
### Handle cases when STDCLIB variable has newlib in it
ifeq ($(STDCLIB),newlib_full)
LDLIBS += -lc -lnosys
else ifeq ($(STDCLIB),newlib_fast)
LDLIBS += -lc_nano -lnosys
STDCLIB_LDFLAGS += -u _printf_float -u _scanf_float
else ifeq ($(STDCLIB),newlib_small)
LDLIBS += -lc_nano -lnosys
STDCLIB_LDFLAGS += -u _printf_float
else ifeq ($(STDCLIB),newlib_nano)
LDLIBS += -lc_nano -lnosys
else
LDLIBS +=
endif
###
else ifneq ($(findstring libncrt,$(STDCLIB)),)
### Handle cases when STDCLIB variable has libncrt in it
LDLIBS += -l$(patsubst lib%,%,$(STDCLIB))
COMMON_FLAGS += -isystem=/include/libncrt
###
else ifeq ($(STDCLIB),nostd)
### Handle cases when no standard system directories for header files
COMMON_FLAGS += -nostdinc
###
else ifeq ($(STDCLIB),nospec)
### Handle cases no specs configs are passed
COMMON_FLAGS +=
###
else
LDLIBS +=
###
endif

ifneq ($(SEMIHOST),)
LDLIBS += -lsemihost
endif

## Heap and stack size settings
## It will define symbols only used in linker script
## __STACK_SIZE and __HEAP_SIZE are not a c marco
## they are ld symbols used by linker
ifneq ($(STACKSZ),)
LDFLAGS += -Wl,--defsym=__STACK_SIZE=$(STACKSZ)
endif
ifneq ($(HEAPSZ),)
LDFLAGS += -Wl,--defsym=__HEAP_SIZE=$(HEAPSZ)
endif

## SIMU=xlspike/qemu
### enable run on xlspike and qemu auto-exit if return from main
ifneq ($(SIMU),)
SIMULATION_MODE=SIMULATION_MODE_$(call uc, $(SIMU))
COMMON_FLAGS += -DSIMULATION_MODE=$(SIMULATION_MODE)
endif

COMMON_FLAGS += -g -fno-common
COMMON_FLAGS += -march=$(RISCV_ARCH) -mabi=$(RISCV_ABI) -mcmodel=$(RISCV_CMODEL)
## Append mtune options when RISCV_TUNE is defined
## It might be defined in SoC/<SOC>/build.mk, and can be overwritten by make
ifneq ($(RISCV_TUNE),)
COMMON_FLAGS +=
endif

ifneq ($(findstring newlib,$(STDCLIB)),)
LDFLAGS += -u _isatty -u _write -u _sbrk -u _read -u _close -u _fstat -u _lseek -u __on_exit_args
endif

LDFLAGS += -fuse-ld=lld