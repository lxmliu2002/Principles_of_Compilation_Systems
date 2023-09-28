	.arch armv7-a
	.fpu vfpv3-d16
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"fib.c"
	.text
	.global	n
	.section	.rodata
	.align	2
	.type	n, %object
	.size	n, 4
n:
	.word	5
	.align	2
.LC1:
	.ascii	"f[5] = %d\012\000"
	.align	2
.LC0:
	.word	1
	.word	1
	.word	0
	.word	0
	.word	0
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 32
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r4, r5, r7, lr}
	sub	sp, sp, #32
	add	r7, sp, #0
	ldr	r2, .L7
.LPIC2:
	add	r2, pc
	ldr	r3, .L7+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #28]
	mov	r3, #0
	ldr	r3, .L7+8
.LPIC0:
	add	r3, pc
	add	r4, r7, #8
	mov	r5, r3
	ldmia	r5!, {r0, r1, r2, r3}
	stmia	r4!, {r0, r1, r2, r3}
	ldr	r3, [r5]
	str	r3, [r4]
	movs	r3, #0
	str	r3, [r7, #4]
	b	.L2
.L4:
	ldr	r3, [r7, #4]
	cmp	r3, #1
	bgt	.L3
	ldr	r3, [r7, #4]
	adds	r3, r3, #1
	str	r3, [r7, #4]
	b	.L2
.L3:
	ldr	r3, [r7, #4]
	subs	r3, r3, #1
	lsls	r3, r3, #2
	adds	r3, r3, #32
	add	r3, r3, r7
	ldr	r2, [r3, #-24]
	ldr	r3, [r7, #4]
	subs	r3, r3, #2
	lsls	r3, r3, #2
	adds	r3, r3, #32
	add	r3, r3, r7
	ldr	r3, [r3, #-24]
	add	r2, r2, r3
	ldr	r3, [r7, #4]
	lsls	r3, r3, #2
	adds	r3, r3, #32
	add	r3, r3, r7
	str	r2, [r3, #-24]
	ldr	r3, [r7, #4]
	adds	r3, r3, #1
	str	r3, [r7, #4]
.L2:
	movs	r2, #5
	ldr	r3, [r7, #4]
	cmp	r3, r2
	blt	.L4
	ldr	r3, [r7, #24]
	mov	r1, r3
	ldr	r3, .L7+12
.LPIC1:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	movs	r3, #0
	ldr	r1, .L7+16
.LPIC3:
	add	r1, pc
	ldr	r2, .L7+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #28]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L6
	bl	__stack_chk_fail(PLT)
.L6:
	mov	r0, r3
	adds	r7, r7, #32
	mov	sp, r7
	@ sp needed
	pop	{r4, r5, r7, pc}
.L8:
	.align	2
.L7:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC0+4)
	.word	.LC1-(.LPIC1+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC3+4)
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",%progbits
