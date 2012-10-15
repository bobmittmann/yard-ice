	.syntax unified
	.cpu cortex-m3
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 1
	.eabi_attribute 30, 1
	.eabi_attribute 18, 4
	.thumb
	.file	"stm32f-usart.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align	2
	.global	stm32f_usart_read
	.thumb
	.thumb_func
	.type	stm32f_usart_read, %function
stm32f_usart_read:
.LFB2:
	.file 1 "stm32f-usart.c"
	.loc 1 112 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL0:
	push	{r3, r4, r5, r6, r7, lr}
.LCFI0:
	.cfi_def_cfa_offset 24
	.cfi_offset 14, -4
	.cfi_offset 7, -8
	.cfi_offset 6, -12
	.cfi_offset 5, -16
	.cfi_offset 4, -20
	.cfi_offset 3, -24
	mov	r7, r0
.LVL1:
.LBB6:
.LBB7:
	.loc 1 100 0
	mov	r3, #20480
	movt	r3, 16384
	ldr	r3, [r3, #0]
	tst	r3, #32
	bne	.L2
	.loc 1 103 0
	lsls	r4, r2, #5
	.loc 1 106 0
	ittt	ne
	movne	r6, #31
	.loc 1 100 0
	movne	r5, #20480
	movtne	r5, 16384
	.loc 1 103 0
	bne	.L12
	b	.L14
.LVL2:
.L2:
	.loc 1 101 0
	mov	r3, #20480
	movt	r3, 16384
	ldr	r3, [r3, #4]
.LBE7:
.LBE6:
	.loc 1 119 0
	cmp	r3, #0
	bge	.L11
	b	.L15
.LVL3:
.L13:
.LBB11:
.LBB8:
	.loc 1 103 0
	subs	r4, r4, #1
.LVL4:
	beq	.L16
.LVL5:
.L12:
	.loc 1 106 0
	mov	r0, r6
	bl	udelay
.LVL6:
	.loc 1 100 0
	ldr	r3, [r5, #0]
	tst	r3, #32
	beq	.L13
	b	.L2
.LVL7:
.L11:
.LBE8:
.LBE11:
	.loc 1 122 0
	strb	r3, [r7, #0]
	.loc 1 124 0
	mov	r0, #1
	pop	{r3, r4, r5, r6, r7, pc}
.LVL8:
.L14:
.LBB12:
.LBB9:
	.loc 1 120 0
	mov	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
.LVL9:
.L15:
.LBE9:
.LBE12:
	mov	r0, #0
	pop	{r3, r4, r5, r6, r7, pc}
.LVL10:
.L16:
.LBB13:
.LBB10:
	mov	r0, #0
.LBE10:
.LBE13:
	.loc 1 125 0
	pop	{r3, r4, r5, r6, r7, pc}
	.cfi_endproc
.LFE2:
	.size	stm32f_usart_read, .-stm32f_usart_read
	.align	2
	.global	stm32f_usart_write
	.thumb
	.thumb_func
	.type	stm32f_usart_write, %function
stm32f_usart_write:
.LFB3:
	.loc 1 128 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL11:
	push	{r4, r5}
.LCFI1:
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -4
	.cfi_offset 4, -8
.LVL12:
	.loc 1 133 0
	cmp	r1, #0
	it	le
	movle	r0, #0
.LVL13:
	ble	.L18
	mov	r4, #0
.LBB14:
.LBB15:
	.loc 1 86 0
	mov	r2, #20480
	movt	r2, 16384
.LVL14:
.L20:
.LBE15:
.LBE14:
	.loc 1 134 0 discriminator 2
	ldrb	r5, [r0, r4]	@ zero_extendqisi2
.LVL15:
.L19:
.LBB17:
.LBB16:
	.loc 1 86 0
	ldr	r3, [r2, #0]
	tst	r3, #128
	beq	.L19
	.loc 1 88 0
	str	r5, [r2, #4]
.LBE16:
.LBE17:
	.loc 1 133 0
	add	r4, r4, #1
.LVL16:
	cmp	r4, r1
	bne	.L20
	mov	r0, r1
.LVL17:
.L18:
	.loc 1 138 0
	pop	{r4, r5}
	bx	lr
	.cfi_endproc
.LFE3:
	.size	stm32f_usart_write, .-stm32f_usart_write
	.align	2
	.global	stm32f_usart_ioctl
	.thumb
	.thumb_func
	.type	stm32f_usart_ioctl, %function
stm32f_usart_ioctl:
.LFB4:
	.loc 1 142 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
.LVL18:
	.loc 1 146 0
	mov	r2, #20480
	movt	r2, 16384
.L25:
	.loc 1 146 0 is_stmt 0 discriminator 1
	ldr	r3, [r2, #0]
	tst	r3, #128
	beq	.L25
	.loc 1 149 0 is_stmt 1
	mov	r3, #20480
	movt	r3, 16384
	ldr	r3, [r3, #4]
	.loc 1 153 0
	mov	r0, #0
.LVL19:
	bx	lr
	.cfi_endproc
.LFE4:
	.size	stm32f_usart_ioctl, .-stm32f_usart_ioctl
	.align	2
	.global	stm32f_usart_open
	.thumb
	.thumb_func
	.type	stm32f_usart_open, %function
stm32f_usart_open:
.LFB8:
	.loc 1 215 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	push	{r4, lr}
.LCFI2:
	.cfi_def_cfa_offset 8
	.cfi_offset 14, -4
	.cfi_offset 4, -8
.LVL20:
.LBB24:
.LBB25:
	.loc 1 209 0
	mov	r0, #2
	bl	stm32f_gpio_clock_en
	.loc 1 210 0
	mov	r4, #2048
	movt	r4, 16386
	mov	r0, r4
	mov	r1, #12
	mov	r2, #2
	mov	r3, #0
	bl	stm32f_gpio_mode
	.loc 1 211 0
	mov	r0, r4
	mov	r1, #12
	mov	r2, #8
	bl	stm32f_gpio_af
.LVL21:
.LBE25:
.LBE24:
.LBB26:
.LBB27:
	.loc 1 201 0
	mov	r0, #3
	bl	stm32f_gpio_clock_en
	.loc 1 202 0
	mov	r4, #3072
	movt	r4, 16386
	mov	r0, r4
	mov	r1, #2
	mov	r2, r1
	mov	r3, #8
	bl	stm32f_gpio_mode
	.loc 1 203 0
	mov	r0, r4
	mov	r1, #2
	mov	r2, #8
	bl	stm32f_gpio_af
.LBE27:
.LBE26:
	.loc 1 229 0
	mov	r3, #14336
	movt	r3, 16386
	ldr	r2, [r3, #64]
	orr	r2, r2, #1048576
	str	r2, [r3, #64]
	.loc 1 232 0
	mov	r3, #20480
	movt	r3, 16384
	mov	r2, #0
	str	r2, [r3, #12]
	.loc 1 234 0
	mov	r2, #20480
	movt	r2, 16384
.L28:
	.loc 1 234 0 is_stmt 0 discriminator 1
	ldr	r3, [r2, #0]
	tst	r3, #128
	beq	.L28
	.loc 1 238 0 is_stmt 1
	mov	r3, #20480
	movt	r3, 16384
	movw	r2, #8204
	str	r2, [r3, #12]
	.loc 1 239 0
	mov	r2, #0
	str	r2, [r3, #16]
	.loc 1 240 0
	str	r2, [r3, #20]
	.loc 1 241 0
	str	r2, [r3, #24]
.LVL22:
.LBB28:
.LBB29:
	.loc 1 172 0
	ldr	r2, [r3, #12]
	and	r2, r2, #32768
.LVL23:
	.loc 1 177 0
	cmp	r2, #0
	ite	ne
	movne	r2, #258
	moveq	r2, #260
.LVL24:
	mov	r3, #20480
	movt	r3, 16384
	str	r2, [r3, #8]
.LBE29:
.LBE28:
	.loc 1 245 0
	mov	r0, #86
	bl	udelay
	.loc 1 248 0
	mov	r0, #0
	pop	{r4, pc}
	.cfi_endproc
.LFE8:
	.size	stm32f_usart_open, .-stm32f_usart_open
	.align	2
	.global	stm32f_usart_close
	.thumb
	.thumb_func
	.type	stm32f_usart_close, %function
stm32f_usart_close:
.LFB9:
	.loc 1 252 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 0, uses_anonymous_args = 0
	@ link register save eliminated.
	.loc 1 254 0
	mov	r0, #0
	bx	lr
	.cfi_endproc
.LFE9:
	.size	stm32f_usart_close, .-stm32f_usart_close
	.weak	console_read
	.thumb_set console_read,stm32f_usart_read
	.weak	console_write
	.thumb_set console_write,stm32f_usart_write
	.weak	console_ioctl
	.thumb_set console_ioctl,stm32f_usart_ioctl
	.weak	console_flush
	.thumb_set console_flush,stm32f_usart_ioctl
	.weak	console_open
	.thumb_set console_open,stm32f_usart_open
	.weak	console_close
	.thumb_set console_close,stm32f_usart_close
.Letext0:
	.file 2 "/home/bob/devel/uboot/include/stdint.h"
	.file 3 "/home/bob/devel/uboot/include/stm32f/stm32f-usart.h"
	.file 4 "/home/bob/devel/uboot/include/stm32f/stm32f-gpio.h"
	.file 5 "/home/bob/devel/uboot/include/stm32f/stm32f-rcc.h"
	.file 6 "/home/bob/devel/uboot/include/arch/stm32f207.h"
	.file 7 "/home/bob/devel/uboot/include/sys/stm32f.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x74e
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF77
	.byte	0x1
	.4byte	.LASF78
	.4byte	.LASF79
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.byte	0x6
	.4byte	.LASF0
	.uleb128 0x2
	.byte	0x2
	.byte	0x5
	.4byte	.LASF1
	.uleb128 0x3
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x2
	.byte	0x8
	.byte	0x5
	.4byte	.LASF2
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF3
	.uleb128 0x4
	.4byte	.LASF5
	.byte	0x2
	.byte	0x19
	.4byte	0x53
	.uleb128 0x2
	.byte	0x2
	.byte	0x7
	.4byte	.LASF4
	.uleb128 0x4
	.4byte	.LASF6
	.byte	0x2
	.byte	0x1a
	.4byte	0x65
	.uleb128 0x2
	.byte	0x4
	.byte	0x7
	.4byte	.LASF7
	.uleb128 0x2
	.byte	0x8
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x5
	.4byte	0x5a
	.uleb128 0x6
	.4byte	.LASF10
	.byte	0x1c
	.byte	0x3
	.2byte	0x1a2
	.4byte	0xed
	.uleb128 0x7
	.ascii	"sr\000"
	.byte	0x3
	.2byte	0x1a3
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.ascii	"dr\000"
	.byte	0x3
	.2byte	0x1a4
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.ascii	"brr\000"
	.byte	0x3
	.2byte	0x1a5
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x7
	.ascii	"cr1\000"
	.byte	0x3
	.2byte	0x1a6
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x7
	.ascii	"cr2\000"
	.byte	0x3
	.2byte	0x1a7
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x7
	.ascii	"cr3\000"
	.byte	0x3
	.2byte	0x1a8
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x8
	.4byte	.LASF9
	.byte	0x3
	.2byte	0x1a9
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.byte	0
	.uleb128 0x9
	.4byte	.LASF11
	.byte	0x28
	.byte	0x4
	.byte	0x53
	.4byte	0x194
	.uleb128 0xa
	.4byte	.LASF12
	.byte	0x4
	.byte	0x54
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0xa
	.4byte	.LASF13
	.byte	0x4
	.byte	0x55
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0xa
	.4byte	.LASF14
	.byte	0x4
	.byte	0x56
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0xa
	.4byte	.LASF15
	.byte	0x4
	.byte	0x57
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0xb
	.ascii	"idr\000"
	.byte	0x4
	.byte	0x58
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0xb
	.ascii	"odr\000"
	.byte	0x4
	.byte	0x59
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0xa
	.4byte	.LASF16
	.byte	0x4
	.byte	0x5a
	.4byte	0x194
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0xa
	.4byte	.LASF17
	.byte	0x4
	.byte	0x5b
	.4byte	0x194
	.byte	0x2
	.byte	0x23
	.uleb128 0x1a
	.uleb128 0xa
	.4byte	.LASF18
	.byte	0x4
	.byte	0x5c
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0xa
	.4byte	.LASF19
	.byte	0x4
	.byte	0x5d
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0xa
	.4byte	.LASF20
	.byte	0x4
	.byte	0x5e
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.byte	0
	.uleb128 0x5
	.4byte	0x48
	.uleb128 0x6
	.4byte	.LASF21
	.byte	0x88
	.byte	0x5
	.2byte	0x310
	.4byte	0x36a
	.uleb128 0x7
	.ascii	"cr\000"
	.byte	0x5
	.2byte	0x311
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x8
	.4byte	.LASF22
	.byte	0x5
	.2byte	0x312
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x8
	.4byte	.LASF23
	.byte	0x5
	.2byte	0x313
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x7
	.ascii	"cir\000"
	.byte	0x5
	.2byte	0x314
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.uleb128 0x8
	.4byte	.LASF24
	.byte	0x5
	.2byte	0x315
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x10
	.uleb128 0x8
	.4byte	.LASF25
	.byte	0x5
	.2byte	0x316
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x14
	.uleb128 0x8
	.4byte	.LASF26
	.byte	0x5
	.2byte	0x317
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x18
	.uleb128 0x8
	.4byte	.LASF27
	.byte	0x5
	.2byte	0x318
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0x1c
	.uleb128 0x8
	.4byte	.LASF28
	.byte	0x5
	.2byte	0x319
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x20
	.uleb128 0x8
	.4byte	.LASF29
	.byte	0x5
	.2byte	0x31a
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x24
	.uleb128 0x8
	.4byte	.LASF30
	.byte	0x5
	.2byte	0x31b
	.4byte	0x36a
	.byte	0x2
	.byte	0x23
	.uleb128 0x28
	.uleb128 0x8
	.4byte	.LASF31
	.byte	0x5
	.2byte	0x31c
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x30
	.uleb128 0x8
	.4byte	.LASF32
	.byte	0x5
	.2byte	0x31d
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x34
	.uleb128 0x8
	.4byte	.LASF33
	.byte	0x5
	.2byte	0x31e
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x38
	.uleb128 0x8
	.4byte	.LASF34
	.byte	0x5
	.2byte	0x31f
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0x3c
	.uleb128 0x8
	.4byte	.LASF35
	.byte	0x5
	.2byte	0x320
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x40
	.uleb128 0x8
	.4byte	.LASF36
	.byte	0x5
	.2byte	0x321
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x44
	.uleb128 0x8
	.4byte	.LASF37
	.byte	0x5
	.2byte	0x322
	.4byte	0x36a
	.byte	0x2
	.byte	0x23
	.uleb128 0x48
	.uleb128 0x8
	.4byte	.LASF38
	.byte	0x5
	.2byte	0x323
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x50
	.uleb128 0x8
	.4byte	.LASF39
	.byte	0x5
	.2byte	0x324
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x54
	.uleb128 0x8
	.4byte	.LASF40
	.byte	0x5
	.2byte	0x325
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x58
	.uleb128 0x8
	.4byte	.LASF41
	.byte	0x5
	.2byte	0x326
	.4byte	0x5a
	.byte	0x2
	.byte	0x23
	.uleb128 0x5c
	.uleb128 0x8
	.4byte	.LASF42
	.byte	0x5
	.2byte	0x327
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x60
	.uleb128 0x8
	.4byte	.LASF43
	.byte	0x5
	.2byte	0x328
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x64
	.uleb128 0x8
	.4byte	.LASF44
	.byte	0x5
	.2byte	0x329
	.4byte	0x36a
	.byte	0x2
	.byte	0x23
	.uleb128 0x68
	.uleb128 0x8
	.4byte	.LASF45
	.byte	0x5
	.2byte	0x32a
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x70
	.uleb128 0x7
	.ascii	"csr\000"
	.byte	0x5
	.2byte	0x32b
	.4byte	0x73
	.byte	0x2
	.byte	0x23
	.uleb128 0x74
	.uleb128 0x8
	.4byte	.LASF46
	.byte	0x5
	.2byte	0x32c
	.4byte	0x36a
	.byte	0x2
	.byte	0x23
	.uleb128 0x78
	.uleb128 0x8
	.4byte	.LASF47
	.byte	0x5
	.2byte	0x32d
	.4byte	0x73
	.byte	0x3
	.byte	0x23
	.uleb128 0x80
	.uleb128 0x8
	.4byte	.LASF48
	.byte	0x5
	.2byte	0x32e
	.4byte	0x73
	.byte	0x3
	.byte	0x23
	.uleb128 0x84
	.byte	0
	.uleb128 0xc
	.4byte	0x5a
	.4byte	0x37a
	.uleb128 0xd
	.4byte	0x65
	.byte	0x1
	.byte	0
	.uleb128 0xe
	.byte	0x1
	.byte	0x6
	.byte	0x6a
	.4byte	0x3b9
	.uleb128 0xf
	.4byte	.LASF49
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF50
	.sleb128 1
	.uleb128 0xf
	.4byte	.LASF51
	.sleb128 2
	.uleb128 0xf
	.4byte	.LASF52
	.sleb128 3
	.uleb128 0xf
	.4byte	.LASF53
	.sleb128 4
	.uleb128 0xf
	.4byte	.LASF54
	.sleb128 5
	.uleb128 0xf
	.4byte	.LASF55
	.sleb128 6
	.uleb128 0xf
	.4byte	.LASF56
	.sleb128 7
	.uleb128 0xf
	.4byte	.LASF57
	.sleb128 8
	.byte	0
	.uleb128 0xe
	.byte	0x1
	.byte	0x7
	.byte	0x2b
	.4byte	0x3da
	.uleb128 0xf
	.4byte	.LASF58
	.sleb128 0
	.uleb128 0xf
	.4byte	.LASF59
	.sleb128 1
	.uleb128 0xf
	.4byte	.LASF60
	.sleb128 2
	.uleb128 0xf
	.4byte	.LASF61
	.sleb128 3
	.byte	0
	.uleb128 0x10
	.4byte	.LASF63
	.byte	0x1
	.byte	0x54
	.byte	0x1
	.4byte	0x33
	.byte	0x3
	.4byte	0x400
	.uleb128 0x11
	.4byte	.LASF62
	.byte	0x1
	.byte	0x54
	.4byte	0x400
	.uleb128 0x12
	.ascii	"c\000"
	.byte	0x1
	.byte	0x54
	.4byte	0x33
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.4byte	0x78
	.uleb128 0x10
	.4byte	.LASF64
	.byte	0x1
	.byte	0x5d
	.byte	0x1
	.4byte	0x33
	.byte	0x1
	.4byte	0x438
	.uleb128 0x11
	.4byte	.LASF62
	.byte	0x1
	.byte	0x5d
	.4byte	0x400
	.uleb128 0x11
	.4byte	.LASF65
	.byte	0x1
	.byte	0x5d
	.4byte	0x33
	.uleb128 0x14
	.ascii	"tm\000"
	.byte	0x1
	.byte	0x5f
	.4byte	0x33
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.4byte	.LASF67
	.byte	0x1
	.byte	0x6f
	.byte	0x1
	.4byte	0x33
	.4byte	.LFB2
	.4byte	.LFE2
	.4byte	.LLST0
	.4byte	0x4da
	.uleb128 0x16
	.ascii	"buf\000"
	.byte	0x1
	.byte	0x6f
	.4byte	0x4da
	.4byte	.LLST1
	.uleb128 0x16
	.ascii	"len\000"
	.byte	0x1
	.byte	0x6f
	.4byte	0x33
	.4byte	.LLST2
	.uleb128 0x17
	.4byte	.LASF65
	.byte	0x1
	.byte	0x6f
	.4byte	0x33
	.4byte	.LLST3
	.uleb128 0x18
	.4byte	.LASF62
	.byte	0x1
	.byte	0x71
	.4byte	0x400
	.4byte	0x40005000
	.uleb128 0x19
	.ascii	"cp\000"
	.byte	0x1
	.byte	0x72
	.4byte	0x4da
	.4byte	.LLST4
	.uleb128 0x14
	.ascii	"c\000"
	.byte	0x1
	.byte	0x73
	.4byte	0x33
	.uleb128 0x1a
	.4byte	0x406
	.4byte	.LBB6
	.4byte	.Ldebug_ranges0+0
	.byte	0x1
	.byte	0x75
	.uleb128 0x1b
	.4byte	0x422
	.4byte	.LLST5
	.uleb128 0x1c
	.4byte	0x417
	.4byte	0x40005000
	.uleb128 0x1d
	.4byte	.Ldebug_ranges0+0x28
	.uleb128 0x1e
	.4byte	0x42d
	.4byte	.LLST6
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.4byte	0x4e0
	.uleb128 0x2
	.byte	0x1
	.byte	0x8
	.4byte	.LASF66
	.uleb128 0x15
	.byte	0x1
	.4byte	.LASF68
	.byte	0x1
	.byte	0x7f
	.byte	0x1
	.4byte	0x33
	.4byte	.LFB3
	.4byte	.LFE3
	.4byte	.LLST7
	.4byte	0x56d
	.uleb128 0x16
	.ascii	"buf\000"
	.byte	0x1
	.byte	0x7f
	.4byte	0x56d
	.4byte	.LLST8
	.uleb128 0x1f
	.ascii	"len\000"
	.byte	0x1
	.byte	0x7f
	.4byte	0x33
	.byte	0x1
	.byte	0x51
	.uleb128 0x18
	.4byte	.LASF62
	.byte	0x1
	.byte	0x81
	.4byte	0x400
	.4byte	0x40005000
	.uleb128 0x19
	.ascii	"cp\000"
	.byte	0x1
	.byte	0x82
	.4byte	0x4da
	.4byte	.LLST9
	.uleb128 0x19
	.ascii	"n\000"
	.byte	0x1
	.byte	0x83
	.4byte	0x33
	.4byte	.LLST10
	.uleb128 0x1a
	.4byte	0x3da
	.4byte	.LBB14
	.4byte	.Ldebug_ranges0+0x50
	.byte	0x1
	.byte	0x86
	.uleb128 0x1b
	.4byte	0x3f6
	.4byte	.LLST11
	.uleb128 0x1b
	.4byte	0x3eb
	.4byte	.LLST12
	.byte	0
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.4byte	0x573
	.uleb128 0x20
	.uleb128 0x21
	.byte	0x1
	.4byte	.LASF69
	.byte	0x1
	.byte	0x8d
	.byte	0x1
	.4byte	0x33
	.4byte	.LFB4
	.4byte	.LFE4
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	0x5c5
	.uleb128 0x16
	.ascii	"req\000"
	.byte	0x1
	.byte	0x8d
	.4byte	0x33
	.4byte	.LLST13
	.uleb128 0x1f
	.ascii	"val\000"
	.byte	0x1
	.byte	0x8d
	.4byte	0x5c5
	.byte	0x1
	.byte	0x51
	.uleb128 0x18
	.4byte	.LASF62
	.byte	0x1
	.byte	0x8f
	.4byte	0x400
	.4byte	0x40005000
	.uleb128 0x14
	.ascii	"c\000"
	.byte	0x1
	.byte	0x90
	.4byte	0x33
	.byte	0
	.uleb128 0x22
	.byte	0x4
	.uleb128 0x23
	.4byte	.LASF73
	.byte	0x1
	.byte	0xce
	.byte	0x1
	.byte	0x1
	.4byte	0x600
	.uleb128 0x11
	.4byte	.LASF70
	.byte	0x1
	.byte	0xce
	.4byte	0x600
	.uleb128 0x11
	.4byte	.LASF71
	.byte	0x1
	.byte	0xcf
	.4byte	0x33
	.uleb128 0x11
	.4byte	.LASF72
	.byte	0x1
	.byte	0xcf
	.4byte	0x33
	.uleb128 0x12
	.ascii	"af\000"
	.byte	0x1
	.byte	0xcf
	.4byte	0x33
	.byte	0
	.uleb128 0x13
	.byte	0x4
	.4byte	0xed
	.uleb128 0x23
	.4byte	.LASF74
	.byte	0x1
	.byte	0xc6
	.byte	0x1
	.byte	0x1
	.4byte	0x63f
	.uleb128 0x11
	.4byte	.LASF70
	.byte	0x1
	.byte	0xc6
	.4byte	0x600
	.uleb128 0x11
	.4byte	.LASF71
	.byte	0x1
	.byte	0xc7
	.4byte	0x33
	.uleb128 0x11
	.4byte	.LASF72
	.byte	0x1
	.byte	0xc7
	.4byte	0x33
	.uleb128 0x12
	.ascii	"af\000"
	.byte	0x1
	.byte	0xc7
	.4byte	0x33
	.byte	0
	.uleb128 0x23
	.4byte	.LASF75
	.byte	0x1
	.byte	0x9e
	.byte	0x1
	.byte	0x1
	.4byte	0x675
	.uleb128 0x11
	.4byte	.LASF62
	.byte	0x1
	.byte	0x9e
	.4byte	0x400
	.uleb128 0x14
	.ascii	"div\000"
	.byte	0x1
	.byte	0xa0
	.4byte	0x5a
	.uleb128 0x14
	.ascii	"f\000"
	.byte	0x1
	.byte	0xa1
	.4byte	0x5a
	.uleb128 0x14
	.ascii	"m\000"
	.byte	0x1
	.byte	0xa2
	.4byte	0x5a
	.byte	0
	.uleb128 0x15
	.byte	0x1
	.4byte	.LASF76
	.byte	0x1
	.byte	0xd6
	.byte	0x1
	.4byte	0x33
	.4byte	.LFB8
	.4byte	.LFE8
	.4byte	.LLST14
	.4byte	0x739
	.uleb128 0x18
	.4byte	.LASF62
	.byte	0x1
	.byte	0xd8
	.4byte	0x400
	.4byte	0x40005000
	.uleb128 0x24
	.4byte	0x5c7
	.4byte	.LBB24
	.4byte	.LBE24
	.byte	0x1
	.byte	0xe1
	.4byte	0x6d0
	.uleb128 0x25
	.4byte	0x5f5
	.byte	0x8
	.uleb128 0x25
	.4byte	0x5ea
	.byte	0xc
	.uleb128 0x25
	.4byte	0x5df
	.byte	0x2
	.uleb128 0x1c
	.4byte	0x5d4
	.4byte	0x40020800
	.byte	0
	.uleb128 0x24
	.4byte	0x606
	.4byte	.LBB26
	.4byte	.LBE26
	.byte	0x1
	.byte	0xe2
	.4byte	0x6ff
	.uleb128 0x25
	.4byte	0x634
	.byte	0x8
	.uleb128 0x25
	.4byte	0x629
	.byte	0x2
	.uleb128 0x25
	.4byte	0x61e
	.byte	0x3
	.uleb128 0x1c
	.4byte	0x613
	.4byte	0x40020c00
	.byte	0
	.uleb128 0x26
	.4byte	0x63f
	.4byte	.LBB28
	.4byte	.LBE28
	.byte	0x1
	.byte	0xf3
	.uleb128 0x1c
	.4byte	0x64c
	.4byte	0x40005000
	.uleb128 0x27
	.4byte	.LBB29
	.4byte	.LBE29
	.uleb128 0x28
	.4byte	0x657
	.2byte	0x104
	.uleb128 0x1e
	.4byte	0x662
	.4byte	.LLST15
	.uleb128 0x29
	.4byte	0x66b
	.byte	0x10
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x2a
	.byte	0x1
	.4byte	.LASF80
	.byte	0x1
	.byte	0xfb
	.byte	0x1
	.4byte	0x33
	.4byte	.LFB9
	.4byte	.LFE9
	.byte	0x2
	.byte	0x7d
	.sleb128 0
	.byte	0
	.section	.debug_abbrev,"",%progbits
.Ldebug_abbrev0:
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x2
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.byte	0
	.byte	0
	.uleb128 0x3
	.uleb128 0x24
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0
	.byte	0
	.uleb128 0x4
	.uleb128 0x16
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0x35
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x6
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x7
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x8
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0x5
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x9
	.uleb128 0x13
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xa
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x4
	.byte	0x1
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x28
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1c
	.uleb128 0xd
	.byte	0
	.byte	0
	.uleb128 0x10
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x11
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x13
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x14
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x15
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x16
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x17
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x18
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x19
	.uleb128 0x34
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x52
	.uleb128 0x1
	.uleb128 0x55
	.uleb128 0x6
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x1b
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1d
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x55
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1e
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x1f
	.uleb128 0x5
	.byte	0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0x21
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x22
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x23
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x20
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x24
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x25
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x26
	.uleb128 0x1d
	.byte	0x1
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x58
	.uleb128 0xb
	.uleb128 0x59
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x27
	.uleb128 0xb
	.byte	0x1
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x28
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.byte	0
	.uleb128 0x29
	.uleb128 0x34
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x1c
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x2a
	.uleb128 0x2e
	.byte	0
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x40
	.uleb128 0xa
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB2-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0-.Ltext0
	.4byte	.LFE2-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 24
	.4byte	0
	.4byte	0
.LLST1:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL2-.Ltext0
	.4byte	.LFE2-.Ltext0
	.2byte	0x1
	.byte	0x57
	.4byte	0
	.4byte	0
.LLST2:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL8-.Ltext0
	.4byte	.LVL9-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
.LLST3:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL8-.Ltext0
	.4byte	.LVL9-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LLST4:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL2-.Ltext0
	.4byte	.LFE2-.Ltext0
	.2byte	0x1
	.byte	0x57
	.4byte	0
	.4byte	0
.LLST5:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL8-.Ltext0
	.4byte	.LVL9-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LLST6:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x5
	.byte	0x72
	.sleb128 0
	.byte	0x35
	.byte	0x24
	.byte	0x9f
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL4-.Ltext0
	.2byte	0x3
	.byte	0x74
	.sleb128 -1
	.byte	0x9f
	.4byte	.LVL4-.Ltext0
	.4byte	.LVL5-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL6-.Ltext0
	.4byte	.LVL7-.Ltext0
	.2byte	0x3
	.byte	0x74
	.sleb128 -1
	.byte	0x9f
	.4byte	.LVL8-.Ltext0
	.4byte	.LVL9-.Ltext0
	.2byte	0x5
	.byte	0x72
	.sleb128 0
	.byte	0x35
	.byte	0x24
	.byte	0x9f
	.4byte	.LVL10-.Ltext0
	.4byte	.LFE2-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	0
	.4byte	0
.LLST7:
	.4byte	.LFB3-.Ltext0
	.4byte	.LCFI1-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI1-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0
	.4byte	0
.LLST8:
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL13-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	0
	.4byte	0
.LLST9:
	.4byte	.LVL12-.Ltext0
	.4byte	.LVL13-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	0
	.4byte	0
.LLST10:
	.4byte	.LVL12-.Ltext0
	.4byte	.LVL14-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL16-.Ltext0
	.4byte	.LVL17-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	0
	.4byte	0
.LLST11:
	.4byte	.LVL15-.Ltext0
	.4byte	.LVL17-.Ltext0
	.2byte	0x1
	.byte	0x55
	.4byte	0
	.4byte	0
.LLST12:
	.4byte	.LVL15-.Ltext0
	.4byte	.LVL17-.Ltext0
	.2byte	0x6
	.byte	0xc
	.4byte	0x40005000
	.byte	0x9f
	.4byte	0
	.4byte	0
.LLST13:
	.4byte	.LVL18-.Ltext0
	.4byte	.LVL19-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	0
	.4byte	0
.LLST14:
	.4byte	.LFB8-.Ltext0
	.4byte	.LCFI2-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI2-.Ltext0
	.4byte	.LFE8-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 8
	.4byte	0
	.4byte	0
.LLST15:
	.4byte	.LVL22-.Ltext0
	.4byte	.LVL23-.Ltext0
	.2byte	0x2
	.byte	0x34
	.byte	0x9f
	.4byte	.LVL23-.Ltext0
	.4byte	.LVL24-.Ltext0
	.2byte	0xc
	.byte	0x32
	.byte	0x34
	.byte	0x72
	.sleb128 0
	.byte	0x30
	.byte	0x2e
	.byte	0x28
	.2byte	0x1
	.byte	0x16
	.byte	0x13
	.byte	0x9f
	.4byte	0
	.4byte	0
	.section	.debug_aranges,"",%progbits
	.4byte	0x1c
	.2byte	0x2
	.4byte	.Ldebug_info0
	.byte	0x4
	.byte	0
	.2byte	0
	.2byte	0
	.4byte	.Ltext0
	.4byte	.Letext0-.Ltext0
	.4byte	0
	.4byte	0
	.section	.debug_ranges,"",%progbits
.Ldebug_ranges0:
	.4byte	.LBB6-.Ltext0
	.4byte	.LBE6-.Ltext0
	.4byte	.LBB11-.Ltext0
	.4byte	.LBE11-.Ltext0
	.4byte	.LBB12-.Ltext0
	.4byte	.LBE12-.Ltext0
	.4byte	.LBB13-.Ltext0
	.4byte	.LBE13-.Ltext0
	.4byte	0
	.4byte	0
	.4byte	.LBB7-.Ltext0
	.4byte	.LBE7-.Ltext0
	.4byte	.LBB8-.Ltext0
	.4byte	.LBE8-.Ltext0
	.4byte	.LBB9-.Ltext0
	.4byte	.LBE9-.Ltext0
	.4byte	.LBB10-.Ltext0
	.4byte	.LBE10-.Ltext0
	.4byte	0
	.4byte	0
	.4byte	.LBB14-.Ltext0
	.4byte	.LBE14-.Ltext0
	.4byte	.LBB17-.Ltext0
	.4byte	.LBE17-.Ltext0
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF48:
	.ascii	"plli2scfgr\000"
.LASF73:
	.ascii	"io_txd_cfg\000"
.LASF62:
	.ascii	"usart\000"
.LASF1:
	.ascii	"short int\000"
.LASF52:
	.ascii	"STM32F_GPIOD_ID\000"
.LASF56:
	.ascii	"STM32F_GPIOH_ID\000"
.LASF33:
	.ascii	"ahb3enr\000"
.LASF58:
	.ascii	"INPUT\000"
.LASF47:
	.ascii	"sscgr\000"
.LASF40:
	.ascii	"ahb3lpenr\000"
.LASF63:
	.ascii	"stm32f_usart_putc\000"
.LASF18:
	.ascii	"lckr\000"
.LASF75:
	.ascii	"set_baudrate\000"
.LASF13:
	.ascii	"otyper\000"
.LASF39:
	.ascii	"hb2lpenr\000"
.LASF9:
	.ascii	"gtpr\000"
.LASF26:
	.ascii	"ahb3rstr\000"
.LASF19:
	.ascii	"afrl\000"
.LASF64:
	.ascii	"stm32f_usart_getc\000"
.LASF2:
	.ascii	"long long int\000"
.LASF49:
	.ascii	"STM32F_GPIOA_ID\000"
.LASF22:
	.ascii	"pllcfgr\000"
.LASF66:
	.ascii	"char\000"
.LASF14:
	.ascii	"ospeedr\000"
.LASF12:
	.ascii	"moder\000"
.LASF45:
	.ascii	"bdcr\000"
.LASF60:
	.ascii	"ALT_FUNC\000"
.LASF57:
	.ascii	"STM32F_GPIOI_ID\000"
.LASF68:
	.ascii	"stm32f_usart_write\000"
.LASF28:
	.ascii	"apb1rstr\000"
.LASF23:
	.ascii	"cfgr\000"
.LASF69:
	.ascii	"stm32f_usart_ioctl\000"
.LASF3:
	.ascii	"unsigned char\000"
.LASF70:
	.ascii	"gpio\000"
.LASF11:
	.ascii	"stm32f_gpio\000"
.LASF0:
	.ascii	"signed char\000"
.LASF8:
	.ascii	"long long unsigned int\000"
.LASF6:
	.ascii	"uint32_t\000"
.LASF7:
	.ascii	"unsigned int\000"
.LASF43:
	.ascii	"apb2lpenr\000"
.LASF5:
	.ascii	"uint16_t\000"
.LASF74:
	.ascii	"io_rxd_cfg\000"
.LASF35:
	.ascii	"apb1enr\000"
.LASF50:
	.ascii	"STM32F_GPIOB_ID\000"
.LASF24:
	.ascii	"ahb1rstr\000"
.LASF4:
	.ascii	"short unsigned int\000"
.LASF54:
	.ascii	"STM32F_GPIOF_ID\000"
.LASF31:
	.ascii	"ahb1enr\000"
.LASF61:
	.ascii	"ANALOG\000"
.LASF77:
	.ascii	"GNU C 4.6.2\000"
.LASF76:
	.ascii	"stm32f_usart_open\000"
.LASF38:
	.ascii	"ahb1lpenr\000"
.LASF59:
	.ascii	"OUTPUT\000"
.LASF67:
	.ascii	"stm32f_usart_read\000"
.LASF72:
	.ascii	"port\000"
.LASF80:
	.ascii	"stm32f_usart_close\000"
.LASF29:
	.ascii	"pb2rstr\000"
.LASF78:
	.ascii	"stm32f-usart.c\000"
.LASF15:
	.ascii	"pupdr\000"
.LASF36:
	.ascii	"apb2enr\000"
.LASF20:
	.ascii	"afrh\000"
.LASF51:
	.ascii	"STM32F_GPIOC_ID\000"
.LASF71:
	.ascii	"gpio_id\000"
.LASF17:
	.ascii	"bsrrh\000"
.LASF79:
	.ascii	"/home/bob/devel/uboot/src/stm32f\000"
.LASF16:
	.ascii	"bsrrl\000"
.LASF53:
	.ascii	"STM32F_GPIOE_ID\000"
.LASF55:
	.ascii	"STM32F_GPIOG_ID\000"
.LASF32:
	.ascii	"ahb2enr\000"
.LASF21:
	.ascii	"stm32f_rcc\000"
.LASF27:
	.ascii	"res0\000"
.LASF30:
	.ascii	"res1\000"
.LASF34:
	.ascii	"res2\000"
.LASF37:
	.ascii	"res3\000"
.LASF41:
	.ascii	"res4\000"
.LASF44:
	.ascii	"res5\000"
.LASF46:
	.ascii	"res6\000"
.LASF42:
	.ascii	"apb1lpenr\000"
.LASF25:
	.ascii	"ahb2rstr\000"
.LASF65:
	.ascii	"msec\000"
.LASF10:
	.ascii	"stm32f_usart\000"
	.ident	"GCC: (GNU) 4.6.2"
