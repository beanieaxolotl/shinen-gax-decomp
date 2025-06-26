#include "gax.h"
#include "gax_assert.h"

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


void GAX2_init_song() {}

// void GAX2_init_soundhw
// https://decomp.me/scratch/mBHfv - beanieaxolotl
// accuracy -> 44.94%

void GAX2_init_soundhw() {
    
    int i;

    if (GAX_ram->dma2cnt_unk == NULL) {
        REG_SOUNDCNT_H = 0xB04;
        for (i = 7; i > -1; i--) {
            REG_FIFO_A = i;
        }
        
    } else {
        REG_DMA2CNT_L = 0;
        REG_SOUNDCNT_H = 0xFB0C;
        for (i = 7; i > -1; i--) {
            REG_FIFO_B = 0;
        }
        REG_DMA2DAD = REG_FIFO_B;
        REG_DMA2SAD = GAX_ram->dma2sad;
    }

    REG_DMA1CNT_L  = 4;
    REG_FIFO_A     = 0;
    REG_SOUNDCNT_X = 0;
    REG_SOUNDBIAS  = 0x42;

    REG_DMA1DAD = REG_FIFO_A;
    REG_DMA1SAD = GAX_ram->dma1sad_unk;
    
}

void GAX2_init_volboost() {}
void GAX_clear_mem() {}

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
    
    
    params->mixing_rate = -1;
    params->fx_mixing_rate = -1;
    params->filter_depth = 0;
    params->fx_channels = -1;
    params->volume = -1;
    params->debug = TRUE;

}

// void GAX2_new_fx
// https://decomp.me/scratch/ffZpU - beanieaxolotl
// accuracy -> 100%

void GAX2_new_fx(GAXFXParams* fxparams) {
    
    if (fxparams == NULL) {
        GAX_ASSERT("GAX2_NEW_FX", "FXPARAMS ARG IS NULL");
    } else {
        fxparams->fxid   = 0xFFFF;
        fxparams->fxch   = 0xFFFF;
        fxparams->prio   = -1;
        fxparams->note   = 0xFFFFFFFF;
        fxparams->volume = 0xFFFF;
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

  b8 GAX2_init(GAXParams* params) {}
  b8 GAX2_jingle(const void* jingle) {}

// u32 GAX2_fx
// https://decomp.me/scratch/krNc3
// accuracy -> 64.79%

 u32 GAX2_fx(const GAXFXParams* fxparams) {

    s32 prio;
    s16 volume;
    s32 fxch;
    u32 note;
    s32 temp;
    
    if (fxparams == NULL) {
        GAX_ASSERT("GAX2_FX", "FXPARAMS_ARG IS NULL");
        
    } else if (fxparams->fxid == (u32)-1) {
        GAX_ASSERT("GAX2_FX","FXPARAMS->FXID IS GAX_DEFAULT");
        
    } else {
        
        fxch   = -1;
        prio   = INT32_MAX;
        note   = -1;
        
        if (fxparams->fxch != (u16)-1) {
            fxch = fxparams->fxch;
        }
        if (fxparams->prio != -1) {
            prio = fxparams->prio;
        }                
        if (fxparams->note != -1) {
            note = fxparams->note;
        }           
        if (fxparams->volume != (u16)-1) {
            volume = fxparams->volume;
        }
        if (volume > 255) {
            volume = 255;
        }
        
        temp = GAX_fx_ex(fxparams->fxid, fxch, prio, note);
        if (temp != -1) {
            *(u8*)&GAX_ram->fx_channels[temp].fxvol = volume;
            return temp;
        }    
    }
    // no free FX channel is found / 
    // sound is at higher prio than fxparams->prio
    return -1;
    
}

// main
void GAX_irq() {}
void GAX_play() {}

// void GAX_pause
// https://decomp.me/scratch/zama3 - beanieaxolotl
// accuracy -> 71.79%

void GAX_pause() {

    if (GAX_ram->unk_state) {
        GAX_ram->unk_state = 0;

        if (GAX_ram->dma2cnt_unk) {
            REG_SOUNDCNT_H &= 0xCCFF;
        } else {
            REG_SOUNDCNT_H &= 0xFCFF;
        }
        
        REG_DMA1CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT;
        if (GAX_ram->dma2cnt_unk) {
            REG_DMA2CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT;
        }
    }
}

// void GAX_resume
// https://decomp.me/scratch/sOHcc - beanieaxolotl
// accuracy -> 83.59%

void GAX_resume() {

    int i;
    
    if (!GAX_ram->irq_state) {
        GAX_ram->irq_state = 1;
        
        if (GAX_ram->dma2cnt_unk) {
            
            for (i = 7; i >= 0; i--) {
                // resume FIFOs used for different mix rates + playback
                REG_FIFO_A = 0;
                REG_FIFO_B = 0;
            }
            REG_SOUNDCNT_H |= 0x300;
            
        } else {
            
            for (i = 7; i >= 0; i--) {
                // resume FIFO for playback
                REG_FIFO_A = 0;
            }
            REG_SOUNDCNT_H |= 0x3300;
        }
    }
}

// void GAX_pause_music
// https://decomp.me/scratch/9UV1i - beanieaxolotl
// accuracy -> 100%

void GAX_pause_music() {
    GAX_ram->music_playback_state = PAUSED;
}

// void GAX_resume_music
// https://decomp.me/scratch/cGYOI - beanieaxolotl
// accuracy -> 100%

void GAX_resume_music() {
    GAX_ram->music_playback_state = RESUMED;
}

// https://decomp.me/scratch/pXSjo - beanieaxolotl
// accuracy -> 77.77%

u32 GAX_backup_fx(s32 fxch, void* buf) {
    
    int i;
    int temp;
    u32 buf_size;

    if (fxch == -1) {
        // size of all of the allocated fx channels
        buf_size = (GAX_ram->num_fx_channels * sizeof(GAX_FX_channel));
    } else {
        // size of one allocated fx channel
        buf_size = sizeof(GAX_FX_channel);
    }

    // only do this if the buffer is not NULL,
    // otherwise this just gives you the buffer size
    
    if (buf != NULL) {
        if (fxch == -1) {
            // backup all fx channels
            temp = GAX_ram->num_fx_channels;
            CpuSet(GAX_ram->fx_channels, buf, (temp * 0x14 & 0x1FFFFF) | REG_DISPCNT);
            for (i = 0; i < GAX_ram->num_fx_channels; i++) {
                // to do: this is extremely unclear
                *(u8*)(buf + (i * sizeof(GAX_FX_channel))) = *(u8*)((u32)GAX_ram->fxch + (i-0xC));
            }
        } else {
            CpuSet(GAX_ram->fx_channels+fxch, buf, REG_BG1HOFS);
            *(u8*)(buf + sizeof(GAX_FX_channel)) = *(u8*)((int)GAX_ram->fxch + (fxch-0xC));
        }
    }

    return buf_size;
}

void GAX_restore_fx(s32 fxch, const void* buf) {}
 u32 GAX_fx(s32 fxid) {}
 u32 GAX_fx_ex(u32 fxid, s32 fxch, s32 prio, s32 note) {}

// void GAX_fx_note
// https://decomp.me/scratch/AEzPr - beanieaxolotl
// accuracy -> 93.97%

void GAX_fx_note(s32 fxch, s32 note) {
    
    if (note < 3821 && fxch > -1 
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
// accuracy -> 96.14%

 u32 GAX_fx_status(s32 fxch) {

    u32 current_sound;
    
    if (fxch < 0 || fxch >= GAX_ram->num_fx_channels) {
        // no sound is being played on this FX channel
        current_sound = 0;
    } else {
        // to do: this is kind of awkward and unreadable
        // ...what does this even do?
        if (GAX_ram->fx_channels[fxch].fxch.priority == 1 << 31) {
            *(u8 *)((s32)GAX_ram->fxch + fxch-0xC) = 0;
        }
        // sound is being played on this FX channel, get the sound ID
        current_sound = *(u8 *)(u32*)(fxch-0xC + (int)GAX_ram->fxch);
    }
    return current_sound;
     
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

void GAX_set_music_volume(s32 ch, u32 vol) {}

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
// https://decomp.me/scratch/G5819 - beanieaxolotl
// accuracy -> 57.78%

void GAX_stop() {

    // to do: what does this do?
    *(u8 *)(*(int *)(GAX_ram->signature + (int)GAX_ram->unk20 * 4 + 0x18) + 0x12) = 0;
    
    GAX_ram->irq_state = 0;
    REG_SOUNDCNT_X     = 0; // turn DirectSound off

    // set dma values
    REG_DMA1CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT;
    if (GAX_ram->dma2cnt_unk) {
        REG_DMA2CNT_H = DMA_DEST_FIXED | DMA_REPEAT | DMA_32BIT;
    }
    
    // disable timer 0
    REG_TM0CNT_L = 0;
    if (GAX_ram->dma2cnt_unk) {
        // disable timer 1
        REG_TM1CNT_L = 0;
    }
}


// internal, don't use!

// u32 GAX_get_free_mem
// https://decomp.me/scratch/CRQ9x - beanieaxolotl
// accuracy -> 100%

u32 GAX_get_free_mem() {
    return GAX_ram->free_mem;
}
