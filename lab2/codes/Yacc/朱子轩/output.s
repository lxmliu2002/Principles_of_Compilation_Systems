.arch armv7-a
.align 2
.section    .rodata
a:  .ascii "num: %d\n"
.align 2
.text
.global main
main:
push {fp,lr}
mov r0, #1
push {r0}
mov r0, #2
push {r0}
mov r0, #4
push {r0}
pop {r1}
pop {r0}
bl __aeabi_idiv
push {r0}
pop {r0, r1}
add r0, r1, r0
push {r0}
pop {r1}
ldr r0, =a
bl printf
mov r0, #0
mov r0, #1
push {r0}
mov r0, #2
push {r0}
mov r0, #4
push {r0}
pop {r1}
pop {r0}
bl __aeabi_idiv
push {r0}
pop {r0, r1}
add r0, r1, r0
push {r0}
pop {r1}
ldr r0, =a
bl printf
mov r0, #0
mov r0, #1
push {r0}
mov r0, #2
push {r0}
pop {r0, r1}
add r0, r1, r0
push {r0}
mov r0, #4
push {r0}
pop {r1}
pop {r0}
bl __aeabi_idiv
push {r0}
pop {r1}
ldr r0, =a
bl printf
mov r0, #0
pop {fp,pc}
.section .note.GNU-stack,"",%progbits
