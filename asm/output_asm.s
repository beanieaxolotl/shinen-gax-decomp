GAXOutput_render_asm:

	push  {r4, r5, r6, r7, r8, sb, sl, fp, ip}
	ldmia r0!, {r0, r1, r2, r3}
	lsl   r3, r3, #4
	add   r2, r2, r1
	mov   sl, #0x10000
	sub   sl, sl, #1
	mov   fp, #0xff
	lsl   ip, fp, #5

	LAB_00010020:

		ldmia r0!, {r6, r7, r8, sb}
		and   r5, r6, sl
		rsb   r5, r3, r5

		cmn   r5, #0xfe0
		mvnlt r5, #0xfe0
		cmp   r5, #0xfe0
		movgt r5, #0xfe0
		and   r5, fp, r5, asr #5
		rsb   r6, r3, r6, lsr #16

		cmn   r6, #0xfe0
		mvnlt r6, #0xfe0
		cmp   r6, #0xfe0
		movgt r6, #0xfe0
		and   r6, ip, r6
		orr   r6, r5, r6, lsl #3
		and   r5, r7, sl
		rsb   r5, r3, r5

		cmn   r5, #0xfe0
		mvnlt r5, #0xfe0
		cmp   r5, #0xfe0
		movgt r5, #0xfe0
		and   r5, ip, r5
		rsb   r7, r3, r7, lsr #16

		cmn   r7, #0xfe0
		mvnlt r7, #0xfe0
		cmp   r7, #0xfe0
		movgt r7, #0xfe0
		and   r7, ip, r7
		orr   r6, r6, r5, lsl #11
		orr   r6, r6, r7, lsl #19
		and   r5, r8, sl
		rsb   r5, r3, r5

		cmn   r5, #0xfe0
		mvnlt r5, #0xfe0
		cmp   r5, #0xfe0
		movgt r5, #0xfe0
		and   r5, fp, r5, asr #5
		rsb   r8, r3, r8, lsr #16

		cmn   r8, #0xfe0
		mvnlt r8, #0xfe0
		cmp   r8, #0xfe0
		movgt r8, #0xfe0
		and   r8, ip, r8
		orr   r8, r5, r8, lsl #3
		and   r5, sb, sl
		rsb   r5, r3, r5

		cmn   r5, #0xfe0
		mvnlt r5, #0xfe0
		cmp   r5, #0xfe0
		movgt r5, #0xfe0
		and   r5, ip, r5
		rsb   sb, r3, sb, lsr #16

		cmn   sb, #0xfe0
		mvnlt sb, #0xfe0
		cmp   sb, #0xfe0
		movgt sb, #0xfe0
		and   sb, ip, sb
		orr   r7, r8, r5, lsl #11
		orr   r7, r7, sb, lsl #19
		
		stmia r1!, {r6, r7}
		cmp   r1, r2
		blt   LAB_00010020
		pop   {r4, r5, r6, r7, r8, sb, sl, fp, ip}
		bx    lr


GAXOutput_filter_lowpass_asm:

	GAXOutput_render_asm_end:

		push {r4, r5, r6, r7, r8, sb, sl, fp}
		ldr  r1, [r0, #0xc]
		ldr  r7, [r0, #8]
		ldr  r2, [r0, #4]
		ldr  r0, [r0]
		add  r4, pc, #0xa4
		ldr  r5, [r4, #4]
		ldr  r4, [r4]
		mov  r8, #0x334
		mul  r7, r8, r7
		asr  r7, r7, #8
		mov  sb, #0x7f
		lsl  sb, sb, #8
		mul  sb, r1, sb

		LAB_00010158:

			ldrsh r8, [r0]
			lsl   r8, r8, #8
			sub   r6, r8, r5
			sub   r6, r6, r4

			cmn   r6, sb
			mvnlt r6, sb
			cmp   r6, sb
			cpygt r6, sb
			mul   sl, r6, r7
			add   r5, r5, sl, asr #8

			cmn   r5, sb
			mvnlt r5, sb
			cmp   r5, sb
			cpygt r5, sb
			mul   sl, r5, r7
			add   r4, r4, sl, asr #8

			cmn   r4, sb
			mvnlt r4, sb
			cmp   r4, sb
			cpygt r4, sb
			asr   sl, r4, #8
			strh  sl, [r0]
			add   r0, r0, #2
			subs  r2, r2, #2
			cmp   r2, #0
			bgt   LAB_00010158
			add   r0, pc, #0x18
			str   r4, [r0]
			str   r5, [r0, #4]
			pop   {r4, r5, r6, r7, r8, sb, sl, fp}
			bx    lr

.word 0, 0, 0
vars:
    .word 0
DAT_000101e4:
    .word 0
.word 0x544c4946 @ "FILT"


GAXOutput_reverb_asm:

	GAXOutput_filter_lowpass_asm_end:

		push {r4, r5, r6, r7, r8, sb, sl}
		ldr  r5, [pc, #0xe8]
		ldr  r4, [r0]
		ldr  r3, [r0, #0xc]
		ldr  r2, [r0, #0x10]
		ldr  r1, [r0, #8]
		ldr  sb, [r0, #0x14]
		lsl  sb, sb, #4
		ldr  sl, [r0, #0x18]
		lsl  sl, sl, #4
		ldr  r0, [r0, #4]
		add  r4, r4, r0

		LAB_0001021c:

			cmp   r5, r3
			moveq r5, #0
			add   r7, r5, r3
			ldr   r8, [r2, #4]
			sub   r7, r7, r8
			cmp   r7, r3
			subge r7, r7, r3
			ldrsh r7, [r1, r7]
			ldr   r8, [r2, #0x10]
			muls  r7, r8, r7
			asr   r6, r7, #4
			ldr   r8, [r2, #8]
			cmp   r8, #0
			beq   LAB_0001029c
			add   r7, r5, r3
			sub   r7, r7, r8
			cmp   r7, r3
			subge r7, r7, r3
			ldrsh r7, [r1, r7]
			ldr   r8, [r2, #0x14]
			muls  r7, r8, r7
			add   r6, r6, r7, asr #4
			ldr   r8, [r2, #0xc]
			cmp   r8, #0
			beq   LAB_0001029c
			add   r7, r5, r3
			sub   r7, r7, r8
			cmp   r7, r3
			subge r7, r7, r3
			ldrsh r7, [r1, r7]
			ldr   r8, [r2, #0x18]
			add   r6, r6, r7, asr #4

		LAB_0001029c:

			ldrh  r8, [r0]
			sub   r8, r8, sb
			add   r8, r8, r6
			strh  r8, [r1, r5]
			add   r8, r8, sl
			strh  r8, [r0]
			add   r0, r0, #2
			add   r5, r5, #2
			cmp   r0, r4
			blt   LAB_0001021c
			add   r0, pc, #0x14
			str   r5, [r0]
			pop   {r4, r5, r6, r7, r8, sb, sl}
			bx    lr

.word 0, 0, 0
rvi:
    .word 0
.word 0x54524142  @ "BART"

