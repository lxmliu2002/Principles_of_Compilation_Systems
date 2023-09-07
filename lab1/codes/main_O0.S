	.text
	.file	"main.c"
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3                               # -- Begin function main
.LCPI0_0:
	.quad	0x412e848000000000              # double 1.0E+6
	.text
	.globl	main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	$0, -44(%rbp)
	movabsq	$.L.str, %rdi
	leaq	-12(%rbp), %rsi
	movb	$0, %al
	callq	__isoc99_scanf@PLT
	movl	$2, -4(%rbp)
	movl	$1, -8(%rbp)
	callq	clock@PLT
	movq	%rax, -40(%rbp)
	cmpl	$1, -4(%rbp)
	jge	.LBB0_2
# %bb.1:
	movabsq	$.L.str.1, %rdi
	movb	$0, %al
	callq	printf@PLT
.LBB0_2:
	jmp	.LBB0_3
.LBB0_3:                                # =>This Inner Loop Header: Depth=1
	movl	-4(%rbp), %eax
	cmpl	-12(%rbp), %eax
	jg	.LBB0_5
# %bb.4:                                #   in Loop: Header=BB0_3 Depth=1
	movl	-8(%rbp), %eax
	imull	-4(%rbp), %eax
	movl	%eax, -8(%rbp)
	movl	-4(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.LBB0_3
.LBB0_5:
	cmpl	$10000, -8(%rbp)                # imm = 0x2710
	jle	.LBB0_7
# %bb.6:
	movabsq	$.L.str.2, %rdi
	movb	$0, %al
	callq	printf@PLT
	jmp	.LBB0_8
.LBB0_7:
	movl	-8(%rbp), %esi
	movabsq	$.L.str.3, %rdi
	movb	$0, %al
	callq	printf@PLT
.LBB0_8:
	callq	clock@PLT
	movq	%rax, -32(%rbp)
	movq	-32(%rbp), %rax
	subq	-40(%rbp), %rax
	cvtsi2sd	%rax, %xmm0
	movsd	.LCPI0_0(%rip), %xmm1           # xmm1 = mem[0],zero
	divsd	%xmm1, %xmm0
	movsd	%xmm0, -24(%rbp)
	movsd	-24(%rbp), %xmm0                # xmm0 = mem[0],zero
	movabsq	$.L.str.4, %rdi
	movb	$1, %al
	callq	printf@PLT
	xorl	%eax, %eax
	addq	$48, %rsp
	popq	%rbp
	.cfi_def_cfa %rsp, 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L.str,@object                  # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"%d"
	.size	.L.str, 3

	.type	.L.str.1,@object                # @.str.1
.L.str.1:
	.asciz	"i\345\260\217\344\272\2161\n"
	.size	.L.str.1, 10

	.type	.L.str.2,@object                # @.str.2
.L.str.2:
	.asciz	"\350\276\223\345\207\272\350\266\205\351\231\220\n"
	.size	.L.str.2, 14

	.type	.L.str.3,@object                # @.str.3
.L.str.3:
	.asciz	"%d\n"
	.size	.L.str.3, 4

	.type	.L.str.4,@object                # @.str.4
.L.str.4:
	.asciz	"\347\250\213\345\272\217\350\277\220\350\241\214\346\227\266\351\227\264\344\270\272\357\274\232%f\n"
	.size	.L.str.4, 28

	.ident	"Ubuntu clang version 14.0.0-1ubuntu1.1"
	.section	".note.GNU-stack","",@progbits
