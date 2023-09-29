  .arch armv7-a
  .data
  .align 2
  .data
  .align 2
str:
  .ascii " result:%d\n"
  .text
  .align 2
  .global	__aeabi_idiv
  .global main
  .type main, %function
main:
  push {fp, lr}
  mov r0, #2
  push {r0}
  mov r0, #3
  push {r0}
  mov r0, #6
  push {r0}
  mov r0, #2
  push {r0}
  pop {r0, r1}
  add r0, r0, r1
  push {r0}
  pop {r0, r1}
  mul r0, r0, r1
  push {r0}
  mov r0, #3
  push {r0}
  pop {r1}
  pop {r0}
  bl __aeabi_idiv
  push {r0}
  pop {r0, r1}
  add r0, r0, r1
  push {r0}
  pop {r1}
  ldr r0, =str
  bl printf
  mov r0, #0
  pop {fp, pc}

.extern printf
.section .note.GNU-stack,"",%progbits
