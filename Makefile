# Optimally, you should only need to point to the SDK.
SDK_PATH := $(shell echo ~/freedom-e-sdk)
BOARD = freedom-e300-hifive1
TCDIR = $(SDK_PATH)/work/build/riscv-gnu-toolchain/riscv64-unknown-elf/prefix/bin/
BSPDIR = $(SDK_PATH)/bsp

BSPINC = -I$(BSPDIR)/include -I$(BSPDIR)/env -I$(BSPDIR)/env/$(BOARD)
CROSS_COMPILE = riscv64-unknown-elf-

include $(BSPDIR)/env/$(BOARD)/settings.mk

CC = $(TCDIR)$(CROSS_COMPILE)gcc
SIZE = $(TCDIR)$(CROSS_COMPILE)size
OBJCOPY = $(TCDIR)$(CROSS_COMPILE)objcopy


OPTIONS = -DF_CPU=192000000
CFLAGS  = -Wall -g -Os
CFLAGS += -march=$(RISCV_ARCH)
CFLAGS += -mabi=$(RISCV_ABI)
CFLAGS += -mcmodel=medany
CFLAGS += -nostartfiles -nostdlib
CFLAGS += -Wno-main
CFLAGS += -Wl,-relax
CFLAGS += $(BSPINC)
CFLAGS += $(OPTIONS)

LDFLAGS = -Tlink.lds

DEPS = main.h uart.h spihw.h delay.h
SOURCES = start.S entry.S traps.c main.c uart.c spihw.c delay.c flash.c

include libfrser/Makefile.frser
include libfrser/Makefile.spilib

all: frser.elf size

frser.elf: $(SOURCES) $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -I. -flto -flto-partition=none -fwhole-program  -o $@ $(SOURCES) -lc


size: frser.elf
	$(SIZE) frser.elf

# This "upload" crap. Hey riscv folks, a bit too many commands for a darn upload...
RISCV_OPENOCD ?= $(SDK_PATH)/work/build/openocd/prefix/bin/openocd
RISCV_GDB = $(TCDIR)$(CROSS_COMPILE)gdb


OPENOCDCFG ?= $(BSPDIR)/env/$(BOARD)/openocd.cfg
OPENOCDARGS += -f $(OPENOCDCFG)

GDB_UPLOAD_ARGS ?= --batch
GDB_PORT ?= 3333

GDB_UPLOAD_CMDS += -ex "set remotetimeout 240"
GDB_UPLOAD_CMDS += -ex "target extended-remote localhost:$(GDB_PORT)"
GDB_UPLOAD_CMDS += -ex "monitor reset halt"
GDB_UPLOAD_CMDS += -ex "monitor flash protect 0 64 last off"
GDB_UPLOAD_CMDS += -ex "load"
GDB_UPLOAD_CMDS += -ex "monitor resume"
GDB_UPLOAD_CMDS += -ex "monitor shutdown"
GDB_UPLOAD_CMDS += -ex "quit"

program: frser.elf
	$(RISCV_OPENOCD) $(OPENOCDARGS) & \
	$(RISCV_GDB) frser.elf $(GDB_UPLOAD_ARGS) $(GDB_UPLOAD_CMDS) && \
	echo "Successfully uploaded."

clean:
	-rm -rf *.o frser.elf

objdump: frser.elf
	$(TCDIR)$(CROSS_COMPILE)objdump -xdC frser.elf | less

run_openocd:
	$(RISCV_OPENOCD) $(OPENOCDARGS)

GDBCMDS += -ex "set remotetimeout 240"
GDBCMDS += -ex "target extended-remote localhost:$(GDB_PORT)"

run_gdb:
	$(RISCV_GDB) frser.elf $(GDBARGS) $(GDBCMDS)
