	.text
	.syntax unified
	.eabi_attribute	67, "2.09"
	.cpu	cortex-m0
	.eabi_attribute	6, 12
	.eabi_attribute	8, 0
	.eabi_attribute	9, 1
	.eabi_attribute	17, 1
	.eabi_attribute	20, 1
	.eabi_attribute	21, 1
	.eabi_attribute	23, 3
	.eabi_attribute	34, 1
	.eabi_attribute	24, 1
	.eabi_attribute	25, 1
	.eabi_attribute	38, 1
	.eabi_attribute	14, 0
	.file	"../../../examples/compiled_code/identity.asm"
	.globl	predict
	.p2align	1
	.type	predict,%function
	.code	16
	.thumb_func
predict:
	.fnstart
	ldm	r0!, {r2, r3}
	stm	r1!, {r2, r3}
	ldr	r2, [r0]
	ldr	r3, [r0, #4]
	str	r3, [r1, #4]
	str	r2, [r1]
	ldr	r2, [r0, #8]
	ldr	r0, [r0, #12]
	str	r0, [r1, #12]
	str	r2, [r1, #8]
	bx	lr
.Lfunc_end0:
	.size	predict, .Lfunc_end0-predict
	.fnend


	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 1
