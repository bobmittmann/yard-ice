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
	.file	"vfprintf.c"
	.text
.Ltext0:
	.cfi_sections	.debug_frame
	.align	2
	.global	vfprintf
	.thumb
	.thumb_func
	.type	vfprintf, %function
vfprintf:
.LFB3:
	.file 1 "vfprintf.c"
	.loc 1 87 0
	.cfi_startproc
	@ args = 0, pretend = 0, frame = 40
	@ frame_needed = 0, uses_anonymous_args = 0
.LVL0:
	push	{r4, r5, r6, r7, r8, r9, sl, fp, lr}
.LCFI0:
	.cfi_def_cfa_offset 36
	.cfi_offset 14, -4
	.cfi_offset 11, -8
	.cfi_offset 10, -12
	.cfi_offset 9, -16
	.cfi_offset 8, -20
	.cfi_offset 7, -24
	.cfi_offset 6, -28
	.cfi_offset 5, -32
	.cfi_offset 4, -36
	sub	sp, sp, #44
.LCFI1:
	.cfi_def_cfa_offset 80
	mov	r7, r0
	str	r2, [sp, #4]
.LVL1:
	.loc 1 106 0
	mov	r2, r1
.LVL2:
	mov	r5, r1
	.loc 1 102 0
	mov	r6, #0
	.loc 1 103 0
	mov	r8, r6
	.loc 1 104 0
	mov	r9, r6
	.loc 1 108 0
	mov	r4, r6
	.loc 1 267 0
	mov	fp, r6
	.loc 1 257 0
	movw	r3, #:lower16:.LANCHOR0
	movt	r3, #:upper16:.LANCHOR0
	str	r3, [sp, #16]
	add	r3, r3, #32
	str	r3, [sp, #20]
.LBB4:
.LBB5:
	.loc 1 60 0
	movw	sl, #8720
	movt	sl, 1091
.LBE5:
.LBE4:
	.loc 1 108 0
	b	.L34
.LVL3:
.L26:
	.loc 1 109 0
	cbnz	r4, .L3
	.loc 1 110 0
	cmp	r3, #37
	.loc 1 125 0
	it	ne
	addne	r6, r6, #1
.LVL4:
	.loc 1 110 0
	bne	.L34
.LVL5:
	.loc 1 111 0
	mov	r8, fp
	.loc 1 112 0
	mov	r4, #1
	.loc 1 114 0
	cmp	r6, #0
	beq	.L34
	.loc 1 115 0
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	ldr	r3, [r3, #0]
	mov	r1, r2
.LVL6:
	mov	r2, r6
.LVL7:
	blx	r3
.LVL8:
	.loc 1 116 0
	mov	r2, r5
.LVL9:
	.loc 1 117 0
	add	r9, r9, r6
.LVL10:
	.loc 1 118 0
	mov	r6, fp
	.loc 1 111 0
	mov	r8, fp
	.loc 1 112 0
	mov	r4, #1
	b	.L34
.LVL11:
.L3:
	.loc 1 134 0
	sub	r0, r3, #48
	cmp	r0, #9
	bhi	.L6
	.loc 1 135 0
	tst	r4, #2
	bne	.L7
	.loc 1 136 0
	orr	r4, r4, #2
.LVL12:
	.loc 1 137 0
	cmp	r3, #48
	.loc 1 138 0
	it	eq
	orreq	r4, r4, #4
.LVL13:
	.loc 1 137 0
	beq	.L34
.L7:
.LVL14:
	.loc 1 143 0
	add	r8, r8, r8, lsl #2
.LVL15:
	sub	r3, r3, #48
	add	r8, r3, r8, lsl #1
.LVL16:
	.loc 1 144 0
	b	.L34
.L6:
	.loc 1 148 0
	cmp	r3, #45
	.loc 1 149 0
	it	eq
	orreq	r4, r4, #8
.LVL17:
	.loc 1 148 0
	beq	.L34
	.loc 1 156 0
	cmp	r3, #43
	beq	.L34
	.loc 1 159 0
	cmp	r3, #99
	bne	.L9
.LVL18:
	.loc 1 160 0
	ldr	r2, [sp, #4]
.LVL19:
	ldr	r1, [r2, #0]
.LVL20:
	add	r2, r2, #4
	str	r2, [sp, #4]
	.loc 1 161 0
	b	.L10
.LVL21:
.L9:
	.loc 1 164 0
	cmp	r3, #100
	bne	.L11
	.loc 1 165 0
	ldr	r3, [sp, #4]
.LVL22:
	add	r1, r3, #4
.LVL23:
	ldr	r3, [r3, #0]
.LVL24:
	.loc 1 166 0
	cmp	r3, #0
	.loc 1 167 0
	itttt	lt
	movlt	r2, #45
	strblt	r2, [sp, #28]
	.loc 1 168 0
	rsblt	r3, r3, #0
.LVL25:
	.loc 1 169 0
	orrlt	r4, r4, #16
.LVL26:
	.loc 1 170 0
	it	lt
	addlt	r8, r8, #-1
.LVL27:
.L12:
	.loc 1 177 0
	mov	r6, #12
.LVL28:
.LBB9:
.LBB6:
	.loc 1 59 0
	mov	lr, #858993459
	.loc 1 63 0
	mvn	r0, #9
.LVL29:
.LBE6:
.LBE9:
	.loc 1 177 0
	cbnz	r3, .L13
.LVL30:
	.loc 1 178 0
	mov	r3, #48
.LVL31:
	strb	r3, [sp, #39]
	mov	r6, #11
	b	.L14
.LVL32:
.L13:
	.loc 1 181 0
	mov	r2, r3
.LVL33:
.LBB10:
.LBB7:
	.loc 1 59 0
	mul	ip, lr, r3
	add	ip, ip, r3, lsr #3
	lsr	ip, ip, #29
	.loc 1 60 0
	lsl	ip, ip, #2
	asr	ip, sl, ip
	and	ip, ip, #7
	rsb	r3, ip, r3
.LVL34:
	add	ip, r3, r3, lsl #1
	add	ip, ip, ip, lsl #4
	add	ip, ip, ip, lsl #8
	add	ip, ip, ip, lsl #16
	add	r3, r3, ip, lsl #2
.LVL35:
	.loc 1 62 0
	asr	r3, r3, #1
.LVL36:
.LBE7:
.LBE10:
	.loc 1 182 0
	add	r6, r6, #-1
.LVL37:
.LBB11:
.LBB8:
	.loc 1 63 0
	mla	r2, r0, r3, r2
.LVL38:
.LBE8:
.LBE11:
	.loc 1 182 0
	add	r2, r2, #48
	add	ip, sp, #28
	strb	r2, [r6, ip]
	.loc 1 180 0
	cmp	r3, #0
	bne	.L13
.LVL39:
.L14:
	.loc 1 186 0
	add	r2, sp, #28
	adds	r2, r2, r6
	str	r2, [sp, #12]
.LVL40:
	.loc 1 187 0
	rsb	r6, r6, #12
.LVL41:
	.loc 1 188 0
	str	r1, [sp, #4]
	b	.L15
.LVL42:
.L11:
	.loc 1 191 0
	cmp	r3, #120
	bne	.L16
.LVL43:
	.loc 1 192 0
	ldr	r3, [sp, #4]
	ldr	r1, [r3, #0]
.LVL44:
	add	r3, r3, #4
	str	r3, [sp, #4]
.LVL45:
.L17:
	.loc 1 196 0
	add	r0, sp, #28
.LVL46:
	bl	uint2hex
.LVL47:
	mov	r6, r0
.LVL48:
	.loc 1 197 0
	add	r2, sp, #28
	str	r2, [sp, #12]
.LVL49:
	.loc 1 198 0
	b	.L15
.LVL50:
.L16:
	.loc 1 201 0
	cmp	r3, #115
	bne	.L18
	.loc 1 202 0
	ldr	r3, [sp, #4]
	add	r3, r3, #4
	str	r3, [sp, #8]
.LVL51:
	ldr	r2, [sp, #4]
.LVL52:
	ldr	r2, [r2, #0]
	str	r2, [sp, #12]
.LVL53:
	.loc 1 203 0
	mov	r0, r2
	bl	strlen
.LVL54:
	mov	r6, r0
.LVL55:
	.loc 1 202 0
	ldr	r3, [sp, #8]
	str	r3, [sp, #4]
	.loc 1 204 0
	b	.L15
.LVL56:
.L18:
	.loc 1 208 0
	cmp	r3, #117
	bne	.L19
	.loc 1 209 0
	ldr	r2, [sp, #4]
.LVL57:
	add	r1, r2, #4
.LVL58:
	ldr	r3, [r2, #0]
.LVL59:
	.loc 1 210 0
	b	.L12
.LVL60:
.L19:
	.loc 1 215 0
	cmp	r3, #112
	.loc 1 224 0
	itt	ne
	movne	r8, fp
.LVL61:
	.loc 1 223 0
	movne	r4, fp
	.loc 1 215 0
	bne	.L10
.LVL62:
	.loc 1 216 0
	ldr	r3, [sp, #4]
	ldr	r1, [r3, #0]
.LVL63:
	.loc 1 218 0
	orr	r4, r4, #4
.LVL64:
	.loc 1 216 0
	add	r3, r3, #4
	str	r3, [sp, #4]
	.loc 1 217 0
	mov	r8, #8
	.loc 1 219 0
	b	.L17
.LVL65:
.L10:
	.loc 1 227 0
	add	r2, sp, #28
	str	r2, [sp, #12]
.LVL66:
	.loc 1 228 0
	strb	r1, [sp, #28]
.LVL67:
	.loc 1 229 0
	mov	r6, #1
.LVL68:
.L15:
	.loc 1 233 0
	cmp	r8, r6
	ite	le
	movle	r3, #0
	movgt	r3, #1
	str	r3, [sp, #8]
	tst	r4, #8
	ite	ne
	movne	r3, #0
	andeq	r3, r3, #1
	cbz	r3, .L20
	.loc 1 237 0
	tst	r4, #4
	beq	.L21
	.loc 1 238 0
	tst	r4, #16
	beq	.L22
	.loc 1 239 0
	bic	r4, r4, #16
.LVL69:
	.loc 1 240 0
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	ldr	r3, [r3, #0]
	add	r1, sp, #28
	mov	r2, #1
	blx	r3
.L22:
	.loc 1 242 0
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	rsb	r2, r6, r8
	ldr	r3, [r3, #0]
	ldr	r1, [sp, #16]
	blx	r3
	b	.L23
.L21:
	.loc 1 244 0
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	rsb	r2, r6, r8
	ldr	r3, [r3, #0]
	ldr	r1, [sp, #20]
	blx	r3
.L23:
	.loc 1 246 0
	rsb	r3, r6, r8
	add	r9, r9, r3
.LVL70:
.L20:
	.loc 1 249 0
	tst	r4, #16
	beq	.L24
	.loc 1 250 0
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	ldr	r3, [r3, #0]
	add	r1, sp, #28
	mov	r2, #1
	blx	r3
.L24:
	.loc 1 251 0
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	ldr	r3, [r3, #0]
	ldr	r1, [sp, #12]
	mov	r2, r6
	blx	r3
	.loc 1 252 0
	add	r9, r9, r6
.LVL71:
	.loc 1 256 0
	ldr	r2, [sp, #8]
	ands	r2, r2, r4, lsr #3
	beq	.L25
	.loc 1 257 0
	rsb	r6, r6, r8
.LVL72:
	ldr	r3, [r7, #4]
	ldr	r0, [r7, #0]
	ldr	r3, [r3, #0]
	ldr	r1, [sp, #20]
	mov	r2, r6
	blx	r3
	.loc 1 258 0
	add	r9, r9, r6
.LVL73:
.L25:
	.loc 1 266 0
	mov	r2, r5
.LVL74:
	.loc 1 267 0
	mov	r6, fp
	.loc 1 263 0
	mov	r8, fp
	.loc 1 262 0
	mov	r4, fp
.LVL75:
.L34:
	.loc 1 108 0 discriminator 1
	ldrb	r3, [r5], #1	@ zero_extendqisi2
	mov	r1, r3
.LVL76:
	cmp	r3, #0
	bne	.L26
	.loc 1 272 0
	cbz	r6, .L27
	.loc 1 273 0
	ldr	r3, [r7, #4]
.LVL77:
	ldr	r0, [r7, #0]
	ldr	r3, [r3, #0]
	mov	r1, r2
.LVL78:
	mov	r2, r6
.LVL79:
	blx	r3
.LVL80:
	.loc 1 274 0
	add	r9, r9, r6
.LVL81:
.L27:
	.loc 1 279 0
	mov	r0, r9
	add	sp, sp, #44
	pop	{r4, r5, r6, r7, r8, r9, sl, fp, pc}
	.cfi_endproc
.LFE3:
	.size	vfprintf, .-vfprintf
	.section	.rodata
	.align	2
	.set	.LANCHOR0,. + 0
	.type	zeros, %object
	.size	zeros, 32
zeros:
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.byte	48
	.type	blanks, %object
	.size	blanks, 64
blanks:
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.byte	32
	.text
.Letext0:
	.file 2 "/opt/arm-none-eabi/lib/gcc/arm-none-eabi/4.6.2/include/stdarg.h"
	.file 3 "/home/bob/devel/uboot/include/stdio.h"
	.file 4 "<built-in>"
	.file 5 "/home/bob/devel/uboot/include/sys/file.h"
	.file 6 "/home/bob/devel/uboot/include/stdlib.h"
	.section	.debug_info,"",%progbits
.Ldebug_info0:
	.4byte	0x3ae
	.2byte	0x2
	.4byte	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.4byte	.LASF28
	.byte	0x1
	.4byte	.LASF29
	.4byte	.LASF30
	.4byte	.Ltext0
	.4byte	.Letext0
	.4byte	.Ldebug_line0
	.uleb128 0x2
	.4byte	.LASF0
	.byte	0x2
	.byte	0x28
	.4byte	0x30
	.uleb128 0x3
	.4byte	.LASF3
	.byte	0x4
	.byte	0x4
	.byte	0
	.4byte	0x4a
	.uleb128 0x4
	.4byte	.LASF18
	.4byte	0x4a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.byte	0x1
	.byte	0
	.uleb128 0x5
	.byte	0x4
	.uleb128 0x2
	.4byte	.LASF1
	.byte	0x2
	.byte	0x66
	.4byte	0x25
	.uleb128 0x6
	.byte	0x4
	.byte	0x7
	.4byte	.LASF6
	.uleb128 0x2
	.4byte	.LASF2
	.byte	0x3
	.byte	0x16
	.4byte	0x69
	.uleb128 0x3
	.4byte	.LASF4
	.byte	0x8
	.byte	0x5
	.byte	0x1b
	.4byte	0x91
	.uleb128 0x7
	.4byte	.LASF5
	.byte	0x5
	.byte	0x1c
	.4byte	0x4a
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x8
	.ascii	"op\000"
	.byte	0x5
	.byte	0x1d
	.4byte	0x14f
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0
	.uleb128 0x6
	.byte	0x4
	.byte	0x5
	.4byte	.LASF7
	.uleb128 0x9
	.byte	0x4
	.byte	0x5
	.ascii	"int\000"
	.uleb128 0x6
	.byte	0x2
	.byte	0x7
	.4byte	.LASF8
	.uleb128 0x6
	.byte	0x1
	.byte	0x8
	.4byte	.LASF9
	.uleb128 0x3
	.4byte	.LASF10
	.byte	0x10
	.byte	0x5
	.byte	0x14
	.4byte	0xf2
	.uleb128 0x7
	.4byte	.LASF11
	.byte	0x5
	.byte	0x15
	.4byte	0x113
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x7
	.4byte	.LASF12
	.byte	0x5
	.byte	0x16
	.4byte	0x133
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.uleb128 0x7
	.4byte	.LASF13
	.byte	0x5
	.byte	0x17
	.4byte	0x149
	.byte	0x2
	.byte	0x23
	.uleb128 0x8
	.uleb128 0x7
	.4byte	.LASF14
	.byte	0x5
	.byte	0x18
	.4byte	0x149
	.byte	0x2
	.byte	0x23
	.uleb128 0xc
	.byte	0
	.uleb128 0xa
	.byte	0x1
	.4byte	0x98
	.4byte	0x10c
	.uleb128 0xb
	.4byte	0x4a
	.uleb128 0xb
	.4byte	0x10c
	.uleb128 0xb
	.4byte	0x98
	.byte	0
	.uleb128 0xc
	.byte	0x4
	.4byte	0x112
	.uleb128 0xd
	.uleb128 0xc
	.byte	0x4
	.4byte	0xf2
	.uleb128 0xa
	.byte	0x1
	.4byte	0x98
	.4byte	0x133
	.uleb128 0xb
	.4byte	0x4a
	.uleb128 0xb
	.4byte	0x4a
	.uleb128 0xb
	.4byte	0x98
	.byte	0
	.uleb128 0xc
	.byte	0x4
	.4byte	0x119
	.uleb128 0xa
	.byte	0x1
	.4byte	0x98
	.4byte	0x149
	.uleb128 0xb
	.4byte	0x4a
	.byte	0
	.uleb128 0xc
	.byte	0x4
	.4byte	0x139
	.uleb128 0xc
	.byte	0x4
	.4byte	0x155
	.uleb128 0xe
	.4byte	0xad
	.uleb128 0xf
	.byte	0x8
	.byte	0x6
	.byte	0x15
	.4byte	0x17f
	.uleb128 0x7
	.4byte	.LASF15
	.byte	0x6
	.byte	0x16
	.4byte	0x98
	.byte	0x2
	.byte	0x23
	.uleb128 0
	.uleb128 0x8
	.ascii	"rem\000"
	.byte	0x6
	.byte	0x17
	.4byte	0x98
	.byte	0x2
	.byte	0x23
	.uleb128 0x4
	.byte	0
	.uleb128 0x2
	.4byte	.LASF16
	.byte	0x6
	.byte	0x18
	.4byte	0x15a
	.uleb128 0x10
	.4byte	.LASF31
	.byte	0x1
	.byte	0x36
	.byte	0x1
	.4byte	0x17f
	.byte	0x3
	.4byte	0x1b7
	.uleb128 0x11
	.ascii	"n\000"
	.byte	0x1
	.byte	0x36
	.4byte	0x57
	.uleb128 0x12
	.ascii	"d\000"
	.byte	0x1
	.byte	0x38
	.4byte	0x17f
	.uleb128 0x12
	.ascii	"x\000"
	.byte	0x1
	.byte	0x39
	.4byte	0x98
	.byte	0
	.uleb128 0x13
	.byte	0x1
	.4byte	.LASF32
	.byte	0x1
	.byte	0x56
	.byte	0x1
	.4byte	0x98
	.4byte	.LFB3
	.4byte	.LFE3
	.4byte	.LLST0
	.4byte	0x2fd
	.uleb128 0x14
	.ascii	"f\000"
	.byte	0x1
	.byte	0x56
	.4byte	0x2fd
	.4byte	.LLST1
	.uleb128 0x14
	.ascii	"fmt\000"
	.byte	0x1
	.byte	0x56
	.4byte	0x303
	.4byte	.LLST2
	.uleb128 0x14
	.ascii	"ap\000"
	.byte	0x1
	.byte	0x56
	.4byte	0x4c
	.4byte	.LLST3
	.uleb128 0x15
	.ascii	"buf\000"
	.byte	0x1
	.byte	0x58
	.4byte	0x315
	.byte	0x2
	.byte	0x91
	.sleb128 -52
	.uleb128 0x16
	.4byte	.LASF17
	.byte	0x1
	.byte	0x59
	.4byte	0x98
	.4byte	.LLST4
	.uleb128 0x17
	.ascii	"cnt\000"
	.byte	0x1
	.byte	0x5a
	.4byte	0x98
	.4byte	.LLST5
	.uleb128 0x17
	.ascii	"i\000"
	.byte	0x1
	.byte	0x5b
	.4byte	0x98
	.4byte	.LLST6
	.uleb128 0x17
	.ascii	"c\000"
	.byte	0x1
	.byte	0x5c
	.4byte	0x98
	.4byte	.LLST7
	.uleb128 0x17
	.ascii	"w\000"
	.byte	0x1
	.byte	0x5d
	.4byte	0x98
	.4byte	.LLST8
	.uleb128 0x17
	.ascii	"n\000"
	.byte	0x1
	.byte	0x5e
	.4byte	0x98
	.4byte	.LLST9
	.uleb128 0x18
	.byte	0x4
	.byte	0x1
	.byte	0x5f
	.4byte	0x283
	.uleb128 0x19
	.ascii	"cp\000"
	.byte	0x1
	.byte	0x60
	.4byte	0x325
	.uleb128 0x19
	.ascii	"n\000"
	.byte	0x1
	.byte	0x61
	.4byte	0x57
	.uleb128 0x19
	.ascii	"i\000"
	.byte	0x1
	.byte	0x62
	.4byte	0x98
	.byte	0
	.uleb128 0x17
	.ascii	"val\000"
	.byte	0x1
	.byte	0x63
	.4byte	0x25e
	.4byte	.LLST10
	.uleb128 0x17
	.ascii	"d\000"
	.byte	0x1
	.byte	0x64
	.4byte	0x17f
	.4byte	.LLST11
	.uleb128 0x1a
	.4byte	.LASF19
	.byte	0x1
	.byte	0xe2
	.4byte	.L10
	.uleb128 0x1a
	.4byte	.LASF20
	.byte	0x1
	.byte	0xad
	.4byte	.L12
	.uleb128 0x1a
	.4byte	.LASF21
	.byte	0x1
	.byte	0xe7
	.4byte	.L15
	.uleb128 0x1a
	.4byte	.LASF22
	.byte	0x1
	.byte	0xc2
	.4byte	.L17
	.uleb128 0x1b
	.4byte	0x18a
	.4byte	.LBB4
	.4byte	.Ldebug_ranges0+0
	.byte	0x1
	.byte	0xb5
	.uleb128 0x1c
	.4byte	0x19b
	.4byte	.LLST12
	.uleb128 0x1d
	.4byte	.Ldebug_ranges0+0x28
	.uleb128 0x1e
	.4byte	0x1a4
	.4byte	.LLST13
	.uleb128 0x1e
	.4byte	0x1ad
	.4byte	.LLST14
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0xc
	.byte	0x4
	.4byte	0x69
	.uleb128 0xc
	.byte	0x4
	.4byte	0x309
	.uleb128 0xe
	.4byte	0x30e
	.uleb128 0x6
	.byte	0x1
	.byte	0x8
	.4byte	.LASF23
	.uleb128 0x1f
	.4byte	0x30e
	.4byte	0x325
	.uleb128 0x20
	.4byte	0x57
	.byte	0xb
	.byte	0
	.uleb128 0xc
	.byte	0x4
	.4byte	0x30e
	.uleb128 0x21
	.4byte	.LASF24
	.byte	0x3
	.byte	0x18
	.4byte	0x338
	.byte	0x1
	.byte	0x1
	.uleb128 0xc
	.byte	0x4
	.4byte	0x5e
	.uleb128 0x21
	.4byte	.LASF25
	.byte	0x3
	.byte	0x19
	.4byte	0x338
	.byte	0x1
	.byte	0x1
	.uleb128 0x1f
	.4byte	0x30e
	.4byte	0x35b
	.uleb128 0x20
	.4byte	0x57
	.byte	0x1f
	.byte	0
	.uleb128 0x22
	.4byte	.LASF26
	.byte	0x1
	.byte	0x22
	.4byte	0x36c
	.byte	0x5
	.byte	0x3
	.4byte	zeros
	.uleb128 0xe
	.4byte	0x34b
	.uleb128 0x1f
	.4byte	0x30e
	.4byte	0x381
	.uleb128 0x20
	.4byte	0x57
	.byte	0x3f
	.byte	0
	.uleb128 0x22
	.4byte	.LASF27
	.byte	0x1
	.byte	0x2a
	.4byte	0x392
	.byte	0x5
	.byte	0x3
	.4byte	blanks
	.uleb128 0xe
	.4byte	0x371
	.uleb128 0x21
	.4byte	.LASF24
	.byte	0x3
	.byte	0x18
	.4byte	0x338
	.byte	0x1
	.byte	0x1
	.uleb128 0x21
	.4byte	.LASF25
	.byte	0x3
	.byte	0x19
	.4byte	0x338
	.byte	0x1
	.byte	0x1
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
	.uleb128 0x3
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
	.uleb128 0x4
	.uleb128 0xd
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x38
	.uleb128 0xa
	.uleb128 0x34
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x5
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x6
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
	.uleb128 0x7
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
	.uleb128 0x8
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
	.uleb128 0x9
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
	.uleb128 0xa
	.uleb128 0x15
	.byte	0x1
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xb
	.uleb128 0x5
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xc
	.uleb128 0xf
	.byte	0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xd
	.uleb128 0x26
	.byte	0
	.byte	0
	.byte	0
	.uleb128 0xe
	.uleb128 0x26
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0xf
	.uleb128 0x13
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
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x12
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
	.uleb128 0x13
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
	.uleb128 0x14
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
	.uleb128 0x15
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
	.uleb128 0xa
	.byte	0
	.byte	0
	.uleb128 0x16
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
	.uleb128 0x2
	.uleb128 0x6
	.byte	0
	.byte	0
	.uleb128 0x17
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
	.uleb128 0x18
	.uleb128 0x17
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
	.uleb128 0x19
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
	.byte	0
	.byte	0
	.uleb128 0x1a
	.uleb128 0xa
	.byte	0
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x11
	.uleb128 0x1
	.byte	0
	.byte	0
	.uleb128 0x1b
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
	.uleb128 0x1c
	.uleb128 0x5
	.byte	0
	.uleb128 0x31
	.uleb128 0x13
	.uleb128 0x2
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
	.uleb128 0x1
	.byte	0x1
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x1
	.uleb128 0x13
	.byte	0
	.byte	0
	.uleb128 0x20
	.uleb128 0x21
	.byte	0
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2f
	.uleb128 0xb
	.byte	0
	.byte	0
	.uleb128 0x21
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
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3c
	.uleb128 0xc
	.byte	0
	.byte	0
	.uleb128 0x22
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
	.uleb128 0x2
	.uleb128 0xa
	.byte	0
	.byte	0
	.byte	0
	.section	.debug_loc,"",%progbits
.Ldebug_loc0:
.LLST0:
	.4byte	.LFB3-.Ltext0
	.4byte	.LCFI0-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 0
	.4byte	.LCFI0-.Ltext0
	.4byte	.LCFI1-.Ltext0
	.2byte	0x2
	.byte	0x7d
	.sleb128 36
	.4byte	.LCFI1-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x3
	.byte	0x7d
	.sleb128 80
	.4byte	0
	.4byte	0
.LLST1:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL3-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x1
	.byte	0x57
	.4byte	0
	.4byte	0
.LLST2:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
.LLST3:
	.4byte	.LVL0-.Ltext0
	.4byte	.LVL1-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL2-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL18-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL18-.Ltext0
	.4byte	.LVL21-.Ltext0
	.2byte	0x9
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x23
	.uleb128 0x4
	.byte	0x9f
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL21-.Ltext0
	.4byte	.LVL23-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL23-.Ltext0
	.4byte	.LVL42-.Ltext0
	.2byte	0x3
	.byte	0x51
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL42-.Ltext0
	.4byte	.LVL43-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL43-.Ltext0
	.4byte	.LVL45-.Ltext0
	.2byte	0x9
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x23
	.uleb128 0x4
	.byte	0x9f
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL45-.Ltext0
	.4byte	.LVL51-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL51-.Ltext0
	.4byte	.LVL54-1-.Ltext0
	.2byte	0x3
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL54-1-.Ltext0
	.4byte	.LVL56-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -72
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL56-.Ltext0
	.4byte	.LVL58-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL58-.Ltext0
	.4byte	.LVL60-.Ltext0
	.2byte	0x3
	.byte	0x51
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL60-.Ltext0
	.4byte	.LVL62-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL62-.Ltext0
	.4byte	.LVL65-.Ltext0
	.2byte	0x9
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x23
	.uleb128 0x4
	.byte	0x9f
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL65-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -76
	.byte	0x93
	.uleb128 0x4
	.4byte	0
	.4byte	0
.LLST4:
	.4byte	.LVL2-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL5-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL5-.Ltext0
	.4byte	.LVL11-.Ltext0
	.2byte	0x2
	.byte	0x31
	.byte	0x9f
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL13-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL14-.Ltext0
	.4byte	.LVL17-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL26-.Ltext0
	.4byte	.LVL42-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL45-.Ltext0
	.4byte	.LVL50-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL64-.Ltext0
	.4byte	.LVL73-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	.LVL73-.Ltext0
	.4byte	.LVL75-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL75-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x1
	.byte	0x54
	.4byte	0
	.4byte	0
.LLST5:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL3-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x1
	.byte	0x59
	.4byte	0
	.4byte	0
.LLST6:
	.4byte	.LVL27-.Ltext0
	.4byte	.LVL30-.Ltext0
	.2byte	0x2
	.byte	0x3c
	.byte	0x9f
	.4byte	.LVL30-.Ltext0
	.4byte	.LVL32-.Ltext0
	.2byte	0x2
	.byte	0x3b
	.byte	0x9f
	.4byte	.LVL37-.Ltext0
	.4byte	.LVL41-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	0
	.4byte	0
.LLST7:
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL6-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL20-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL20-.Ltext0
	.4byte	.LVL21-.Ltext0
	.2byte	0x4
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.4byte	.LVL21-.Ltext0
	.4byte	.LVL22-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL22-.Ltext0
	.4byte	.LVL23-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL23-.Ltext0
	.4byte	.LVL29-.Ltext0
	.2byte	0x3
	.byte	0x70
	.sleb128 48
	.byte	0x9f
	.4byte	.LVL42-.Ltext0
	.4byte	.LVL44-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL44-.Ltext0
	.4byte	.LVL46-.Ltext0
	.2byte	0x3
	.byte	0x70
	.sleb128 48
	.byte	0x9f
	.4byte	.LVL50-.Ltext0
	.4byte	.LVL54-1-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL56-.Ltext0
	.4byte	.LVL58-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL58-.Ltext0
	.4byte	.LVL59-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL59-.Ltext0
	.4byte	.LVL65-.Ltext0
	.2byte	0x3
	.byte	0x70
	.sleb128 48
	.byte	0x9f
	.4byte	.LVL65-.Ltext0
	.4byte	.LVL68-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	.LVL76-.Ltext0
	.4byte	.LVL77-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL77-.Ltext0
	.4byte	.LVL78-.Ltext0
	.2byte	0x1
	.byte	0x51
	.4byte	0
	.4byte	0
.LLST8:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL5-.Ltext0
	.2byte	0x1
	.byte	0x58
	.4byte	.LVL5-.Ltext0
	.4byte	.LVL11-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL15-.Ltext0
	.2byte	0x1
	.byte	0x58
	.4byte	.LVL16-.Ltext0
	.4byte	.LVL61-.Ltext0
	.2byte	0x1
	.byte	0x58
	.4byte	.LVL63-.Ltext0
	.4byte	.LVL65-.Ltext0
	.2byte	0x2
	.byte	0x38
	.byte	0x9f
	.4byte	.LVL65-.Ltext0
	.4byte	.LVL73-.Ltext0
	.2byte	0x1
	.byte	0x58
	.4byte	.LVL73-.Ltext0
	.4byte	.LVL75-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL75-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x1
	.byte	0x58
	.4byte	0
	.4byte	0
.LLST9:
	.4byte	.LVL1-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL4-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL10-.Ltext0
	.4byte	.LVL11-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL28-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL41-.Ltext0
	.4byte	.LVL48-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL48-.Ltext0
	.4byte	.LVL50-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL50-.Ltext0
	.4byte	.LVL55-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL55-.Ltext0
	.4byte	.LVL56-.Ltext0
	.2byte	0x1
	.byte	0x50
	.4byte	.LVL56-.Ltext0
	.4byte	.LVL67-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL67-.Ltext0
	.4byte	.LVL68-.Ltext0
	.2byte	0x2
	.byte	0x31
	.byte	0x9f
	.4byte	.LVL68-.Ltext0
	.4byte	.LVL72-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	.LVL74-.Ltext0
	.4byte	.LVL75-.Ltext0
	.2byte	0x2
	.byte	0x30
	.byte	0x9f
	.4byte	.LVL75-.Ltext0
	.4byte	.LFE3-.Ltext0
	.2byte	0x1
	.byte	0x56
	.4byte	0
	.4byte	0
.LLST10:
	.4byte	.LVL2-.Ltext0
	.4byte	.LVL3-.Ltext0
	.2byte	0x3
	.byte	0x51
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL3-.Ltext0
	.4byte	.LVL7-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL7-.Ltext0
	.4byte	.LVL8-1-.Ltext0
	.2byte	0x3
	.byte	0x51
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL9-.Ltext0
	.4byte	.LVL11-.Ltext0
	.2byte	0x3
	.byte	0x55
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL11-.Ltext0
	.4byte	.LVL19-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL21-.Ltext0
	.4byte	.LVL24-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL24-.Ltext0
	.4byte	.LVL25-.Ltext0
	.2byte	0x6
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL25-.Ltext0
	.4byte	.LVL31-.Ltext0
	.2byte	0x3
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL40-.Ltext0
	.4byte	.LVL44-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL44-.Ltext0
	.4byte	.LVL45-.Ltext0
	.2byte	0x6
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL45-.Ltext0
	.4byte	.LVL47-1-.Ltext0
	.2byte	0x3
	.byte	0x51
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL49-.Ltext0
	.4byte	.LVL52-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL53-.Ltext0
	.4byte	.LVL54-1-.Ltext0
	.2byte	0x6
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL54-1-.Ltext0
	.4byte	.LVL56-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -68
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL56-.Ltext0
	.4byte	.LVL57-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL59-.Ltext0
	.4byte	.LVL60-.Ltext0
	.2byte	0x6
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL60-.Ltext0
	.4byte	.LVL63-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL63-.Ltext0
	.4byte	.LVL65-.Ltext0
	.2byte	0x6
	.byte	0x91
	.sleb128 -76
	.byte	0x6
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL66-.Ltext0
	.4byte	.LVL68-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL68-.Ltext0
	.4byte	.LVL74-.Ltext0
	.2byte	0x5
	.byte	0x91
	.sleb128 -68
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL74-.Ltext0
	.4byte	.LVL75-.Ltext0
	.2byte	0x3
	.byte	0x55
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL75-.Ltext0
	.4byte	.LVL79-.Ltext0
	.2byte	0x3
	.byte	0x52
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL79-.Ltext0
	.4byte	.LVL80-1-.Ltext0
	.2byte	0x3
	.byte	0x51
	.byte	0x93
	.uleb128 0x4
	.4byte	0
	.4byte	0
.LLST11:
	.4byte	.LVL27-.Ltext0
	.4byte	.LVL31-.Ltext0
	.2byte	0x5
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL32-.Ltext0
	.4byte	.LVL34-.Ltext0
	.2byte	0x5
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL36-.Ltext0
	.4byte	.LVL38-.Ltext0
	.2byte	0xe
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.byte	0x73
	.sleb128 0
	.byte	0x9
	.byte	0xf6
	.byte	0x1e
	.byte	0x72
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL38-.Ltext0
	.4byte	.LVL39-.Ltext0
	.2byte	0x5
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.byte	0x93
	.uleb128 0x4
	.4byte	0
	.4byte	0
.LLST12:
	.4byte	.LVL33-.Ltext0
	.4byte	.LVL34-.Ltext0
	.2byte	0x1
	.byte	0x53
	.4byte	.LVL34-.Ltext0
	.4byte	.LVL38-.Ltext0
	.2byte	0x1
	.byte	0x52
	.4byte	0
	.4byte	0
.LLST13:
	.4byte	.LVL36-.Ltext0
	.4byte	.LVL38-.Ltext0
	.2byte	0xe
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.byte	0x73
	.sleb128 0
	.byte	0x9
	.byte	0xf6
	.byte	0x1e
	.byte	0x72
	.sleb128 0
	.byte	0x22
	.byte	0x9f
	.byte	0x93
	.uleb128 0x4
	.4byte	.LVL38-.Ltext0
	.4byte	.LVL39-.Ltext0
	.2byte	0x5
	.byte	0x53
	.byte	0x93
	.uleb128 0x4
	.byte	0x93
	.uleb128 0x4
	.4byte	0
	.4byte	0
.LLST14:
	.4byte	.LVL33-.Ltext0
	.4byte	.LVL34-.Ltext0
	.2byte	0x10
	.byte	0x73
	.sleb128 0
	.byte	0xc
	.4byte	0x33333333
	.byte	0x1e
	.byte	0x73
	.sleb128 0
	.byte	0x33
	.byte	0x25
	.byte	0x22
	.byte	0x4d
	.byte	0x25
	.byte	0x9f
	.4byte	.LVL34-.Ltext0
	.4byte	.LVL35-.Ltext0
	.2byte	0x10
	.byte	0x72
	.sleb128 0
	.byte	0xc
	.4byte	0x33333333
	.byte	0x1e
	.byte	0x72
	.sleb128 0
	.byte	0x33
	.byte	0x25
	.byte	0x22
	.byte	0x4d
	.byte	0x25
	.byte	0x9f
	.4byte	.LVL35-.Ltext0
	.4byte	.LVL36-.Ltext0
	.2byte	0x1
	.byte	0x53
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
	.4byte	.LBB4-.Ltext0
	.4byte	.LBE4-.Ltext0
	.4byte	.LBB9-.Ltext0
	.4byte	.LBE9-.Ltext0
	.4byte	.LBB10-.Ltext0
	.4byte	.LBE10-.Ltext0
	.4byte	.LBB11-.Ltext0
	.4byte	.LBE11-.Ltext0
	.4byte	0
	.4byte	0
	.4byte	.LBB5-.Ltext0
	.4byte	.LBE5-.Ltext0
	.4byte	.LBB6-.Ltext0
	.4byte	.LBE6-.Ltext0
	.4byte	.LBB7-.Ltext0
	.4byte	.LBE7-.Ltext0
	.4byte	.LBB8-.Ltext0
	.4byte	.LBE8-.Ltext0
	.4byte	0
	.4byte	0
	.section	.debug_line,"",%progbits
.Ldebug_line0:
	.section	.debug_str,"MS",%progbits,1
.LASF31:
	.ascii	"div10\000"
.LASF25:
	.ascii	"stdout\000"
.LASF10:
	.ascii	"fileop\000"
.LASF16:
	.ascii	"div_t\000"
.LASF20:
	.ascii	"decimal\000"
.LASF30:
	.ascii	"/home/bob/devel/uboot/src/libc\000"
.LASF2:
	.ascii	"FILE\000"
.LASF5:
	.ascii	"data\000"
.LASF0:
	.ascii	"__gnuc_va_list\000"
.LASF9:
	.ascii	"unsigned char\000"
.LASF22:
	.ascii	"hexadecimal\000"
.LASF15:
	.ascii	"quot\000"
.LASF8:
	.ascii	"short unsigned int\000"
.LASF24:
	.ascii	"stdin\000"
.LASF11:
	.ascii	"write\000"
.LASF1:
	.ascii	"va_list\000"
.LASF28:
	.ascii	"GNU C 4.6.2\000"
.LASF4:
	.ascii	"file\000"
.LASF29:
	.ascii	"vfprintf.c\000"
.LASF27:
	.ascii	"blanks\000"
.LASF12:
	.ascii	"read\000"
.LASF6:
	.ascii	"unsigned int\000"
.LASF17:
	.ascii	"flags\000"
.LASF23:
	.ascii	"char\000"
.LASF21:
	.ascii	"print_buf\000"
.LASF18:
	.ascii	"__ap\000"
.LASF3:
	.ascii	"__va_list\000"
.LASF13:
	.ascii	"flush\000"
.LASF32:
	.ascii	"vfprintf\000"
.LASF7:
	.ascii	"long int\000"
.LASF19:
	.ascii	"print_char\000"
.LASF14:
	.ascii	"close\000"
.LASF26:
	.ascii	"zeros\000"
	.ident	"GCC: (GNU) 4.6.2"
