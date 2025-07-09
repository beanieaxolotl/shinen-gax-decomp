#include "gax.h"


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

void speech_asl() {
	
}

void speech_create() {
	
}

void speech_decode() {
	
}

void STSF() {
	
}
