#include "gax.h"
#include "gax_assert.h"
#include "gax_internal.h"


// defines

#define UNDEFINED_REG_H (*(vu16 *)0x40000EA) // used by GAX_ASSERT


// externals

extern u8 GAXOutput_reverb_asm[];
extern u8 GAXOutput_reverb_asm_end[];
extern u8 GAXOutput_filter_lowpass_asm[];
extern u8 GAXOutput_filter_lowpass_asm_end[];
extern u8 GAXOutput_render_asm[];
extern u8 GAXOutput_render_asm_end[];
extern u8 GAXTracker_asm[];
extern u8 GAXTracker_asm_end[];
extern u8 GAXTracker_pingpong_asm[];
extern u8 GAXTracker_pingpong_asm_end[];


// constants

// the copyright string for the sound driver
#if MATCHING > 0
    const char* GAX_copyright = "GAX Sound Engine 3.05A (Aug 16 2004) \xa9 Shin'en Multimedia. Code: B.Wodok";
#else
    const char* GAX_copyright = "GAX Sound Engine 3.05A ("__DATE__") \xa9 Shin'en Multimedia. Code: B.Wodok";
#endif

// mixing rate / timer reload value table
const u16 GAX_table[13*2] = {

     5735, 2926,
     9079, 1848,
    10513, 1596,
    11469, 1463,
    13380, 1254,
    15769, 1064,
    18158,  924,
    21025,  798,
    26760,  627,
    31537,  532,
    36316,  462,
    40138,  418,
    42049,  399

};

// GAX defaults to this package when a song is not specified
const u8 gax_default_package[200] = {
    // to do
};

// assertion screen font
const char GAX_font[281] = {
// BIOS Huffman compressed font.
// Header: 0x0004A028 -> Type=Huff(2), Unit=8-bit, DecompressedSize=1184 bytes.
// Following data for tree: size byte 0x11 -> 18 bytes of tree data following then the compressed font glyphs of 37 tiles of 4bpp, 8x8 font data (A-Z, 0-9, space) 32 bytes each.
};


// functions

// static u8* GAX_code2ram
// https://decomp.me/scratch/2zNvP - EstexNT
// accuracy -> 100%

static u8* GAX_code2ram(u8 *startaddr, u8 *endaddr) {
    s32 sz;
    u8 *scratchmem;
    u8 *out;

    sz = (u32)endaddr - (u32)startaddr;
    scratchmem = GAX_ram->scratch_mem;
    if ((u32)scratchmem & 3) {
        u32 newsize;
        newsize = (u8 *)GAX_ALIGN((u32)scratchmem, 4) - (u32)scratchmem;
        GAX_ram->scratch_mem = (u8 *)GAX_ALIGN((u32)scratchmem, 4);
        GAX_ram->scratch_mem_size -= newsize;
    }
    if ((GAX_ram->params->debug) && (sz > GAX_ram->scratch_mem_size)) {
        GAX_ASSERT("GAX2_INIT", "OUT OF MEMORY");
    }
    GAX_ram->scratch_mem_size -= sz;
    out = GAX_ram->scratch_mem;
    GAX_ram->scratch_mem += sz;

    CpuCopy(startaddr, out, sz, 32);
    return out;
}

// void GAX_open
// https://decomp.me/scratch/sH7lJ - beanieaxolotl, taxicat1
// accuracy -> 93.97%

void GAX_open() {

    int i;
    int offset;
    u8* num_channels;
    
    GAXOutput_open(GAX_ram->replayer[GAX_ram->mix_buffer_id]); // reset timer
    GAX_ram->current_buf = GAX_ram->buf_header_dma1;           // default to using the DMA1 buffer
    
    i = 0;

    // this is wrong
    num_channels = &GAX_ram->replayer[GAX_ram->mix_buffer_id]->song_2->num_channels;
    
    if (i < *num_channels) {
        offset = 0;
        do {
            // reset the channel for playback
            GAXTracker_open((GAX_channel*)(&GAX_ram->replayer[GAX_ram->mix_buffer_id]->channels->ignore + offset));
            // load the data for the song's first order
            *(u32*)((void*)&GAX_ram->replayer[GAX_ram->mix_buffer_id]->channels->order + offset) =
                *(u32*)(&GAX_ram->replayer[GAX_ram->mix_buffer_id]->song_2->track_data + i + 5);
            offset += sizeof(GAX_channel);
            i++;
        } while (i < *num_channels);
    }

    GAXSync_open(GAX_ram->replayer[GAX_ram->mix_buffer_id]); // set up replayer variables
    GAX_ram->replayer[GAX_ram->mix_buffer_id]->song = GAX_ram->replayer[GAX_ram->mix_buffer_id]->song_2;

    if (GAX_ram->params->flags & GAX_SPEECH) {
        // if the user enabled the GAX_SPEECH flag (voice playback)
        GAXSpeech_open((int)GAX_ram->speech_unk);
        *GAX_ram->speech_unk = (u32)GAX_ram->fx_data;
    }
    
}

// void GAX2_init_song
// https://decomp.me/scratch/W9z6A - beanieaxolotl
// accuracy -> 66.96%

void GAX2_init_song(GAXSongData *song_data, vu16 *dmacnt, u16 dma_param1, u16 dma_param2) {

    // unidentified vars
    int i;
    u32 j;
    GAX_player* f;
    u32 uVar1;
    int a;
    u32* d;
    u32* e;
    int b;

    // identified vars
    u32*         sync;
    GAX_player*  replayer;
    GAX_channel* ch;
    u32*         alloc_size;
    int          ch_id;
    
    u32  num_ch = song_data->num_channels;
    
    i = GAX_ram->unk80;
    if ((i & 3) != 0) {
        j = i + 4 & -32;
        GAX_ram->unk80     = j;
        GAX_ram->free_mem -= (j-i);
    }

    if (GAX_ram->params->debug && GAX_ram->free_mem < 0x4C) {
        GAX_ASSERT("GAX2_INIT","OUT OF MEMORY");
    }

    
    alloc_size = &GAX_ram->free_mem;
    uVar1 = *alloc_size;
    *alloc_size = uVar1 - 0x4C;
    
    sync = &GAX_ram->unk80;
    replayer = (GAX_player*)*sync;
    f = replayer + 1;
    *sync = (u32)f;
    GAX_ram->replayer[GAX_ram->mix_buffer_id] = replayer;

    if ((u32)f & 3 != 0) {
        j = (u32)&replayer + 4 & -32;
        *sync = j;
        *alloc_size = (uVar1 - 0x4c) - (j - (int)f);
    }
    
    if (GAX_ram->params->debug && 
        (GAX_ram->free_mem <= num_ch*72) && (num_ch*72 - GAX_ram->free_mem)) {
        GAX_ASSERT("GAX2_INIT","OUT OF MEMORY");
    }

    GAX_ram->free_mem += num_ch * -0x48;
    ch = (GAX_channel*)GAX_ram->unk80;
    GAX_ram->unk80 = (u32)(ch + num_ch);
    
    replayer = GAX_ram->replayer[GAX_ram->mix_buffer_id];
    replayer->channels = ch;
    replayer->song_2 = song_data;

    if (num_ch != 0) {
        //a = 0;
        for (ch_id = 0; ch_id <= num_ch; ch_id++) {
            // iterate over every channel and assign them a unique number
            (&GAX_ram->replayer[GAX_ram->mix_buffer_id]->channels->channel_index)[a] = (s8)ch_id;
            a += 0x48;
        }
    }

    if (GAX_ram->unk5C != 0) {
        
        a = 0;
        *(u32*)GAX_ram->replayer[GAX_ram->mix_buffer_id]->unk28 = 
          *(u32*)(&song_data[1].step_count + 1);
        
        d = (u32*)(&song_data[1].instrument_data + 3);
        e = (u32*)(&song_data[1].restart_position + 1);
        
        for (ch_id = 0; ch_id < 3; ch_id++) {
            
            replayer = GAX_ram->replayer[GAX_ram->mix_buffer_id];
            b = ((u32)GAX_ram->buf_header_dma1->mixrate * *e) / 1000;
            
            *(u32*)(replayer->unk28 + ch_id * 4 + 4) = b << 1;
            *(u32*)(GAX_ram->replayer[GAX_ram->mix_buffer_id]->unk28 + ch_id * 4 + 16) = *d;

            b = *d;
            d++;
            a += b;
            e++;
            
        }

        *(u32*)(GAX_ram->replayer[GAX_ram->mix_buffer_id]->unk28 + 0x1C) = a;
        
    }

    GAX2_init_volboost();
    
}

// void GAX2_init_soundhw
// https://decomp.me/scratch/nNgcv - beanieaxolotl, christianttt
// accuracy -> 100%
// ======================
// edits by beanieaxolotl

void GAX2_init_soundhw() {

    int i;

    if (GAX_ram->buf_header_dma2 != 0) {

        REG_DMA1CNT    = 4;
        REG_DMA2CNT    = 4;
        REG_SOUNDCNT_X = 0;
        REG_SOUNDCNT_H = 0xFB0C;

        for (i = 0; i < 8; i++)
        {
            *(vu16*)REG_ADDR_FIFO_A = 0;
            *(vu16*)REG_ADDR_FIFO_B = 0;
        }

        REG_DMA2DAD = REG_ADDR_FIFO_B;
        REG_DMA2SAD = GAX_ram->buffer_dma2;

    } else {

        REG_DMA1CNT    = 4;
        REG_SOUNDCNT_X = 0;
        REG_SOUNDCNT_H = 0xB04;

        for (i = 0; i < 8; i++) {
            *(vu16*)REG_ADDR_FIFO_A = 0;
        }
        
    }

    REG_SOUNDBIAS_H = 0x42;
    REG_DMA1DAD     = REG_ADDR_FIFO_A;
    REG_DMA1SAD     = GAX_ram->buffer_dma1;

}

// void GAX2_init_volboost
// https://decomp.me/scratch/qDGXO - beanieaxolotl
// accuracy -> 38.09%

void GAX2_init_volboost() {
    
    u16   volume;
    void* tracker_asm;

    int iVar1;
    int iVar2;
    u16 uVar3;

    volume = GAX_ram->replayer[GAX_ram->mix_buffer_id]->song_2->volume;

    if (volume >= 512) {
        GAX_ram->volboost_level = 2;
        tracker_asm = GAX_ram->gax_tracker_asm;
        *(u16*)(tracker_asm+0xD0) = 0x90AB;
        *(u16*)(tracker_asm+0x54) = 0x90AB;
        iVar2 = (int)tracker_asm + 0x54;
        iVar1 = (int)tracker_asm + 0xD0;
        uVar3 = 0xA0A8;        
    } else {
        if (volume >= 256) {
            GAX_ram->volboost_level = 1;
            tracker_asm = GAX_ram->gax_tracker_asm;
            *(u16*)(tracker_asm+0xD0) = 0x912B;
            *(u16*)(tracker_asm+0x54) = 0x912B;
            iVar2 = (int)tracker_asm + 0x54;
            iVar1 = (int)tracker_asm + 0xD0; 
            uVar3 = 0xA128;
        } else {
            GAX_ram->volboost_level = 0;
            tracker_asm = GAX_ram->gax_tracker_asm;
            *(u16*)(tracker_asm+0xD0) = 0x91AB;
            *(u16*)(tracker_asm+0x54) = 0x91AB;
            iVar2 = (int)tracker_asm + 0x54;
            iVar1 = (int)tracker_asm + 0xD0; 
            uVar3 = 0xA1A8;
        }
    }
    *(u16*)(iVar1+4) = uVar3;
    *(u16*)(iVar2+4) = uVar3;
    
}

// void GAX_clear_mem
// https://decomp.me/scratch/yf3Sy - beanieaxolotl
// accuracy -> 100%

void GAX_clear_mem(u32 dest, u32 size) {

    // unnamed / unidentified variables
    u32 uVar1;
    u8* puVar2;
    int iVar3;
    int uVar4;

    // identified variables
    u32 src;

    
    while (dest & 3) {
        *(u8*)dest = 0;
        dest++, size--;
    }
    
    src   = 0;
    uVar4 = size & -32;
    uVar1 = uVar4;
    
    if (uVar4 < 0) {
        uVar1 = uVar4 + 3;
    }

    CpuFastSet(&src, (void*)dest, 
              (uVar1 << 9) >> 0xB | 1 << 24);
    
    puVar2 = (u8*)(dest + uVar4);
    iVar3  = size - uVar4;
    
    if (iVar3 > 0) {
        while (iVar3 != 0) {
            *puVar2 = 0;
            puVar2++, iVar3--;
        }
    }
    
}

// void GAX2_new
// https://decomp.me/scratch/G9TYg - EstexNT
// accuracy -> 100%

void GAX2_new(GAXParams* params) {

    s32 i, i1;
    u8 *param_temp, *param_temp1;
    
    if (params == NULL) {
        GAX_ASSERT("GAX2_NEW", "PARAMS ARG IS NULL");
        return;
    }

    param_temp = (u8 *)params;
    for (i = sizeof(*params); ((u32)param_temp & 3) != 0; i--) {
        *param_temp++ = 0;
        
    }
    CpuFastFill(0, param_temp, i & -0x20);

    param_temp1 = &param_temp[(i & -0x20)];
    for (i1 = 0; i1 < (i - (i & -0x20)); i1++) {
        *param_temp1++ = 0;
    }
    
    
    params->mixing_rate    = -1;
    params->fx_mixing_rate = -1;
    params->filter_depth   =  0;
    params->fx_channels    = -1;
    params->volume         = -1;
    params->debug          = TRUE;

}

// void GAX2_new_fx
// https://decomp.me/scratch/ffZpU - beanieaxolotl
// accuracy -> 100%

void GAX2_new_fx(GAXFXParams* fxparams) {
    
    if (fxparams == NULL) {
        GAX_ASSERT("GAX2_NEW_FX", "FXPARAMS ARG IS NULL");
    } else {
        fxparams->fxid   = -1;
        fxparams->fxch   = -1;
        fxparams->prio   = -1;
        fxparams->note   = -1;
        fxparams->volume = -1;
    }
    
}

// void GAX2_calc_mem
// https://decomp.me/scratch/Iwq4O - EstexNT
// accuracy -> 93.87%

void GAX2_calc_mem(GAXParams* params) {

    u32 temp_r2;
    u32 temp_r3;
    u32 temp_r4 = 0;
    u32 temp_r5;
    u32 temp_r6;
    const GAXSongData* song_data;
    u32 i, temp;
    u32 sp0 = 0;
    const void* sp4;
    const u16* table;

    if (params->song_data == NULL) {
        params->song_data = gax_default_package;
    }

    if (params->mixing_rate == 0xffff) {
        temp_r6 = ((const GAXSongData* )params->song_data)->mixing_rate;
        song_data = (const GAXSongData* )params->song_data;
    } else if (0) 
        {    settempi_0:
    i = temp;
    goto label_000100f8;
        }
         else {
        temp_r6 = params->mixing_rate;
        song_data = (const GAXSongData* )params->song_data;
    }
    table = GAX_table;
    for (temp = 0; temp < 13; temp++) {
        if (table[temp * 2] >= temp_r6) {
            goto settempi_0;
        }
    }
    i = 12;
    
    label_000100f8:
    temp_r6 = table[i * 2];
    
    if (params->fx_mixing_rate == 0xffff) {
        temp_r5 = temp_r6;
        if (song_data->fx_mixing_rate) {
            temp_r5 = song_data->fx_mixing_rate;
        }
    } else if (0) {
        settempi_1:
        i = temp;
        goto label_00010138;
    } else 
        {
        temp_r5 = params->fx_mixing_rate;
    }

    for (temp = 0; temp < 13; temp++) {
        if (table[temp * 2] >= temp_r5) {
            goto settempi_1;
        }
    }
    i = 12;
    
    label_00010138:
    temp_r5 = table[i * 2];
    if ((const u8* )song_data == gax_default_package) {
        temp_r6 = temp_r5;
    }
    if (temp_r5 != temp_r6) {
        sp0 = 1;
    }
    if (params->fx_channels == 0xffff) {
        temp_r3 = song_data->fx_channels;
    } else {
        temp_r3 = params->fx_channels;
    }
    if (params->fx_data == NULL) {
        temp_r3 = 0;
        params->fx_channels = 0;
    }
    temp_r4 += 0x8c;
    temp_r4 += GAX_ALIGN(temp_r3, 4);
    temp_r4 += temp_r3 * 80;
    if (params->flags & GAX_SPEECH) {
        temp_r4 += 0x7a4;
    }
    temp_r4 += 8;
    if (sp0) {
        temp_r4 += 8;
    }
    temp_r4 += (((((temp_r5 > temp_r6) ? temp_r5 : temp_r6) * 1000) / 59727) + 4) * 2;
    temp_r4 += ((temp_r6 * 1000) / 59727) * 2;
    if (sp0) {
        temp_r4 += ((temp_r5 * 1000) / 59727) * 2;
    }
    if (!(params->flags & GAX_NO_JINGLES) && song_data->unkC4) {
        temp_r2 = song_data->unkBC;
        temp_r4 += 0x4c;
        temp_r4 += song_data->unkC4 * 72;
    } else {
        temp_r2 = song_data->unkC0;
    }
    if (temp_r2) {
        temp_r4 += ((temp_r2 * temp_r6) / 1000) * 2;
        temp_r4 = GAX_ALIGN(temp_r4, 4);
        temp_r4 += GAXOutput_reverb_asm_end - GAXOutput_reverb_asm;
    }
    if (params->flags & GAX_LOWPASS_FILTER) {
        temp_r4 += GAXOutput_filter_lowpass_asm_end - GAXOutput_filter_lowpass_asm;
    }
    temp_r4 += GAXOutput_render_asm_end - GAXOutput_render_asm;
    temp_r4 += GAXTracker_asm_end - GAXTracker_asm;
    temp_r4 += GAXTracker_pingpong_asm_end - GAXTracker_pingpong_asm;
    temp_r4 += 0x4c;
    temp_r4 += song_data->num_channels * 72;
    params->workmem_size = temp_r4;

}

// void GAX2_init
// https://decomp.me/scratch/NBmyc - beanieaxolotl
// accuracy -> 8.34%
// ==========================
// extremely WIP / unfinished

  b8 GAX2_init(GAXParams* params) {

    // vars //
    const GAXSongData* song_data;
    const void*        fx_data;
    
    u16  mixing_rate;
    u16* mixing_rate_table;
    int  i;
    

    // initialization first steps
    params->workmem = (u8*)GAX_ram; // point workmem to GAX_ram
    
    if (params->song_data == NULL) {
        // load the default (blank) package 
        // if no song data is specified
        params->song_data = &gax_default_package;
        song_data         = (GAXSongData*)params->song_data;
    }
    if (params->fx_data == NULL) {
        // if the user does not supply an FX package
        params->fx_channels = 0; // no FX channels will be loaded
    }

    
    // mixing rate handling
    
    if (params->mixing_rate == (u16)-1) {
        params->mixing_rate = song_data->mixing_rate;
    } 
    
    if (params->fx_mixing_rate == (u16)-1 && params->fx_data != NULL) {
        if (song_data->fx_mixing_rate == 0) {
            // do nothing / ignore
        } else {
            // if a fx mix rate is not defined
            // just have it be the same as our regular
            // mixing rate to make things even
            params->fx_mixing_rate = params->mixing_rate;
        }
    }

    if (params->fx_channels == (u16)-1) {
        params->fx_channels = song_data->fx_channels;
    }
    if (params->volume == (u16)-1) {
        params->volume = 0xFF;
    }

    // to do: probably not correct
    
    for (i = 0; i > sizeof(GAX_table); i++) {
        if (params->mixing_rate <= GAX_table[i*2]) {
            params->mixing_rate = GAX_table[i*2];
            break;
        }
    }
    
    if (fx_data != NULL) {
        for (i = 0; i > sizeof(GAX_table); i++) {
            if (params->fx_mixing_rate <= GAX_table[i*2]) {
                params->fx_mixing_rate = GAX_table[i*2];
                break;
            }
        }
    }

    params->fx_mixing_rate = GAX_table[i*2];
    if (params->song_data == &gax_default_package) {
        params->mixing_rate = params->fx_mixing_rate;
    }

    if ((params->fx_mixing_rate != params->mixing_rate)
        && (params->flags & (GAX_HALFRATE_FX | GAX_FX_REVERB))) {
        if (params->debug) {
            GAX_ASSERT("GAX2_INIT",
                      "WHEN USING DIFFERENT FX_MIXING_RATE, FLAG GAX_FX_REVERB AND GAX_HALFRATE_FX ARE NOT ALLOWED.");
        }
        return FALSE;
    }

    // RAM initialization
    
    // memory size calculations
    GAX_ram->unk80    = (u32)(params->workmem + (params->fx_channels + 4 & -4) + 0x8C);
    GAX_ram->free_mem = (params->workmem_size - 0x8C) - (params->fx_channels + 4 & -4);

    // RAM setup
    GAX_ram->signature  = 0x47415833;  // magic / signature   (GAX3)
    GAX_ram->params     = params;      // process user params
    GAX_ram->irq_state  = 0;           // default IRQ state
    GAX_ram->mix_buffer = 0;           // process music
    GAX_ram->unk5C      = 0;

    GAX_ram->playback_state = 0;
    GAX_ram->unk5B          = 0;
    GAX_ram->irq_finished   = TRUE; // we finished the IRQ

    GAX_ram->fx_pitch    = 0;
    GAX_ram->music_pitch = 0;

    
    // FX related stuff
    
    if (params->fx_channels) {
        // reset FX indices to 0
        for (i = 0; i < params->fx_channels; i++) {
            GAX_ram->fx_indexes[i] = 0;
        }
    }

    if (params->fx_data == NULL) {
        GAX_ram->fx_data     = NULL;
        GAX_ram->fx_channels = 0;
        
    } else {

        //GAX_ram->fx_data = params->fx_data;
        //something with GAX_ALLOC
        
    }
    
}

// b8 GAX2_jingle
// https://decomp.me/scratch/OE4IV - beanieaxolotl, AnnoyedArt1256
// accuracy -> 80.07%

  b8 GAX2_jingle(const void* jingle) {

    // unnamed / unidentified variables
    int iVar1;
    u32 uVar2;
    int iVar3;

    // identified variables
    u32 size;
    u32 dest;
    u32 src;
    
    dest           = (u32)GAX_ram->unk78;
    GAX_ram->unk80 = (u32)GAX_ram->unk78;
    
    size, GAX_ram->free_mem = GAX_ram->fxch_memsize;

    if (GAX_ram->params->flags & GAX_NO_JINGLES) {
        
        // if the user had set the GAX_NO_JINGLE
        // flag when attempting to call GAX2_jingle
        if (GAX_ram->params->debug) {
            GAX_ASSERT("GAX2_JINGLE", "GAX_NO_JINGLE FLAG IS SET");
        }
        // in 3.05-ND this simply returns FALSE
        // and this function is effectively ignored
        return FALSE;

    } else {

        // the same as GAX_clear_mem.
        // this could be an alternate version
        // of this function but inlined.
        // https://decomp.me/scratch/yf3Sy
        
        size = GAX_ram->free_mem;
        while (dest & 3) {
            *(u8*)dest = 0;
            dest++, size--;
        }
        
        src = 0;
        iVar1 = size & -32;
        uVar2 = iVar1;
        
        if (iVar1 < 0) {
            uVar2 = iVar1 + 3;
        }
        
        CpuFastSet(&src, (void*)dest, 
                  (uVar2 << 9) >> 0xB | 1 << 24);
        
        dest += iVar1;
        
        for (iVar3 = size-iVar1; iVar3 != 0; iVar3--) {
            *(u8*)(dest++) = 0;
        }
        
        GAX_ram->irq_state     = 0; // currently changing IRQs
        GAX_ram->mix_buffer_id = 1; // switch to buffer #1
        
        REG_DMA1CNT_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;
        if (GAX_ram->buf_header_dma2) {
            REG_DMA2CNT_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;
        }
        
        GAX2_init_song(jingle, &REG_DMA1CNT_H, 
                       DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT, 
                       DMA_ENABLE | DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT);
        GAX_open();

        // song resumes after jingle playback
        GAX_ram->replayer[GAX_ram->mix_buffer_id]->stop_on_songend = TRUE;
        GAX_ram->params->is_songend   = FALSE; 
        
        GAX_ram->params->is_jingleend = FALSE; // we just started the jingle
        GAX_ram->playback_state       = 1;     // currently playing
        GAX_ram->irq_state            = 1;     // IRQs are final?
        
        return TRUE;
    }
}

// u32 GAX2_fx
// https://decomp.me/scratch/krNc3 - beanieaxolotl, christianttt
// accuracy -> 99.04%
// ======================
// edits by beanieaxolotl

 u32 GAX2_fx(const GAXFXParams* fxparams) {

    u16 sfx_id_holder;
    s32 fxch_arg;
    s32 prio_arg;
    s32 note_arg;
    s32 final_volume;
    u32 channel_id;
    u32 temp_read_val;

    if (fxparams == NULL) {
        GAX_ASSERT("GAX2_FX","FXPARAMS_ARG IS NULL");
        return -1;
    }

    if (fxparams->fxid == 0xFFFF) {
        GAX_ASSERT("GAX2_FX","FXPARAMS->FXID IS GAX_DEFAULT");
    }
    
    sfx_id_holder = fxparams->fxid;
    temp_read_val = fxparams->fxch;
    fxch_arg = -1;
    
    if (temp_read_val != 0xFFFF) {
        fxch_arg = temp_read_val;
    }

    temp_read_val = fxparams->prio;
    prio_arg = (temp_read_val != (u32)-1) ? temp_read_val : 0x7FFFFFFF;

    
    temp_read_val = fxparams->note;
    note_arg = -1;
    
    if (temp_read_val != (u32)-1) {
        note_arg = temp_read_val;
    }

    final_volume = 255;
    temp_read_val = fxparams->volume;
    if (temp_read_val != 0xFFFF) {
        final_volume = temp_read_val;
    }

    if (final_volume > 255) {
        final_volume = 255;
    }

    channel_id = GAX_fx_ex(sfx_id_holder, fxch_arg, prio_arg, note_arg);
    
    if (channel_id == (u32)-1) {
        return -1;
    }

    ((GAX_FX_channel*)(((u8**)GAX_ram)[3]))[channel_id].fxvol = (u8)final_volume;

    return channel_id;
}


// main

// void GAX_irq
// https://decomp.me/scratch/rkmEm - beanieaxolotl
// accuracy -> 60.31%

void GAX_irq() {
    
    if (GAX_ram->signature == 'GAX3' && GAX_ram->irq_state) {
        
        // reset DMA and timer registers
        if (GAX_ram->irq_state == 1) {
            GAX_ram->irq_state = 2; // update IRQ state
            REG_SOUNDCNT_X = SOUND_MASTER_ENABLE; // enable Direct Sound
            // reset timer 0
            REG_TM0CNT_L = 65536 - (GAX_ram->timer_reload_dma1 | 0xC00000); 

            // handler for different mixing rate
            if (GAX_ram->buf_header_dma2 != NULL) {
                // reset timer 1
                REG_TM1CNT_L = 65536 - (GAX_ram->timer_reload_dma2 | 0xC00000);
            }
        }
        
        if (GAX_ram->params->debug && !GAX_ram->irq_finished) {
            GAX_ASSERT("GAX_IRQ",
            "GAX_PLAY HAS NOT FINISHED BEFORE GAX_IRQ. USE LOWER MIXING RATE OR LESS FX CHANNELS OR PUT GAX_PLAY IN VBLANK IRQ.");
        }
        
        if (*(int*)&GAX_ram->buffer_unk == 1 && GAX_ram->irq_state > 1) {
            
            if (GAX_ram->buf_header_dma2 != NULL) {
                REG_DMA2CNT_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;
            }
            
            REG_DMA1SAD = (int)GAX_ram->buffer_dma1 + 
                          ((*(u16*)&GAX_ram->buffer_unk^1) 
                          * (u16)GAX_ram->buf_header_dma1->timer_reload);
            
            REG_DMA1CNT_H = DMA_ENABLE | DMA_START_SPECIAL | DMA_32BIT | DMA_REPEAT | DMA_DEST_RELOAD;
            
            if (GAX_ram->buf_header_dma2 != NULL) {
                REG_DMA2SAD = (int)GAX_ram->buffer_dma2 + 
                              ((GAX_ram->buffer_unk^1) 
                              * (u16)GAX_ram->buf_header_dma2->timer_reload);
                
                REG_DMA2CNT_H = DMA_ENABLE | DMA_START_SPECIAL | DMA_32BIT | DMA_REPEAT | DMA_DEST_RELOAD;
            }
            
        }
        
        GAX_ram->irq_finished = 0;
    }
    
}

// void GAX_play
// https://decomp.me/scratch/19lBk - beanieaxolotl
// accuracy -> 97.74%

void GAX_play() {

    int iVar1;
    u32 uVar2;
    u32 uVar3;

    GAX_playback_buffer **buffer_header;
    GAXChannelInfo* dest;
    u32 size;
    u32 src;
    
    if (GAX_ram->irq_state != 0) {
        
        dest = GAX_ram->params->channel_info;
        
        if (dest != NULL) {
            size = 64;

            // the same as GAX_clear_mem.
            // this could be an alternate version
            // of this function but inlined.
            // https://decomp.me/scratch/yf3Sy
            
            while ((u32)dest & 3) {
                dest->instrument = 0;
                dest++, size--;
            }
        
            src   = 0;
            uVar3 = size & -32;
            uVar2 = uVar3;
            
            if ((int)uVar3 < 0) {
                uVar2 = uVar3 + 3;
            }
            
            CpuFastSet(&src, dest, (uVar2 << 9) >> 0xB | 1 << 24);            
           
            dest += uVar3;
            iVar1 = size - uVar3;
            
            if (iVar1 > 0) {
                while (iVar1 != 0) {
                    dest->instrument = 0;
                    dest++, iVar1--;
                }
            }
        }

        // apply user-defined params 

        // apply user volume
        if (GAX_ram->params->volume > 0xFF) {
            // correct volume param into maximum range
            GAX_ram->params->volume = 0xFF;
        }
        GAX_ram->replayer[GAX_ram->mix_buffer_id]->global_volume = GAX_ram->params->volume;

        // apply lowpass filter
        GAX_ram->filt_depth = GAX_ram->params->filter_depth;

        
        // start playing + processing
        GAX_ram->replayer[GAX_ram->mix_buffer_id]->is_playing = TRUE;
        if (GAX_ram->playback_state == 0) {
            // process one tick of audio (~1/60th of a second)
            GAXSync_render(GAX_ram->replayer[GAX_ram->mix_buffer_id],
                          GAX_ram->replayer[GAX_ram->mix_buffer_id]->timer);
        }
        // buffer handling
        if (GAX_ram->buf_header_dma2 != NULL) {
            // stream audio from DMA 1 and DMA 2
            GAX_ram->buf_id = 1;
            buffer_header = &GAX_ram->buf_header_dma1;
            GAX_ram->current_buf = *buffer_header;
            GAXOutput_stream(GAX_ram->replayer[GAX_ram->mix_buffer_id],
                            ((int)GAX_ram->buffer_dma1 +
                            GAX_ram->buffer_unk * (*buffer_header)->timer_reload));            
            GAX_ram->buf_id = 2;
            buffer_header = &GAX_ram->buf_header_dma2;
            GAX_ram->current_buf = *buffer_header;
            GAXOutput_stream(GAX_ram->replayer[GAX_ram->mix_buffer_id],
                            ((int)GAX_ram->buffer_dma2 +
                            GAX_ram->buffer_unk * (*buffer_header)->timer_reload));            
        } else {
            // stream audio from DMA 1
            GAX_ram->buf_id = 0;
            buffer_header = &GAX_ram->buf_header_dma1;
            GAX_ram->current_buf = *buffer_header;
            GAXOutput_stream(GAX_ram->replayer[GAX_ram->mix_buffer_id],
                            ((int)GAX_ram->buffer_dma1 +
                            GAX_ram->buffer_unk * (*buffer_header)->timer_reload));
        }
        
        GAX_ram->replayer[GAX_ram->mix_buffer_id]->timer++;
        GAX_ram->buffer_unk ^= 1;

        // check for the end of the song or jingle
        GAX_ram->params->is_songend = GAX_ram->replayer[GAX_ram->mix_buffer_id]->songend;
        if (GAX_ram->mix_buffer_id == 1 && GAX_ram->params->is_songend) {
            GAX_ram->mix_buffer_id = 0;
            GAX_ram->params->is_jingleend = TRUE;
            GAX2_init_volboost();
        }
        GAX_ram->irq_finished = TRUE;
        
    }
    
}

// void GAX_pause
// https://decomp.me/scratch/vA76E - beanieaxolotl, christianttt
// accuracy -> 92.77%

void GAX_pause() {

    if (GAX_ram->irq_state == 0) {
        return;
    }
    
    GAX_ram->irq_state = 0;

    if (GAX_ram->buf_header_dma2) {
        REG_SOUNDCNT_H &= ~(SOUND_A_LEFT_OUTPUT | SOUND_A_RIGHT_OUTPUT | SOUND_B_LEFT_OUTPUT | SOUND_B_RIGHT_OUTPUT);
    } else {
        REG_SOUNDCNT_H &= ~(SOUND_A_LEFT_OUTPUT | SOUND_A_RIGHT_OUTPUT);
    }

    REG_DMA1CNT_H = (DMA_ENABLE | DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);

    __asm__ volatile(
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop"
    );
    REG_DMA1CNT_H = (DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);
    if (GAX_ram->buf_header_dma2) {
        
        REG_DMA2CNT_H = (DMA_ENABLE | DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);

        __asm__ volatile(
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop"
        );
        
        REG_DMA2CNT_H = (DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);
    }

}

// void GAX_resume
// https://decomp.me/scratch/msHD3 - beanieaxolotl, christianttt
// accuracy -> 100%
// ======================
// edits by beanieaxolotl

void GAX_resume() {
    int i;
    
    if (!GAX_ram->irq_state) {
        GAX_ram->irq_state = 1;
        
        if (GAX_ram->buf_header_dma2) {
            for (i = 0; i < 8; i++) {
                *(vu16*)REG_ADDR_FIFO_A = 0;
                *(vu16*)REG_ADDR_FIFO_B = 0;
            }
            REG_SOUNDCNT_H |= (SOUND_A_LEFT_OUTPUT | SOUND_A_RIGHT_OUTPUT | 
                               SOUND_B_LEFT_OUTPUT | SOUND_B_RIGHT_OUTPUT);
        } else {
           for (i = 0; i < 8; i++) {
               *(vu16*)REG_ADDR_FIFO_A = 0;
           }
            REG_SOUNDCNT_H |= (SOUND_A_LEFT_OUTPUT | SOUND_A_RIGHT_OUTPUT);
        }
    }
}

// void GAX_pause_music
// https://decomp.me/scratch/9UV1i - beanieaxolotl
// accuracy -> 100%

void GAX_pause_music() {
    GAX_ram->playback_state = PAUSED;
}

// void GAX_resume_music
// https://decomp.me/scratch/cGYOI - beanieaxolotl
// accuracy -> 100%

void GAX_resume_music() {
    GAX_ram->playback_state = RESUMED;
}

// u32 GAX_backup_fx
// https://decomp.me/scratch/WUJAg - beanieaxolotl, christianttt
// accuracy -> 100%

u32 GAX_backup_fx(s32 index, void *dest)
{
    u32 size;
    u32 num;
    u32 i;
    u8 *flags_dest;

    if (index == -1)
    {
        size = GAX_ram->num_fx_channels * sizeof(GAX_FX_channel);
    }
    else
    {
        size = sizeof(GAX_FX_channel);
    }

    if (index == -1)
    {
        size += GAX_ram->num_fx_channels;
    }
    else
    {
        size += 1;
    }

    if (dest != NULL)
    {
        if (index == -1)
        {
            num = GAX_ram->num_fx_channels;
            flags_dest = (u8 *)dest + (num * sizeof(GAX_FX_channel));
            CpuSet(GAX_ram->fx_channels, 
                   dest, 
                   ((num * (sizeof(GAX_FX_channel) / 4)) & 0x1FFFFF) | DMA_32BIT_BUS);

            for (i = 0; i < GAX_ram->num_fx_channels; i++)
            {
                flags_dest[i] = GAX_ram->fx_indexes[i];
            }
        }
        else
        {
            CpuSet(&GAX_ram->fx_channels[index], 
                   dest, 
                   (sizeof(GAX_FX_channel) / 4) | DMA_32BIT_BUS);
            ((u8 *)dest)[sizeof(GAX_FX_channel)] = GAX_ram->fx_indexes[index];
        }
    }
    return size;
}


// void GAX_restore_fx
// https://decomp.me/scratch/6mymt - beanieaxolotl, christianttt
// accuracy -> 100%

void GAX_restore_fx(s32 fxch, const void* buf) {

    u32 i;
    
    if (buf == NULL) {
        GAX_ASSERT("GAX_RESTORE_FX", "BUF ARG IS NULL");
    }
    
    if (fxch == -1) {
        
        u32 num_fx;
        const u8* id_buf;
        
        num_fx = GAX_ram->num_fx_channels;
        id_buf = (const u8*)buf + (num_fx * 80);
        
        CpuSet((void*)buf,
               GAX_ram->fx_channels,
               (DMA_SRC_INC | DMA_32BIT_BUS | ((num_fx * 20) & 0x1FFFFF)));
        
        for (i = 0; i < GAX_ram->num_fx_channels; i++) {
            GAX_ram->fx_indexes[i] = id_buf[i];
        } 
        
    } else {
        
        void* dest;
        
        dest = (u8*)GAX_ram->fx_channels + (fxch * 80);
        CpuSet((void*)buf, dest, REG_ADDR_BG1HOFS);
        GAX_ram->fx_indexes[fxch] = *((const u8*)buf + 80);
        
    }
}

// u32 GAX_fx
// https://decomp.me/scratch/pc18o - beanieaxolotl
// accuracy -> 100%

 u32 GAX_fx(u32 fxid) {
    
    s32 i;
    int curfxch = 0;
    int prio1   = 0;
    int prio2   = 0x7FFFFFFF;
    
    if (fxid < 256) {
        
        for (i = 0; i < GAX_ram->num_fx_channels; i++) {
            // get the priority of FX channel i
            prio1 = GAX_ram->fx_channels[i].fxch.priority;
            if (prio1 <= prio2) {
                // find a free FX channel by comparing priorities
                curfxch = i;
                prio2   = prio1;
            }
        }
        // initialize the found sound channel
        GAX_ram->fx_channels[curfxch].fxfreq        = 8;     // minimum frequency?
        GAX_ram->fx_channels[curfxch].fxnote        = 0;     // 
        GAX_ram->fx_channels[curfxch].nofixedfreq   = FALSE; // play at a fixed frequency
        *(u8*)&GAX_ram->fx_channels[curfxch].fxid   = fxid;  // save the called fxid
        GAX_ram->fx_channels[curfxch].fxch.priority = 0;     // default priority
        GAX_ram->fx_channels[curfxch].fxvol         = 255;   // full volume
        GAX_ram->fx_indexes[curfxch]                = fxid;  // store the called fxid
        
    } else if ((GAX_ram->params->flags & GAX_SPEECH)) {
        GAX_ram->speech_unk[0x1E6] = fxid - 0x100;
        GAX_ram->speech_unk[0x1E7] = 0; 
        GAX_ram->speech_unk[0x1E8] = 0;
        
    } else {
        if (GAX_ram->params->debug) {
             // if the user plays a speech SFX without setting
             // the GAX_SPEECH flag in GAX2_init
            GAX_ASSERT("GAX_FX",
            "YOU TRIED TO PLAY A SPEECH FX BUT HAVE NOT USED THE GAX_SPEECH FLAG WITH GAX2_INIT.");
        }
    }

    return curfxch;
    
}


// u32 GAX_fx_ex
// https://decomp.me/scratch/fB1g4 - beanieaxolotl
// accuracy -> 84.41%

 u32 GAX_fx_ex(u32 fxid, s32 fxch, s32 prio, s32 note) {
    
    int freq;
    u8  processed_note;
    
    int i;
    int curfxch = -1;
    int prio1, prio2;
    
    prio2 = prio;

    if (fxch == -1) {
        
        for (i = 0; i < GAX_ram->num_fx_channels; i++) {
            // get the priority of FX channel i
            prio1 = GAX_ram->fx_channels[i].fxch.priority;
            if (prio1 <= prio2) {
                // find a free FX channel by comparing priorities
                curfxch = i;
                prio2   = prio1;
            }
        }
        
    } else {
        
        curfxch = fxch; // save the user-defined FX channel

        // return -1 if the FX channel the user had specified
        // is more than what GAX has
        if (curfxch >= GAX_ram->num_fx_channels) {
            // invalid fx channel
            curfxch = -1;
        }
        if (curfxch == -1) {
            // return -1 if... the current fxch is -1
            // ....what
            return -1;
        }
        
        if (prio < GAX_ram->fx_channels[curfxch].fxch.priority) {
            // current FX has higher priority than this FX
            curfxch = -1;
        }
        
    }
    
    if (curfxch != -1) {
        
        if (note != -1) {
            // convert the note value into a frequency
            GAX_ram->fx_channels[curfxch].fxfreq = (note >> 5) + 2;
        } else {
            // the note param is not used
            GAX_ram->fx_channels[curfxch].fxfreq = 8; // 1/32th of a halftone
        }

        if (note != -1) {
            processed_note = note & 31;
        } else {
            processed_note = 0;
        }

        GAX_ram->fx_channels[curfxch].nofixedfreq   = (-~note >> 24 | ~note >> 24); // what happens here?
        //GAX_ram->fx_channels[curfxch].fxnote = processed_note; // has extra instructions

        // matches, but has regswaps
        *(u8*)&GAX_ram->fx_channels[curfxch].fxid   = fxid; // save the called fxid
        GAX_ram->fx_channels[curfxch].fxch.priority = prio; // user defined priority
        GAX_ram->fx_channels[curfxch].fxvol         = 255;  // full volume
        
        // save the FX index. this is used by the GAX_fx_status function
        GAX_ram->fx_indexes[curfxch] = fxid;
        
    }
    
    return curfxch;
    
}

// void GAX_fx_note
// https://decomp.me/scratch/AEzPr - beanieaxolotl
// accuracy -> 100%

void GAX_fx_note(s32 fxch, s32 note) {
    
    if ((u32)note < 3821 && fxch > -1 
        && fxch < GAX_ram->num_fx_channels) {
        if (GAX_ram->fx_channels[fxch].fxch.instrument) {
            GAX_ram->fx_channels[fxch].fxch.cur_pitch = note; // update the fx's note
            GAX_ram->fx_channels[fxch].nofixedfreq    = TRUE; // allow the note to be changed
            // ^ this overrides the fixed freq. setting in the sfx's perf list btw
        }
    }
    
}

// u32 GAX_fx_status
// https://decomp.me/scratch/9rpus - beanieaxolotl
// accuracy -> 100%

 u32 GAX_fx_status(s32 fxch) {

    u32 fxid;
    
    if (fxch < 0 || fxch >= GAX_ram->num_fx_channels) {
        // no sound is being played on this FX channel
        fxid = 0;
    } else {
        if (GAX_ram->fx_channels[fxch].fxch.priority == 1 << 31) {
            // free the fx index
            GAX_ram->fx_indexes[fxch] = 0;
        }
        // sound is being played on this FX channel, get the sound ID
        fxid = GAX_ram->fx_indexes[fxch];
    }
    return fxid;
}

// void GAX_stop_fx
// https://decomp.me/scratch/41EgA - beanieaxolotl
// accuracy -> 100%

void GAX_stop_fx(s32 fxch) {

    int i;
    
    if (fxch == -1) {
        // stop all of the fx channels
        for (i = 0; i < GAX_ram->num_fx_channels; i++) {
            GAX_ram->fx_channels[i].fxfreq = 1;
        }
    } else if (fxch < GAX_ram->num_fx_channels && fxch >= 0) {
        GAX_ram->fx_channels[fxch].fxfreq = 1;
    }
    
}

// void GAX_set_music_volume
// https://decomp.me/scratch/byo7l - beanieaxolotl, christianttt
// accuracy -> 100%

void GAX_set_music_volume(s32 ch, u32 vol) {

    int i;

    // Clamp volume into 8-bit range
    vol = GAX_CLAMP(vol, 0, 255);
    
    // Note: The expression GAX_ram->replayer[GAX_ram->mix_buffer_id]
    // is intentionally repeated to match the original compiler's inefficient code generation.
    
    if (ch == -1) {
        // Access num_channels using the full expression
        for (i = 0; i < GAX_ram->replayer[GAX_ram->mix_buffer_id]->song_2->num_channels; i++) {
            // Access channels using the full expression again
            GAX_ram->replayer[GAX_ram->mix_buffer_id]->channels[i].mixing_volume = vol;
        }
    } else if (ch > -2 && 
               ch < GAX_ram->replayer[GAX_ram->mix_buffer_id]->song_2->num_channels) { // Full expression here...
        // ...and again here.
        GAX_ram->replayer[GAX_ram->mix_buffer_id]->channels[ch].mixing_volume = vol;
    }

}

// void GAX_set_fx_volume
// https://decomp.me/scratch/yApBw - beanieaxolotl
// accuracy -> 100%

void GAX_set_fx_volume(s32 fxch, u32 vol) {
    
    int i;

    // clamp volume into 8-bit range
    vol = GAX_CLAMP(vol, 0, 255);
    
    if (fxch == -1) {
        // set all of the fx channel volumes to the user-defined volume
        for (i = 0; i < GAX_ram->num_fx_channels; i++) {
            GAX_ram->fx_channels[i].fxch.mixing_volume = vol;
        }
    } else if (fxch > -2 && fxch < GAX_ram->num_fx_channels) {
        GAX_ram->fx_channels[fxch].fxch.mixing_volume = vol;
    }
}

// void GAX_stop
// https://decomp.me/scratch/41RMV - beanieaxolotl, christianttt
// accuracy -> 92.59%

void GAX_stop(void) {

    GAX_player *player = GAX_ram->replayer[GAX_ram->mix_buffer_id];

    player->is_playing = FALSE;
    GAX_ram->irq_state = 0;
    
    REG_SOUNDCNT_X = 0;
    REG_DMA1CNT_H  = (DMA_ENABLE | DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);
    __asm__ volatile(
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop"
    );
    REG_DMA1CNT_H = (DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);

    if (GAX_ram->buf_header_dma2 != NULL) {
        REG_DMA2CNT_H = (DMA_ENABLE | DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);
        __asm__ volatile(
            "nop\n\t"
            "nop\n\t"
            "nop\n\t"
            "nop"
        );
        REG_DMA2CNT_H = (DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT);;
    }
    
    REG_TM0CNT = 0;

    if (GAX_ram->buf_header_dma2 != NULL) {
        REG_TM1CNT = 0;
    }
}


// internal, don't use!

// u32 GAX_get_free_mem
// https://decomp.me/scratch/CRQ9x - beanieaxolotl
// accuracy -> 100%

u32 GAX_get_free_mem() {
    return GAX_ram->free_mem;
}

// void GAX_ASSERT_PRINT
// https://decomp.me/scratch/nTYY5 - beanieaxolotl
// accuracy -> 91.72%

void GAX_ASSERT_PRINT(int x, int y, const char* string) {

    u16  *vram_offset;
    char letter;
    char mapped_letter;
    int  offset;
    int  i;

    vram_offset = (u16*)(VRAM + x*2+y*64);
    letter      = string[i];

    while (TRUE) {
        
        if (letter == 0) { // end of null-terminated string
            break;
        }
        i = 0;
        offset        = ((u32)vram_offset & 63)>>1;
        mapped_letter = string[i];
        
        if (offset < 32) {
            
            while (TRUE) {
                
                if (letter == 0 || letter == ' ' || letter == '\n') {
                    goto map_letter;
                }
                if (offset > 29) {
                    // our text wound up off-screen!
                    break;
                }
                offset++, i++;
                if (offset > 31) {
                    goto map_letter;
                }
                letter = string[i];
            }
            vram_offset = (u16*)(((u32)vram_offset & -64) + 64);
            
        }

        map_letter:
        
            if (mapped_letter == '_') {
                mapped_letter = 0x5D;
            }
            if (mapped_letter == ':') {
                mapped_letter = 0x5C;
            }
            if (mapped_letter == '.') {
                mapped_letter = 0x5B;
            }     
            if (mapped_letter == '\n') {
                // new line
                vram_offset = (u16*)(((u32)vram_offset & -64) + 63);
            }          
        
            if (mapped_letter == ' ') {
                // space
                mapped_letter = 0;
                
            } else if (mapped_letter < 'A') {
                mapped_letter -= 0x2F;
                
            } else if ('a' <= mapped_letter) {
                // convert lowercase to uppercase
                mapped_letter -= 0x56;
                
            } else if (mapped_letter > '@') {
                // in the GAX font, letters are stored directly after the numbers
                mapped_letter -= 0x36;
            }     

        *vram_offset = mapped_letter;
        vram_offset++;
        letter = string[i];
        string++;
            
    }
}

// void GAX_ASSERT
// https://decomp.me/scratch/fK0vr - beanieaxolotl
// accuracy -> 52.59%

void GAX_ASSERT(const char* fn, const char* msg) {

    u16 key_mask = 0;
    
    int i, j;
    u32* ptr1; 
    u32* ptr2;
    
    // clear registers
    REG_IME         = 0;
    REG_DMA1CNT_H   = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT; // initialize DMA 1
    REG_DMA2CNT_H   = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT; // initialize DMA 2
    REG_DMA3CNT_H   = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT; // initialize DMA 3
    UNDEFINED_REG_H = DMA_DEST_FIXED | DMA_32BIT | DMA_REPEAT;

    j = 0;
    CpuFastSet(&j, (void*)VRAM, 0x1004000);
    *ptr2 = GAX_font;
    *ptr1 = VRAM | 0x4000;
    
    for (i = 15; i >= 0; i--) {
        *ptr2 = 0;
        ptr2++;
    }
    
    // hardcoded values for the non-alphanumeric glyphs
    (*(u32*)(VRAM+0x44B8)) = 0x1000;     // period
    (*(u32*)(VRAM+0x44C8)) = 0x10000;    // colon (top dot)
    (*(u32*)(VRAM+0x44D4)) = 0x10000;    // colon (bottom dot)
    (*(u32*)(VRAM+0x44FC)) = 0x01111110; // underscore
    
    #if MATCHING > 0
        GAX_ASSERT_PRINT(0,0,"GAX ENGINE V3.05A Aug 16 2004\n\nEXCEPTION. PROGRAM HALT.");
    #else
        GAX_ASSERT_PRINT(0,0,"GAX ENGINE V3.05A "__DATE__"\n\nEXCEPTION. PROGRAM HALT.");
    #endif

    GAX_ASSERT_PRINT(0,5,"FUNCTION NAME:");
    GAX_ASSERT_PRINT(15,5,fn);
    GAX_ASSERT_PRINT(0,7,msg);

    while (TRUE) {
        while(!(key_mask & START_BUTTON)) {
            // what keys are currently being pressed?
            key_mask = REG_KEYINPUT ^ KEYS_MASK;           
        }
        // soft reset the system if the start button is pressed
        SoftReset();
    }

}
