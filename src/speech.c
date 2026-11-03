#include "gax.h"
#include "speech_internal.h"


// macro functions

#undef  STEP
#define STEP( B_TIMES_TWO, MIC, INVA )  \
    temp1    = GSM_ADD( *LARc++, MIC ) << 10;   \
    temp1    = GSM_SUB( temp1, B_TIMES_TWO );   \
    temp1    = GSM_MULT_R( INVA, temp1 );       \
    *LARpp++ = GSM_ADD( temp1, temp1 );

#define saturate(x) ((x) < MIN_WORD ? MIN_WORD : (x) > MAX_WORD ? MAX_WORD: (x))
    

// GSM 6.10 tables
// sourced from libgsm, https://quut.com/gsm/

const s16 speech_FAC[8] = {

    // normalized direct mantissa used to compute xM/xmax
    18431, 20479, 22527, 24575,
    26623, 28671, 30719, 32767

};

const s16 speech_QLB[4] = {

    // quantization levels of LTP gain quantizer 
    3277, 11469, 21299, 32767

};


// inlined functions

static inline s16 speech_sub(s16 a, s16 b) {
    
    // implementation of gsm_sub
    // https://github.com/timothytylee/libgsm/src/add.c#L29
    
    s32 diff = (s32)a - (s32)b;
    return saturate(diff);
    
}

static inline s16 speech_asr(s16 a, s32 n) {
    
    // implementation of gsm_asr
    // https://github.com/timothytylee/libgsm/src/add.c#L183
    
    if (n >= 16)  return -(a < 0);
    if (n <= -16) return 0;
    if (n < 0)    return a << -n;
    
    return a >> n;

}


// functions

// void GAXSpeech_internal0
// https://decomp.me/scratch/ZjU9Y - beanieaxolotl, christianttt
// accuracy -> 100%

void GAXSpeech_internal0(s16* LARc, s16* LARpp) {

    // implementation of Decoding_of_the_coded_Log_Area_Ratios
    // https://github.com/timothytylee/libgsm/blob/master/src/short_term.c#L23
    
    s16 temp1;
    s32 ltmp;

	STEP(      0,  -32,  13107 );
	STEP(      0,  -32,  13107 );
	STEP(   4096,  -16,  13107 );
	STEP(  -5120,  -16,  13107 );

	STEP(    188,   -8,  19223 );
	STEP(  -3584,   -8,  17476 );
	STEP(   -682,   -4,  31454 );
	STEP(  -2288,   -4,  29708 );

}

// void GAXSpeech_internal1
// https://decomp.me/scratch/kUbtb - beanieaxolotl, christianttt
// accuracy -> 100%

void GAXSpeech_internal1(s16* LARp) {

    // implementation of LARp_to_rp
    // https://github.com/timothytylee/libgsm/blob/master/src/short_term.c#L144
	
    int i;
    s16 temp;

    for (i = 1; i <= 8; i++, LARp++) {
        
        if (*LARp < 0) {
			
            temp = (*LARp == MIN_WORD) ? MAX_WORD : -(*LARp);

            *LARp = -((temp < 11059) ? (temp << 1)
                     : (temp < 20070) ? (temp + 11059)
                     : GSM_ADD(temp >> 2, 26112));
        } else {
            temp = *LARp;
            
            *LARp = ((temp < 11059) ? (temp << 1)
                     : (temp < 20070) ? (temp + 11059)
                     : GSM_ADD(temp >> 2, 26112));
        }

    }

}

// void GAXSpeech_internal3
// https://decomp.me/scratch/ZH1H7 - Warm Mosquito (anon)
// accuracy -> 99.91%

#define SASR(x, by) ((x) >> (by))

void GAXSpeech_internal3(struct speech_state* S, s16* LARc, s16* s, s8* d) {
    s16 LARp[8];
    s16* LARpp_j_1;
    s16* s_27;
    s16* s_40;
    s16* s_13;
    s8* d_27;
    register s16* LARpp_j asm("sl");
    register s8 *d_40;

    LARpp_j_1 = S->LARpp[S->j];
    LARpp_j = S->LARpp[S->j ^= 1];

    GAXSpeech_internal0(LARc, LARpp_j_1);

    // Coefficients 0..12
    {
        int i;
        s16 *src1 = LARpp_j;
        s16 *src2 = LARpp_j_1;
        s16 *dst = LARp;
        for (i = 7; i >= 0; i--, dst++, src1++, src2++) {
            *dst = SASR(*src1, 2) + SASR(*src2, 2) + SASR(*src1, 1);
        }
    }
    GAXSpeech_internal1(LARp);
    S->filter(S, LARp, 13, s, d);

    // Coefficients 13..26
    {
        int i;
        s16 *src1 = LARpp_j;
        s16 *src2 = LARpp_j_1;
        s16 *dst = LARp;
        register s8 *d_13;

        s_13 = s + 13;
        d_13 = d + 13;
        s_27 = s + 27;
        d_27 = d + 27;
        s_40 = s_27 + 13;
        d_40 = d + 40;

        for (i = 7; i >= 0; i--, src1++, src2++, dst++) {
            *dst = SASR(*src1, 1) + SASR(*src2, 1);
        }
        GAXSpeech_internal1(LARp);
        S->filter(S, LARp, 14, s_13, d_13);
    }

    // Coefficients 27..39
    {
        int i;
        s16 *src1 = LARpp_j;
        s16 *src2 = LARpp_j_1;
        s16 *dst = LARp;
        for (i = 7; i >= 0; i--, src1++, src2++, dst++) {
            *dst = SASR(*src1, 2) + SASR(*src2, 2) + SASR(*src2, 1);
        }
    }
    GAXSpeech_internal1(LARp);
    S->filter(S, LARp, 13, s_27, d_27);

    // Coefficients 40..159
    {
        int i;
        s16 *src1 = LARpp_j_1;
        s16 *dst = LARp;
        for (i = 7; i >= 0; i--, dst++, src1++) {
            *dst = *src1;
        }
    }
    GAXSpeech_internal1(LARp);
    S->filter(S, LARp, 120, s_40, d_40);
}


// void GAXSpeech_internal5
// https://decomp.me/scratch/0j2jj - beanieaxolotl
// accuracy -> 100%

void GAXSpeech_internal5(s16* xMc, s16 mant, s16 exp, s16* xMp) {

    // implementation of APCM_inverse_quantization
    // https://github.com/timothytylee/libgsm/src/rpe.c#L349
    
    int i;
    s16 temp, temp1, temp2, temp3;
    s32 ltmp;

    temp1 = speech_FAC[mant];
    temp2 = speech_sub(6, exp);
    temp3 = speech_asl(1, speech_sub(temp2, 1));

    for (i = 13; i--;) {

        temp = (*xMc++ << 1) - 7;
        temp <<= 12;
        
        temp   = GSM_MULT_R(temp1, temp);
        temp   = GSM_ADD(temp, temp3);
        *xMp++ = speech_asr(temp, temp2);
        
    }
    
}

// void GAXSpeech_internal6
// https://decomp.me/scratch/ncC8W - Warm Mosquito (anon)
// accuracy -> 97.62%

static inline void APCM_quantization_xmaxc_to_exp_mant(s16 xmaxc,
                                                       s16* exp_out,
                                                       s16* mant_out) {
    s16 exp = 0;
    s16 mant;

    if (xmaxc > 15) exp = SASR(xmaxc, 3) - 1;
    mant = xmaxc - (exp << 3);

    if (mant == 0) {
        exp  = -4;
        mant = 7;
    } else {
        while (mant <= 7) {
            mant = (mant << 1) | 1;
            exp--;
        }
        mant -= 8;
    }

    *exp_out  = exp;
    *mant_out = mant;
}

static inline void RPE_grid_positioning(s16 Mc,
                                        register s16* xMp,
                                        register s16* ep) {
    int i = 13;
    
    switch (Mc) {
        case 3: *ep++ = 0;
        case 2: do {
                        *ep++ = 0;
        case 1:         *ep++ = 0;
        case 0:         *ep++ = *xMp++;
                } while (--i);
    } 
    while (++Mc < 4) *ep++ = 0;
}

void GAXSpeech_internal6(struct speech_state *S,
                         s16 xmaxcr,
                         s16 Mcr,
                         s16* xMcr,
                         s16* erp) {
    s16 exp, mant;

    APCM_quantization_xmaxc_to_exp_mant(xmaxcr, &exp, &mant);
    GAXSpeech_internal5(xMcr, mant, exp, xMcr);
    RPE_grid_positioning(Mcr, xMcr, erp);
}

// void GAXSpeech_mix
// https://decomp.me/scratch/FWluT - Warm Mosquito (anon) (fork of 8e0R0)
// accuracy -> 100.00% (score 0/2100)

void GAXSpeech_mix(s16* out, s16* in, int count, int step) {
    register int step_r6 asm("r6") = step;
    register int clob_r4 asm("r4");
    register int clob_r5 asm("r5");

    __asm__ volatile (
        "add r3, pc, #0\n\t"
        "bx r3\n\t"
        ".arm\n\t"
        "mov r4, #0\n\t"
        "1:\n\t"
        "lsr r3, r4, #16\n\t"
        "ldrsb r3, [r1, r3]\n\t"
        "add r3, r3, #128\n\t"
        "ldrh r5, [r0]\n\t"
        "add r5, r5, r3, lsl #5\n\t"
        "strh r5, [r0]\n\t"
        "add r0, r0, #2\n\t"
        "add r4, r4, r6\n\t"
        "subs r2, r2, #1\n\t"
        "bgt 1b\n\t"
        "add r3, pc, #4\n\t"
        "add r3, r3, #1\n\t"
        "bx r3\n\t"
        ".thumb\n\t"
        : "+r" (out), "+r" (in), "+r" (count), "=r" (clob_r4), "=r" (clob_r5)
        : "r" (step_r6)
        : "r3"
    );
}

// void GAXSpeech_open
// https://decomp.me/scratch/B3lkx - beanieaxolotl
// accuracy -> 100%

void GAXSpeech_open(GAXSpeech_unk* param_1) {
    
    speech_create(&param_1->unk04);
    
    param_1->unk798 = -1;
    param_1->unk79C =  0;
    param_1->unk7A0 =  0;
    
}

void GAXSpeech_render() {
	
}


// void SLTSF (Short/Long Term Synthesis Filtering)
// https://decomp.me/scratch/f5qQF - Warm Mosquito (anon) (fork of pEtk7)
// accuracy -> 64.88% (up from 35.99%)

void SLTSF(GAX_unk0* S, s16 Ncr, u16 bcr, s16* erp, s16* drp) {
    s16 Nr;
    s16 brp;
    s16 *drp_nr;
    s16 *dp;
    u32 *dst, *src;
    int k;

    bcr = (u16)bcr;
    Nr = Ncr - 40;
    if ((u16)Nr > 80)
        Nr = S->unk252;
    else
        Nr = Ncr;
    S->unk252 = Nr;

    brp = speech_QLB[bcr];
    dst = (u32*)((u8*)drp - 0xf0);
    src = (u32*)((u8*)drp - 0xa0);
    dp = drp;
    drp_nr = dp - Nr;

    for (k = 39; k >= 0; k--) {
        s32 drpp = GSM_MULT_R(brp, *drp_nr++);
        *dp++ = *erp++ + drpp;
    }

    for (k = 14; k >= 0; k--) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}

// s16 speech_asl
// https://decomp.me/scratch/bS89k - beanieaxolotl
// accuracy -> 100%

s16 speech_asl(s16 a, s32 n) {

    // implementation of gsm_asl
    // https://github.com/timothytylee/libgsm/src/add.c#L161
    
    if (n >= 16)  return 0;
    if (n <= -16) return -(a < 0);
    if (n < 0)    return speech_asr(a, -n);
    
    return a << n;
    
}

// void speech_create
// https://decomp.me/scratch/qne2u - beanieaxolotl
// accuracy -> 100%

void speech_create(GAX_unk0* speech_unk) {
    
    if (speech_unk != NULL) {
        
        GAX_clear_mem((u32)speech_unk, 0x268); // clear / prepare memory
        speech_unk->unk252 = 40;
        
        CpuSet(STSF, 
               &speech_unk->unk530, 
               REG_BASE|0x55);      // REG_BLDY
        CpuSet(SLTSF, 
               &speech_unk->unk684, 
               REG_BASE|0x2D);      // REG_BG2Y
        CpuSet(GAXSpeech_mix, 
               &speech_unk->unk738, 
               (u32)&REG_BG1HOFS);

        speech_unk->unk788 = &speech_unk->unk531;
        speech_unk->unk78C = &speech_unk->unk685;
        speech_unk->unk790 = &speech_unk->unk739;
        
    }
    
}

// s32 speech_decode
// https://decomp.me/scratch/9OGGK - Warm Mosquito (anon) (fork of 05wDP)
// accuracy -> 89.75% (up from 46.79%)

s32 speech_decode(struct speech_state *s, u8* c, s32 target) {
    s16 *drp, *erp, *wt;
    s16 *xmaxc, *Mc, *xmc, *Nc, *bc;
    int i, k;

    if ((*c >> 4 & 0x0F) != GSM_MAGIC) return -1;

    s->LARc[0]  = (*c++ & 0xF) << 2;
    s->LARc[0] |= (*c >> 6) & 0x3;
    s->LARc[1]  = *c++ & 0x3F;
    s->LARc[2]  = (*c >> 3) & 0x1F;
    s->LARc[3]  = (*c++ & 0x7) << 2;
    s->LARc[3] |= (*c >> 6) & 0x3;
    s->LARc[4]  = (*c >> 2) & 0xF;
    s->LARc[5]  = (*c++ & 0x3) << 2;
    s->LARc[5] |= (*c >> 6) & 0x3;
    s->LARc[6]  = (*c >> 3) & 0x7;
    s->LARc[7]  = *c++ & 0x7;
    s->Nc[0]  = (*c >> 1) & 0x7F;
    s->bc[0]  = (*c++ & 0x1) << 1;
    s->bc[0] |= (*c >> 7) & 0x1;
    s->Mc[0]  = (*c >> 5) & 0x3;
    s->xmaxc[0]  = (*c++ & 0x1F) << 1;
    s->xmaxc[0] |= (*c >> 7) & 0x1;
    s->xmc[0]  = (*c >> 4) & 0x7;
    s->xmc[1]  = (*c >> 1) & 0x7;
    s->xmc[2]  = (*c++ & 0x1) << 2;
    s->xmc[2] |= (*c >> 6) & 0x3;
    s->xmc[3]  = (*c >> 3) & 0x7;
    s->xmc[4]  = *c++ & 0x7;
    s->xmc[5]  = (*c >> 5) & 0x7;
    s->xmc[6]  = (*c >> 2) & 0x7;
    s->xmc[7]  = (*c++ & 0x3) << 1;
    s->xmc[7] |= (*c >> 7) & 0x1;
    s->xmc[8]  = (*c >> 4) & 0x7;
    s->xmc[9]  = (*c >> 1) & 0x7;
    s->xmc[10]  = (*c++ & 0x1) << 2;
    s->xmc[10] |= (*c >> 6) & 0x3;
    s->xmc[11]  = (*c >> 3) & 0x7;
    s->xmc[12]  = *c++ & 0x7;
    s->Nc[1]  = (*c >> 1) & 0x7F;
    s->bc[1]  = (*c++ & 0x1) << 1;
    s->bc[1] |= (*c >> 7) & 0x1;
    s->Mc[1]  = (*c >> 5) & 0x3;
    s->xmaxc[1]  = (*c++ & 0x1F) << 1;
    s->xmaxc[1] |= (*c >> 7) & 0x1;
    s->xmc[13]  = (*c >> 4) & 0x7;
    s->xmc[14]  = (*c >> 1) & 0x7;
    s->xmc[15]  = (*c++ & 0x1) << 2;
    s->xmc[15] |= (*c >> 6) & 0x3;
    s->xmc[16]  = (*c >> 3) & 0x7;
    s->xmc[17]  = *c++ & 0x7;
    s->xmc[18]  = (*c >> 5) & 0x7;
    s->xmc[19]  = (*c >> 2) & 0x7;
    s->xmc[20]  = (*c++ & 0x3) << 1;
    s->xmc[20] |= (*c >> 7) & 0x1;
    s->xmc[21]  = (*c >> 4) & 0x7;
    s->xmc[22]  = (*c >> 1) & 0x7;
    s->xmc[23]  = (*c++ & 0x1) << 2;
    s->xmc[23] |= (*c >> 6) & 0x3;
    s->xmc[24]  = (*c >> 3) & 0x7;
    s->xmc[25]  = *c++ & 0x7;
    s->Nc[2]  = (*c >> 1) & 0x7F;
    s->bc[2]  = (*c++ & 0x1) << 1;
    s->bc[2] |= (*c >> 7) & 0x1;
    s->Mc[2]  = (*c >> 5) & 0x3;
    s->xmaxc[2]  = (*c++ & 0x1F) << 1;
    s->xmaxc[2] |= (*c >> 7) & 0x1;
    s->xmc[26]  = (*c >> 4) & 0x7;
    s->xmc[27]  = (*c >> 1) & 0x7;
    s->xmc[28]  = (*c++ & 0x1) << 2;
    s->xmc[28] |= (*c >> 6) & 0x3;
    s->xmc[29]  = (*c >> 3) & 0x7;
    s->xmc[30]  = *c++ & 0x7;
    s->xmc[31]  = (*c >> 5) & 0x7;
    s->xmc[32]  = (*c >> 2) & 0x7;
    s->xmc[33]  = (*c++ & 0x3) << 1;
    s->xmc[33] |= (*c >> 7) & 0x1;
    s->xmc[34]  = (*c >> 4) & 0x7;
    s->xmc[35]  = (*c >> 1) & 0x7;
    s->xmc[36]  = (*c++ & 0x1) << 2;
    s->xmc[36] |= (*c >> 6) & 0x3;
    s->xmc[37]  = (*c >> 3) & 0x7;
    s->xmc[38]  = *c++ & 0x7;
    s->Nc[3]  = (*c >> 1) & 0x7F;
    s->bc[3]  = (*c++ & 0x1) << 1;
    s->bc[3] |= (*c >> 7) & 0x1;
    s->Mc[3]  = (*c >> 5) & 0x3;
    s->xmaxc[3]  = (*c++ & 0x1F) << 1;
    s->xmaxc[3] |= (*c >> 7) & 0x1;
    s->xmc[39]  = (*c >> 4) & 0x7;
    s->xmc[40]  = (*c >> 1) & 0x7;
    s->xmc[41]  = (*c++ & 0x1) << 2;
    s->xmc[41] |= (*c >> 6) & 0x3;
    s->xmc[42]  = (*c >> 3) & 0x7;
    s->xmc[43]  = *c++ & 0x7;
    s->xmc[44]  = (*c >> 5) & 0x7;
    s->xmc[45]  = (*c >> 2) & 0x7;
    s->xmc[46]  = (*c++ & 0x3) << 1;
    s->xmc[46] |= (*c >> 7) & 0x1;
    s->xmc[47]  = (*c >> 4) & 0x7;
    s->xmc[48]  = (*c >> 1) & 0x7;
    s->xmc[49]  = (*c++ & 0x1) << 2;
    s->xmc[49] |= (*c >> 6) & 0x3;
    s->xmc[50]  = (*c >> 3) & 0x7;
    s->xmc[51]  = *c & 0x7;

    drp = s->drp;
    erp = s->erp;
    wt = s->wt;
    xmaxc = s->xmaxc;
    Mc = s->Mc;
    xmc = s->xmc;
    Nc = s->Nc;
    bc = s->bc;

    for (i = 0; i < 4; i++) {
        GAXSpeech_internal6(s, *xmaxc++, *Mc++, xmc, erp);
        s->SLTSF(s, *Nc++, *bc++, erp, drp);
        {
            s16 *w = &wt[i * 40];
            s16 *d = drp;
            for (k = 39; k >= 0; k--) {
                *w++ = *d++;
            }
        }
        xmc += 13;
    }

    GAXSpeech_internal3(s, s->LARc, wt, target);
    return 0;
}

void STSF() {
	
}
