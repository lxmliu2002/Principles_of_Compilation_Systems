  .arch armv7-a
  .text
  .global n
  .section  .rodata
  .align 2
n:
  .word  5
  .align 2
_str0:
  .ascii "f[5] = %d\n"
  .align 2
  .text
  .global main
  .type  main, %function
main:
  push {fp, lr}
  mov fp, sp
  add r7, sp, #0
  @ 初始化数组和 i
  sub sp, sp, #24
  add r3, sp, #4
  mov r4, #1
  str r4, [fp, #-20]
  str r4, [fp, #-16]
  mov r4, #0 
  str r4, [fp, #-12]
  str r4, [fp, #-8]
  str r4, [fp, #-4]
  ldr r4, [fp, #-24]
  ldr r6, _bridge
  ldr r6, [r6]
.L0:  @ while循环
  cmp r6, r4 @ r6 为 n ，r4 为 i
  blt .L3
.L1:  @ if
  cmp r4, #1
  bgt .L2
  add r4, r4, #1
  b  .L1
.L2:  @ else
  sub r0, r4, #1
  lsls r0, r0, #2
  add r0, r3
  ldr r1, [r0] @ r1 = f[i-1]
  sub r0, r4, #2
  lsls r0, r0, #2
  add r0, r3
  ldr r2, [r0] @ r2 = f[i-1]
  add r1, r2 @ r1 = r1 + r2
  mov r0, r4
  lsls r0, r0, #2
  add r0, r3
  str r1, [r0]
  add r4, r4, #1
  b .L0
.L3:  @ 输出结果
  ldr r0, _bridge+4
  ldr r1, [sp, #20]
  bl printf
  mov sp, r7
  mov r0, #0
  pop {fp, pc}
.L4:
  .align 2
_bridge:
  .word  n
  .word  _str0
  .section  .note.GNU-stack,"",%progbits
