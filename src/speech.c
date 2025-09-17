#include "gax.h"

// Put in gsm header file to #include later
#define MIN_WORD -32768
#define MAX_WORD 32767
#define GSM_ADD(a, b) ((a) + (b))
#define GSM_SUB(a, b)    ((a) - (b))
#define GSM_MULT_R(a, b) ((a) * (b) + 0x4000) >> 0xF
#undef	STEP
#define	STEP( B_TIMES_TWO, MIC, INVA )	\
    temp1    = GSM_ADD( *LARc++, MIC ) << 10;	\
    temp1    = GSM_SUB( temp1, B_TIMES_TWO );	\
    temp1    = GSM_MULT_R( INVA, temp1 );		\
    *LARpp++ = GSM_ADD( temp1, temp1 );
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
// https://decomp.me/scratch/yqNKI - beanieaxolotl
// accuracy -> 96.69%

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

void GAXSpeech_internal3() {
	
}

void GAXSpeech_internal5() {
	
}

void GAXSpeech_internal6() {
	
}

void GAXSpeech_mix() {
	
}

void GAXSpeech_open() {
	
}

void GAXSpeech_render() {
	
}


void SLTSF() {
	
}

// s16 speech_asl
// https://decomp.me/scratch/ocZIG - beanieaxolotl
// accuracy -> 46.89%

s16 speech_asl(s16 param1, s32 param2) {
    
    if (param2 > 15) {
        if (param2 > -15) {
            if (param1 << 16 >= 0) {
                return 0;
            } else {
                return -1;
            }
        }
    
        if (param2 < 0) {
            if ((param2 != -15) && (-param2 > 14)) {
                if (param1 < 0) {
                    return -1;
                }
                return 0;
            }
            if ((param2 == 16) || (-param2 < -16)) {
                return 0;
            }
            if (param2 < 1) {
                return param1 >> param2;
            }
        }
        return param1 << param2;
        
    } else {
        return 0;
    }
    
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

void speech_decode() {
	
}

void STSF() {
	
}
