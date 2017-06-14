	.section .init
	.globl _start
	.type _start,@function

_start:
.option push
.option norelax
	la gp, __global_pointer$
.option pop
	la sp, _sp

#if 0
	/* Load the ITIM/non-flash text  */
	la a0, _text_lma
	la a1, _text
	la a2, _etext
	/* There will be .text, no need to test for that. */
0:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 0b
#endif

	/* load data */
	la a0, _data_lma
	la a1, _data
	la a2, _edata
	bgeu a1, a2, 1f
0:
	lw t0, (a0)
	sw t0, (a1)
	addi a0, a0, 4
	addi a1, a1, 4
	bltu a1, a2, 0b
1:

	/* zero bss */
	la a0, __bss_start
	la a1, _end
	bgeu a0, a1, 1f
0:
	sw zero, (a0)
	addi a0, a0, 4
	bltu a0, a1, 0b
1:
	/* void main(void) */
	call main
0:
	j 0b
