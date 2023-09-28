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

@ 数据段
.data
.align 1
pi:
    .float 3.14
D:
    .float 2.0
four:
    .float 4.0

perimeter_format:
    .asciz "perimeter: %.2f\n"
area_format:
    .asciz "area: %.2f\n"

@ 代码段
.text
.align 1
.global main
.type main, %function

main:
    @ 保存返回地址
    push {fp, lr}
    @ 加载pi和D的值
    ldr r0, =pi
    ldr r1, =D
    vldr.32 s0, [r0]   @ 将pi加载到s0
    vldr.32 s1, [r1]   @ 将D加载到s1
    @ 计算周长
    vmul.f32 s2, s0, s1 @ s2 = s0 * s1
    @ 输出周长
    ldr r0, =perimeter_format   @ 准备printf的参数r0
    vcvt.f64.f32 d7, s2
    vmov r2, r3, d7
    bl printf
    @ 加载pi和D的值
    ldr r0, =pi
    ldr r1, =D
    vldr.32 s0, [r0]   @ 将pi加载到s0
    vldr.32 s1, [r1]   @ 将D加载到s1
    @ 计算面积
    vmul.f32 s3, s1, s1   @ 计算D*D
    vmul.f32 s3, s3, s0   @ 计算pi*D*D
    ldr r0, =four   @ 加载4.0的地址到r0中 
    vldr s4, [r0]   @ 将4.0加载到s4中 
    vdiv.f32 s3, s3, s4   @ 计算pi*D*D/4
    @ 输出面积
    ldr r0, =area_format   @ 准备printf的参数r0
    vcvt.f64.f32 d7, s3
    vmov r2, r3, d7
    bl printf

    mov r0, #0   @ 设置返回值为0
    @ 恢复堆栈并返回
    pop {fp, pc}

@ 函数原型
.extern printf

.section .note.GNU-stack,"",%progbits

