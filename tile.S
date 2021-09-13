_fill_16:
        /* arg r0: vram address */
        /* arg r1: palette index */

        /* 6.1.4: 0x20 bytes per tile */
        orr r1, r1, r1, LSL #4
        orr r1, r1, r1, LSL #8
        orr r1, r1, r1, LSL #16
        .irp i, 2, 3, 4, 5, 6, 7, 8
        mov r\i, r1
        .endr
        stmia r0!, {r1 - r8}

        bx lr

        .global tile$basic
tile$basic:
        /* arg r0: vram base address */
        push {r4-r8, r10, r11, lr}

        mov r10, #0
        mov r11, #16

_loop:
        mov r1, r10
        bl _fill_16 /* tile n: color n */
        add r10, r10, #1
        cmp r11, r10
        bne _loop

        pop {r4-r8, r10, r11, lr}
        bx lr
