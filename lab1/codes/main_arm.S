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
	.file	"main.c"
	.text
	.section	.rodata
	.align	2
.LC0:
	.ascii	"%d\000"
	.align	2
.LC1:
	.ascii	"i\345\260\217\344\272\2161\000"
	.align	2
.LC2:
	.ascii	"\350\276\223\345\207\272\350\266\205\351\231\220\000"
	.align	2
.LC3:
	.ascii	"%d\012\000"
	.align	2
.LC4:
	.ascii	"\347\250\213\345\272\217\350\277\220\350\241\214\346"
	.ascii	"\227\266\351\227\264\344\270\272\357\274\232%f\012\000"
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 1, uses_anonymous_args = 0
	push	{r7, lr}
	sub	sp, sp, #40
	add	r7, sp, #0
	ldr	r2, .L9+8
.LPIC5:
	add	r2, pc
	ldr	r3, .L9+12
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #36]
	mov	r3, #0
	adds	r3, r7, #4
	mov	r1, r3
	ldr	r3, .L9+16
.LPIC0:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	movs	r3, #2
	str	r3, [r7, #8]
	movs	r3, #1
	str	r3, [r7, #12]
	bl	clock(PLT)
	str	r0, [r7, #16]
	ldr	r3, [r7, #8]
	cmp	r3, #0
	bgt	.L3
	ldr	r3, .L9+20
.LPIC1:
	add	r3, pc
	mov	r0, r3
	bl	puts(PLT)
	b	.L3
.L4:
	ldr	r3, [r7, #12]
	ldr	r2, [r7, #8]
	mul	r3, r2, r3
	str	r3, [r7, #12]
	ldr	r3, [r7, #8]
	adds	r3, r3, #1
	str	r3, [r7, #8]
.L3:
	ldr	r3, [r7, #4]
	ldr	r2, [r7, #8]
	cmp	r2, r3
	ble	.L4
	ldr	r3, [r7, #12]
	movw	r2, #10000
	cmp	r3, r2
	ble	.L5
	ldr	r3, .L9+24
.LPIC2:
	add	r3, pc
	mov	r0, r3
	bl	puts(PLT)
	b	.L6
.L5:
	ldr	r1, [r7, #12]
	ldr	r3, .L9+28
.LPIC3:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
.L6:
	bl	clock(PLT)
	str	r0, [r7, #20]
	ldr	r2, [r7, #20]
	ldr	r3, [r7, #16]
	subs	r3, r2, r3
	vmov	s15, r3	@ int
	vcvt.f64.s32	d6, s15
	vldr.64	d5, .L9
	vdiv.f64	d7, d6, d5
	vstr.64	d7, [r7, #24]
	ldrd	r2, [r7, #24]
	ldr	r1, .L9+32
.LPIC4:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	movs	r3, #0
	ldr	r1, .L9+36
.LPIC6:
	add	r1, pc
	ldr	r2, .L9+12
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #36]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L8
	bl	__stack_chk_fail(PLT)
.L8:
	mov	r0, r3
	adds	r7, r7, #40
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L10:
	.align	3
.L9:
	.word	0
	.word	1093567616
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC5+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC0+4)
	.word	.LC1-(.LPIC1+4)
	.word	.LC2-(.LPIC2+4)
	.word	.LC3-(.LPIC3+4)
	.word	.LC4-(.LPIC4+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC6+4)
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",%progbits
