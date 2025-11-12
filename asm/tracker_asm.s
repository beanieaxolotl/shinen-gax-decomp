GAX_umull:

	stmdb sp!, {r3, r4}
	umull r3, r4, r0, r1
	cpy   r0, r4
	ldmia sp!, {r3, r4}
	bx    lr


GAXTracker_asm:

	push  {r4, r5, r6, r7, r8, sb, sl, fp}
	ldmia r0, {r1, r2, r3, r4, r5, r6, r7}

	LAB_0001001c:

		ldrb  r8, [r1, r4, asr #11]
		adds  r4, r4, r7
		subpl r4, r4, r5
		ldrb  sb, [r1, r4, asr #11]
		adds  r4, r4, r7
		subpl r4, r4, r5
		orr   r8, r8, sb, lsl #16
		mul   r8, r6, r8
		bic   fp, r8, #0x70000
		ldrb  r8, [r1, r4, asr #11]
		adds  r4, r4, r7
		subpl r4, r4, r5
		ldrb  sb, [r1, r4, asr #11]
		adds  r4, r4, r7
		subpl r4, r4, r5
		orr   r8, r8, sb, lsl #16
		mul   r8, r6, r8
		bic   r8, r8, #0x70000
		ldmia r2, {sb, sl}


GAXTracker_asm_boost_patch:

	add   sb, sb, fp, lsr #3
	add   sl, sl, r8, lsr #3
	stmia r2!, {sb, sl}
	subs  r3, r3, #4
	bgt   LAB_0001001c
	str   r4, [r0, #0xc]
	push  {r4, r5, r6, r7, r8, sb, sl, fp}
	bx    lr


GAXTracker_pingpong_asm:

	GAXTracker_asm_end:

		push  {r4, r5, r6, r7, r8, sb, sl, fp, lr}
		ldmia r0, {r1, r2, r3, r4, r5, r6, r7}
		cmp   r7, #0
		bl    FUN_0001011c

		LAB_00010098:

			ldrb  r8, [r1, r4, asr #11]
			adds  r4, r4, r7
 
			blpl  FUN_0001010c
			ldrb  sb, [r1, r4, asr #11]
			adds  r4, r4, r7
 
			blpl  FUN_0001010c
			orr   r8, r8, sb, lsl #16
			mul   r8, r6, r8
			bic   fp, r8, #0x70000
			ldrb  r8, [r1, r4, asr #11]
			adds  r4, r4, r7
 
			blpl  FUN_0001010c
			ldrb  sb, [r1, r4, asr #11]
			adds  r4, r4, r7
 
			blpl  FUN_0001010c
			orr   r8, r8, sb, lsl #16
			mul   r8, r6, r8
			bic   r8, r8, #0x70000
			ldmia r2, {sb, sl}

	GAXTracker_pingpong_asm_boost_patch:

		add   sb, sb, fp, lsr #3
		add   sl, sl, r8, lsr #3
		stmia r2!, {sb, sl}
		subs  r3, r3, #4
		bgt  LAB_00010098
		str  r4, [r0, #0xc]
		str  r7, [r0, #0x18]
		str  r1, [r0]
		pop  {r4, r5, r6, r7, r8, sb, sl, fp, lr}
		bx   lr

		FUN_0001010c:

			rsbs  r7, r7, #0
			add   r4, r4, r7, lsl #1
			subpl r4, r4, r5
			addpl r1, r1, r5, lsr #11

		FUN_0001011c:

			movmi sl, #0x4b
			addmi r4, r4, r5
			submi r1, r1, r5, lsr #11
			movpl sl, #0x5b
			strb  sl, [pc, #-0x91]
			strb  sl, [pc, #-0x89]
			strb  sl, [pc, #-0x75]
			strb  sl, [pc, #-0x6d]
			bx    lr
