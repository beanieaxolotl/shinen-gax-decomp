// void GAX_ASSERT_PRINT
// https://decomp.me/scratch/nTYY5 - beanieaxolotl
// accuracy -> 63.45%

void GAX_ASSERT_PRINT(int x, int y, const char* string) {

    u16 *vram_offset;
    u8  letter;
    u32 offset;
    u16 mapped_letter;
    int i;
    
    vram_offset = (u16*)(VRAM + (x*2 + y*64));
    letter      = *string;

    while (TRUE) {
        
        if (letter == 0) {
            // stop processing if we
            // encounter zero-pad
            return;
        }
        i = 0;
        offset        = ((*vram_offset & 0x3f)>>1);
        mapped_letter = *string;
        
        if (offset < 32) {
            while (TRUE) {
                
                if (letter == 0x00 || letter == ' ' || letter == '\n') {
                    goto map_letter;
                } else if (offset > 29) {
                    // our text wound up off-screen!
                    break;
                }
                offset++;
                i++;
                if (offset > 31) {
                    goto map_letter;
                }
                letter = string[i];
            }
            vram_offset = (u16*)((u32)offset & 0xffffffc0) + 0x40;
            
        }

        map_letter:
            if (letter == '_') {
                letter = 0x5D;
            }
            if (letter == ':') {
                letter = 0x5C;
            }
            if (letter == '.') {
                letter = 0x5B;
            }     
            if (letter == '\n') {
                // new line
                vram_offset = (u16*)((u32)offset & 0xffffffc0) + 0x3F;
            }           
        
            if (letter == ' ') {
                // space
                letter = 0;
            } else {
                // lowercase to uppercase handler
                if (letter < 'A') {
                    letter -= 0x2F;
                } else if (letter < 'a') {
                    if (letter < 'A') {
                        goto process_text;
                    }
                    letter -= 0x36;
                } else {
                    letter -= 0x56;
                }
            }

        process_text:
            *vram_offset = mapped_letter;
            vram_offset++;
            //letter = ((u8*)string)[1];
            string++;
            
    }
}

// void GAX_ASSERT
// https://decomp.me/scratch/fK0vr - beanieaxolotl
// accuracy -> 45.13%

void GAX_ASSERT(const char* fn, const char* msg) {

    u16 pressed_keys = 0;
    
    // clear registers
    REG_IME       = 0;
    REG_DMA1CNT_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;
    REG_DMA2CNT_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;
    REG_DMA3CNT_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;
    (*(u16 *)0x40000EA) = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;

    CpuFastSet(0, (void*)VRAM, 0x1004000);

    // TO DO: everything here in the middle here
    
    // hardcoded values for the non-alphanumeric glyphs
    (*(u32*)(VRAM+0x44B8)) = 0x1000;     // period
    (*(u32*)(VRAM+0x44C8)) = 0x10000;    // colon (top dot)
    (*(u32*)(VRAM+0x44D4)) = 0x10000;    // colon (bottom dot)
    (*(u32*)(VRAM+0x44FC)) = 0x01111110; // underscore

    GAX_ASSERT_PRINT(0,0,"GAX ENGINE V3.05A Aug 16 2004\n\nEXCEPTION. PROGRAM HALT.");
    GAX_ASSERT_PRINT(0,5,"FUNCTION NAME:");
    GAX_ASSERT_PRINT(15,5,fn);
    GAX_ASSERT_PRINT(0,7,msg);

    while (TRUE) {
        while(!(pressed_keys & START_BUTTON)) {
            pressed_keys = REG_KEYINPUT ^ KEYS_MASK;           
        }
        SoftReset(0);
    }

}
