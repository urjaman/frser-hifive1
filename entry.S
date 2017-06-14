#include "encoding.h"
#include "sifive/bits.h"

  .section      .text.entry
  .align 2
  .global trap_entry
trap_entry:
  addi sp, sp, -16*REGBYTES

  STORE ra, 0*REGBYTES(sp)
  STORE t0, 1*REGBYTES(sp)
  STORE t1, 2*REGBYTES(sp)
  STORE t2, 3*REGBYTES(sp)
  STORE a0, 4*REGBYTES(sp)
  STORE a1, 5*REGBYTES(sp)
  STORE a2, 6*REGBYTES(sp)
  STORE a3, 7*REGBYTES(sp)
  STORE a4, 8*REGBYTES(sp)
  STORE a5, 9*REGBYTES(sp)
  STORE a6, 10*REGBYTES(sp)
  STORE a7, 11*REGBYTES(sp)
  STORE t3, 12*REGBYTES(sp)
  STORE t4, 13*REGBYTES(sp)
  STORE t5, 14*REGBYTES(sp)
  STORE t6, 15*REGBYTES(sp)

  csrr a0, mcause
  call handle_trap

  # Remain in M-mode after mret (why is this needed?, is it needed?)
  li t0, MSTATUS_MPP
  csrs mstatus, t0

  LOAD ra, 0*REGBYTES(sp)
  LOAD t0, 1*REGBYTES(sp)
  LOAD t1, 2*REGBYTES(sp)
  LOAD t2, 3*REGBYTES(sp)
  LOAD a0, 4*REGBYTES(sp)
  LOAD a1, 5*REGBYTES(sp)
  LOAD a2, 6*REGBYTES(sp)
  LOAD a3, 7*REGBYTES(sp)
  LOAD a4, 8*REGBYTES(sp)
  LOAD a5, 9*REGBYTES(sp)
  LOAD a6, 10*REGBYTES(sp)
  LOAD a7, 11*REGBYTES(sp)
  LOAD t3, 12*REGBYTES(sp)
  LOAD t4, 13*REGBYTES(sp)
  LOAD t5, 14*REGBYTES(sp)
  LOAD t6, 15*REGBYTES(sp)

  addi sp, sp, 16*REGBYTES
  mret
