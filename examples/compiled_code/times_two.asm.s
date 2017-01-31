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
	.file	"../../../examples/compiled_code/times_two.asm"
	.globl	predict
	.p2align	2
	.type	predict,%function
	.code	16
	.thumb_func
predict:
	.fnstart
	.save	{r4, r5, r6, r7, lr}
	push	{r4, r5, r6, r7, lr}
	.pad	#12
	sub	sp, #12
	str	r1, [sp, #4]
	mov	r5, r0
	movs	r0, #0
	str	r0, [sp, #8]
	ldr	r7, .LCPI0_1
	ldr	r6, .LCPI0_0
	b	.LBB0_2
.LBB0_1:
	ldr	r0, [sp, #8]
	lsls	r4, r0, #3
	ldr	r0, [r5, r4]
	ldr	r2, [r7, r4]
	adds	r1, r5, r4
	ldr	r1, [r1, #4]
	adds	r3, r7, r4
	ldr	r3, [r3, #4]
	bl	__aeabi_dmul
	str	r0, [r6, r4]
	adds	r0, r6, r4
	str	r1, [r0, #4]
	ldr	r0, [sp, #8]
	adds	r0, r0, #1
	str	r0, [sp, #8]
.LBB0_2:
	ldr	r0, [sp, #8]
	cmp	r0, #2
	ble	.LBB0_1
	ldm	r6!, {r0, r1}
	ldr	r2, [sp, #4]
	stm	r2!, {r0, r1}
	ldr	r0, [r6]
	ldr	r1, [r6, #4]
	str	r1, [r2, #4]
	str	r0, [r2]
	ldr	r0, [r6, #8]
	ldr	r1, [r6, #12]
	str	r1, [r2, #12]
	str	r0, [r2, #8]
	add	sp, #12
	pop	{r4, r5, r6, r7, pc}
	.p2align	2
.LCPI0_0:
	.long	g_0
.LCPI0_1:
	.long	c_0
.Lfunc_end0:
	.size	predict, .Lfunc_end0-predict
	.fnend

	.type	c_0,%object
	.section	.rodata,"a",%progbits
	.p2align	4
c_0:
	.long	0
	.long	1073741824
	.long	0
	.long	1073741824
	.long	0
	.long	1073741824
	.size	c_0, 24

	.type	g_0,%object
	.local	g_0
	.comm	g_0,24,16

	.section	".note.GNU-stack","",%progbits
	.eabi_attribute	30, 1
