#include "gax.h"

// constants

const s16 speech_FAC[8] = {

    18431, 20479, 22527, 24575,
    26623, 28671, 30719, 32767

};

const s16 speech_QLB[4] = {

    3277, 11469, 21299, 32767

};


void GAXSpeech_internal0() {

}

// void GAXSpeech_internal1
// https://decomp.me/scratch/ALcRA - beanieaxolotl
// accuracy -> 84.15%

void GAXSpeech_internal1(s16* param1) {
    
    int i;
    s16 sVar1, iVar2;
    
    for (i = 7; i > -1; i--) {
        
        if (*param1 < 0) {
            
            if (*param1 == -32768) {
                sVar1 = 32767;
            } else {
                sVar1 = -*param1;
            }

            if (sVar1 < 0x2B33) {
                sVar1 *= -2;
            } else if (sVar1 < 0x4E66) {
                sVar1 = -(sVar1 + 0x2B33);
            } else {
                sVar1 = -((sVar1 >> 2) + 0x6600);
            }
            
        } else {

            sVar1 = *param1;
            iVar2 = sVar1;
            if (iVar2 < 0x2B33) {
                sVar1 = iVar2<<1;
            } else if (iVar2 < 0x4E66) {
                sVar1 += 0x2B33;
            } else {
                sVar1 = (sVar1>>2)+0x6600;
            }
        }
        
        *param1 = sVar1;
        *param1++;
        
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
