thumb_func_start GAXOutput_render_asm

GAXOutput_render_asm:

	stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12}
	ldmia r0!,{r0,r1,r2,r3}
	mov   r3,r3, lsl #0x4
	add   r2,r2,r1
	mov   r10,#0x10000
	sub   r10,r10,#0x1
	mov   r11,#0xff
	mov   r12,r11, lsl #0x5

LAB_08049be0:

	ldmia r0!,{r6,r7,r8,r9}
	and   r5,r6,r10
	rsb   r5,r3,r5
	cmn   r5,#0xfe0
	mvnlt r5,#0xfe0
	cmp   r5,#0xfe0
	movgt r5,#0xfe0
	and   r5,r11,r5, asr #0x5
	rsb   r6,r3,r6, lsr #0x10
	cmn   r6,#0xfe0
	mvnlt r6,#0xfe0
	cmp   r6,#0xfe0
	movgt r6,#0xfe0
	and   r6,r12,r6
	orr   r6,r5,r6, lsl #0x3
	and   r5,r7,r10
	rsb   r5,r3,r5
	cmn   r5,#0xfe0
	mvnlt r5,#0xfe0
	cmp   r5,#0xfe0
	movgt r5,#0xfe0
	and   r5,r12,r5
	rsb   r7,r3,r7, lsr #0x10
	cmn   r7,#0xfe0
	mvnlt r7,#0xfe0
	cmp   r7,#0xfe0
	movgt r7,#0xfe0
	and   r7,r12,r7
	orr   r6,r6,r5, lsl #0xb
	orr   r6,r6,r7, lsl #0x13
	and   r5,r8,r10
	rsb   r5,r3,r5
	cmn   r5,#0xfe0
	mvnlt r5,#0xfe0
	cmp   r5,#0xfe0
	movgt r5,#0xfe0
	and   r5,r11,r5, asr #0x5
	rsb   r8,r3,r8, lsr #0x10
	cmn   r8,#0xfe0
	mvnlt r8,#0xfe0
	cmp   r8,#0xfe0
	movgt r8,#0xfe0
	and   r8,r12,r8
	orr   r8,r5,r8, lsl #0x3
	and   r5,r9,r10
	rsb   r5,r3,r5
	cmn   r5,#0xfe0
	mvnlt r5,#0xfe0
	cmp   r5,#0xfe0
	movgt r5,#0xfe0
	and   r5,r12,r5
	rsb   r9,r3,r9, lsr #0x10
	cmn   r9,#0xfe0
	mvnlt r9,#0xfe0
	cmp   r9,#0xfe0
	movgt r9,#0xfe0
	and   r9,r12,r9
	orr   r7,r8,r5, lsl #0xb
	orr   r7,r7,r9, lsl #0x13
	stmia r1!,{r6,r7}
	cmp   r1,r2
	blt   LAB_08049be0
	ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12}=>local_24
	bx    lr

LAB_08049ce0:

	stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11}
	ldr   r1,[r0,#0xc]
	ldr   r7,[r0,#0x8]
	ldr   r2,[r0,#0x4]
	ldr   r0,[r0,#0x0]
	adr   r4,0x8049da0
	ldr   r5,[r4,#0x4]=>DAT_08049da4
	ldr   r4,[r4,#0x0]=>DAT_08049da0
	mov   r8,#0x334
	mul   r7,r8,r7
	mov   r7,r7, asr #0x8
	mov   r9,#0x7f
	mov   r9,r9, lsl #0x8
	mul   r9,r1,r9

LAB_08049d18:
	 
	ldrsh r8,[r0,#0x0]
	mov   r8,r8, lsl #0x8
	sub   r6,r8,r5
	sub   r6,r6,r4
	cmn   r6,r9
	mvnlt r6,r9
	cmp   r6,r9
	movgt r6,r9
	mul   r10,r6,r7
	add   r5,r5,r10, asr #0x8
	cmn   r5,r9
	mvnlt r5,r9
	cmp   r5,r9
	movgt r5,r9
	mul   r10,r5,r7
	add   r4,r4,r10, asr #0x8
	cmn   r4,r9
	mvnlt r4,r9
	cmp   r4,r9
	movgt r4,r9
	mov   r10,r4, asr #0x8
	strh  r10,[r0,#0x0]
	add   r0,r0,#0x2
	subs  r2,r2,#0x2
	cmp   r2,#0x0
	bgt   LAB_08049d18
	adr   r0,DAT_08049da0
	str   r4,[r0,#0x0]=>DAT_08049da0
	str   r5,[r0,#0x4]=>DAT_08049da4
	ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11}
	bx    lr
	mov   r0,r0
	mov   r0,r0
	mov   r0,r0
	mov   r0,r0
	mov   r0,r0

