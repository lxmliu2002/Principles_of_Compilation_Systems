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

	.text
	.global	pi
	.section	.rodata
	.align	2
	.type	pi, %object
	.size	pi, 4
pi:
	.word	1078523331
	.align	2
.LC0:
	.ascii	"perimeter: %.2f\012\000"
	.align	2
.LC1:
	.ascii	"area: %.2f\012\000"
	.text
	.align	1
	.global	main
	.syntax unified
	.thumb
	.thumb_func
	.type	main, %function
main:
	push	{r7, lr}
	sub	sp, sp, #8
	add	r7, sp, #0
	mov	r3, #1073741824
	str	r3, [r7, #4]	@ float
	vldr.32	s14, .L3
	vldr.32	s15, [r7, #4]
	vmul.f32	s15, s14, s15
	vcvt.f64.f32	d7, s15
	vmov	r2, r3, d7
	ldr	r1, .L3+4
.LPIC0:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	vldr.32	s14, .L3
	vldr.32	s15, [r7, #4]
	vmul.f32	s14, s14, s15
	vldr.32	s15, [r7, #4]
	vmul.f32	s15, s14, s15
	vcvt.f64.f32	d7, s15
	vmov.f64	d6, #4.0e+0
	vdiv.f64	d5, d7, d6
	vmov	r2, r3, d5
	ldr	r1, .L3+8
.LPIC1:
	add	r1, pc
	mov	r0, r1
	bl	printf(PLT)
	movs	r3, #0
	mov	r0, r3
	adds	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L4:
	.align	2
.L3:
	.word	1078523331
	.word	.LC0-(.LPIC0+4)
	.word	.LC1-(.LPIC1+4)
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",%progbits
