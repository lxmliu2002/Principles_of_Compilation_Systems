
main_gcc.o：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <main>:
   0:	f3 0f 1e fa          	endbr64 
   4:	55                   	push   %rbp
   5:	48 89 e5             	mov    %rsp,%rbp
   8:	48 83 ec 30          	sub    $0x30,%rsp
   c:	64 48 8b 04 25 28 00 	mov    %fs:0x28,%rax
  13:	00 00 
  15:	48 89 45 f8          	mov    %rax,-0x8(%rbp)
  19:	31 c0                	xor    %eax,%eax
  1b:	48 8d 45 d4          	lea    -0x2c(%rbp),%rax
  1f:	48 89 c6             	mov    %rax,%rsi
  22:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 29 <main+0x29>
  29:	48 89 c7             	mov    %rax,%rdi
  2c:	b8 00 00 00 00       	mov    $0x0,%eax
  31:	e8 00 00 00 00       	call   36 <main+0x36>
  36:	c7 45 d8 02 00 00 00 	movl   $0x2,-0x28(%rbp)
  3d:	c7 45 dc 01 00 00 00 	movl   $0x1,-0x24(%rbp)
  44:	e8 00 00 00 00       	call   49 <main+0x49>
  49:	48 89 45 e0          	mov    %rax,-0x20(%rbp)
  4d:	83 7d d8 00          	cmpl   $0x0,-0x28(%rbp)
  51:	7f 1f                	jg     72 <main+0x72>
  53:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 5a <main+0x5a>
  5a:	48 89 c7             	mov    %rax,%rdi
  5d:	e8 00 00 00 00       	call   62 <main+0x62>
  62:	eb 0e                	jmp    72 <main+0x72>
  64:	8b 45 dc             	mov    -0x24(%rbp),%eax
  67:	0f af 45 d8          	imul   -0x28(%rbp),%eax
  6b:	89 45 dc             	mov    %eax,-0x24(%rbp)
  6e:	83 45 d8 01          	addl   $0x1,-0x28(%rbp)
  72:	8b 45 d4             	mov    -0x2c(%rbp),%eax
  75:	39 45 d8             	cmp    %eax,-0x28(%rbp)
  78:	7e ea                	jle    64 <main+0x64>
  7a:	81 7d dc 10 27 00 00 	cmpl   $0x2710,-0x24(%rbp)
  81:	7e 11                	jle    94 <main+0x94>
  83:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # 8a <main+0x8a>
  8a:	48 89 c7             	mov    %rax,%rdi
  8d:	e8 00 00 00 00       	call   92 <main+0x92>
  92:	eb 19                	jmp    ad <main+0xad>
  94:	8b 45 dc             	mov    -0x24(%rbp),%eax
  97:	89 c6                	mov    %eax,%esi
  99:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # a0 <main+0xa0>
  a0:	48 89 c7             	mov    %rax,%rdi
  a3:	b8 00 00 00 00       	mov    $0x0,%eax
  a8:	e8 00 00 00 00       	call   ad <main+0xad>
  ad:	e8 00 00 00 00       	call   b2 <main+0xb2>
  b2:	48 89 45 e8          	mov    %rax,-0x18(%rbp)
  b6:	48 8b 45 e8          	mov    -0x18(%rbp),%rax
  ba:	48 2b 45 e0          	sub    -0x20(%rbp),%rax
  be:	66 0f ef c0          	pxor   %xmm0,%xmm0
  c2:	f2 48 0f 2a c0       	cvtsi2sd %rax,%xmm0
  c7:	f2 0f 10 0d 00 00 00 	movsd  0x0(%rip),%xmm1        # cf <main+0xcf>
  ce:	00 
  cf:	f2 0f 5e c1          	divsd  %xmm1,%xmm0
  d3:	f2 0f 11 45 f0       	movsd  %xmm0,-0x10(%rbp)
  d8:	48 8b 45 f0          	mov    -0x10(%rbp),%rax
  dc:	66 48 0f 6e c0       	movq   %rax,%xmm0
  e1:	48 8d 05 00 00 00 00 	lea    0x0(%rip),%rax        # e8 <main+0xe8>
  e8:	48 89 c7             	mov    %rax,%rdi
  eb:	b8 01 00 00 00       	mov    $0x1,%eax
  f0:	e8 00 00 00 00       	call   f5 <main+0xf5>
  f5:	b8 00 00 00 00       	mov    $0x0,%eax
  fa:	48 8b 55 f8          	mov    -0x8(%rbp),%rdx
  fe:	64 48 2b 14 25 28 00 	sub    %fs:0x28,%rdx
 105:	00 00 
 107:	74 05                	je     10e <main+0x10e>
 109:	e8 00 00 00 00       	call   10e <main+0x10e>
 10e:	c9                   	leave  
 10f:	c3                   	ret    
