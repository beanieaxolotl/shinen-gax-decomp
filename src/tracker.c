#include "gax.h"
#include "gax_internal.h"


// constants

// vibrato table

const s8 GAX_vibtable[64] = {

    // 64-entry sine wave table

       0,   12,   24,   37, 
      48,   60,   70,   80, 
      90,   98,  106,  112,
     117,  122,  125,  126,
     127,  126,  125,  122,
     117,  112,  106,   98,
      90,   80,   70,   60,
      48,   37,   24,   12,
       0,  -12,  -24,  -37, 
     -48,  -60,  -70,  -80, 
     -90,  -98, -106, -112,
    -117, -122, -125, -126,
    -127, -126, -125, -122,
    -117, -112, -106,  -98,
     -90,  -80,  -70,  -60,
     -48,  -37,  -24,  -12

};

// frequency table

const u32 GAX_freqtable[12 * 32] = {

    /*  12 * 32 LUT of frequencies.
        12 semitones divided into 1/32ths. */

    // C-0
    0x105A0250, 0x10619268, 0x10692600, 0x1070BD18,
    0x107857B4, 0x107FF5D3, 0x10879779, 0x108F3CA6,
    0x1096E55C, 0x109E919E, 0x10A6416B, 0x10ADF4C7,
    0x10B5ABB3, 0x10BD6630, 0x10C52440, 0x10CCE5E5,
    0x10D4AB21, 0x10DC73F4, 0x10E44061, 0x10EC1069,
    0x10F3E40F, 0x10FBBB53, 0x11039638, 0x110B74BE,
    0x111356E9, 0x111B3CB9, 0x11232630, 0x112B1350,
    0x1133041A, 0x113AF890, 0x1142F0B5, 0x114AEC89,

    // C#0
    0x1152EC0E, 0x115AEF46, 0x1162F633, 0x116B00D7,
    0x11730F32, 0x117B2148, 0x11833719, 0x118B50A7,
    0x11936DF5, 0x119B8F03, 0x11A3B3D4, 0x11ABDC69,
    0x11B408C4, 0x11BC38E6, 0x11C46CD3, 0x11CCA48A,
    0x11D4E00F, 0x11DD1F62, 0x11E56286, 0x11EDA97C,
    0x11F5F446, 0x11FE42E6, 0x1206955D, 0x120EEBAE,
    0x121745DA, 0x121FA3E3, 0x122805CB, 0x12306B93,
    0x1238D53E, 0x124142CD, 0x1249B441, 0x1252299D,

    // D-0
    0x125AA2E3, 0x12632014, 0x126BA133, 0x12742640,
    0x127CAF3E, 0x12853C2E, 0x128DCD13, 0x129661EE,
    0x129EFAC2, 0x12A7978F, 0x12B03858, 0x12B8DD1E,
    0x12C185E4, 0x12CA32AC, 0x12D2E376, 0x12DB9846,
    0x12E4511C, 0x12ED0DFB, 0x12F5CEE4, 0x12FE93DB,
    0x13075CDF, 0x131029F4, 0x1318FB1A, 0x1321D055,
    0x132AA9A5, 0x1333870E, 0x133C6890, 0x13454E2D,
    0x134E37E8, 0x135725C2, 0x136017BD, 0x13690DDC,

    // D#0
    0x13720820, 0x137B068A, 0x1384091E, 0x138D0FDC,
    0x13961AC7, 0x139F29E1, 0x13A83D2B, 0x13B154A8,
    0x13BA705A, 0x13C39042, 0x13CCB462, 0x13D5DCBD,
    0x13DF0954, 0x13E83A29, 0x13F16F3E, 0x13FAA896,
    0x1403E632, 0x140D2814, 0x14166E3E, 0x141FB8B2,
    0x14290772, 0x14325A80, 0x143BB1DE, 0x14450D8F,
    0x144E6D93, 0x1457D1EE, 0x14613AA0, 0x146AA7AD,
    0x14741915, 0x147D8EDC, 0x14870903, 0x1490878C,

    // E-0
    0x149A0A79, 0x14A391CC, 0x14AD1D87, 0x14B6ADAD,
    0x14C0423F, 0x14C9DB40, 0x14D378B0, 0x14DD1A94,
    0x14E6C0EC, 0x14F06BBA, 0x14FA1B01, 0x1503CEC3,
    0x150D8701, 0x151743BE, 0x152104FD, 0x152ACABE,
    0x15349504, 0x153E63D2, 0x15483729, 0x15520F0B,
    0x155BEB7B, 0x1565CC7A, 0x156FB20B, 0x15799C30,
    0x15838AEB, 0x158D7E3D, 0x1597762A, 0x15A172B4,
    0x15AB73DB, 0x15B579A4, 0x15BF840F, 0x15C9931F,

    // F-0
    0x15D3A6D6, 0x15DDBF36, 0x15E7DC41, 0x15F1FDFA,
    0x15FC2462, 0x16064F7C, 0x16107F4B, 0x161AB3CF,
    0x1624ED0C, 0x162F2B03, 0x16396DB7, 0x1643B52A,
    0x164E015E, 0x16585255, 0x1662A812, 0x166D0296,
    0x167761E4, 0x1681C5FF, 0x168C2EE7, 0x16969CA1,
    0x16A10F2D, 0x16AB868E, 0x16B602C6, 0x16C083D8,
    0x16CB09C6, 0x16D59491, 0x16E0243D, 0x16EAB8CB,
    0x16F5523E, 0x16FFF098, 0x170A93DC, 0x17153C0B,

    // F#0
    0x171FE927, 0x172A9B34, 0x17355233, 0x17400E27,
    0x174ACF12, 0x175594F6, 0x17605FD6, 0x176B2FB3,
    0x17760491, 0x1780DE71, 0x178BBD56, 0x1796A143,
    0x17A18A38, 0x17AC783A, 0x17B76B49, 0x17C26369,
    0x17CD609C, 0x17D862E4, 0x17E36A44, 0x17EE76BD,
    0x17F98852, 0x18049F06, 0x180FBADB, 0x181ADBD4,
    0x182601F2, 0x18312D38, 0x183C5DA9, 0x18479346,
    0x1852CE13, 0x185E0E11, 0x18695343, 0x18749DAC,

    // G-0
    0x187FED4E, 0x188B422B, 0x18969C45, 0x18A1FBA0,
    0x18AD603D, 0x18B8CA20, 0x18C43949, 0x18CFADBD,
    0x18DB277D, 0x18E6A68B, 0x18F22AEB, 0x18FDB49F,
    0x190943A8, 0x1914D80A, 0x192071C8, 0x192C10E3,
    0x1937B55D, 0x19435F3B, 0x194F0E7D, 0x195AC327,
    0x19667D3B, 0x19723CBB, 0x197E01AB, 0x1989CC0C,
    0x19959BE1, 0x19A1712C, 0x19AD4BF1, 0x19B92C31,
    0x19C511F0, 0x19D0FD2F, 0x19DCEDF1, 0x19E8E43A,

    // G#0
    0x19F4E00A, 0x1A00E166, 0x1A0CE84E, 0x1A18F4C7,
    0x1A2506D3, 0x1A311E74, 0x1A3D3BAC, 0x1A495E7F,
    0x1A5586EF, 0x1A61B4FF, 0x1A6DE8B0, 0x1A7A2207,
    0x1A866104, 0x1A92A5AC, 0x1A9EF000, 0x1AAB4004,
    0x1AB795B9, 0x1AC3F123, 0x1AD05244, 0x1ADCB91F,
    0x1AE925B6, 0x1AF5980C, 0x1B021024, 0x1B0E8E01,
    0x1B1B11A4, 0x1B279B11, 0x1B342A4B, 0x1B40BF54,
    0x1B4D5A2E, 0x1B59FADD, 0x1B66A163, 0x1B734DC3,

    // A-0
    0x1B800000, 0x1B8CB81B, 0x1B997619, 0x1BA639FC,
    0x1BB303C6, 0x1BBFD37A, 0x1BCCA91B, 0x1BD984AC,
    0x1BE6662F, 0x1BF34DA7, 0x1C003B17, 0x1C0D2E82,
    0x1C1A27EA, 0x1C272753, 0x1C342CBE, 0x1C41382F,
    0x1C4E49A9, 0x1C5B612E, 0x1C687EC2, 0x1C75A266,
    0x1C82CC1E, 0x1C8FFBEC, 0x1C9D31D4, 0x1CAA6DD8,
    0x1CB7AFFB, 0x1CC4F840, 0x1CD246A9, 0x1CDF9B3A,
    0x1CECF5F6, 0x1CFA56DE, 0x1D07BDF7, 0x1D152B42,

    // A#0
    0x1D229EC4, 0x1D30187E, 0x1D3D9874, 0x1D4B1EA8,
    0x1D58AB1D, 0x1D663DD7, 0x1D73D6D7, 0x1D817622,
    0x1D8F1BBA, 0x1D9CC7A1, 0x1DAA79DC, 0x1DB8326C,
    0x1DC5F154, 0x1DD3B698, 0x1DE1823B, 0x1DEF543F,
    0x1DFD2CA7, 0x1E0B0B77, 0x1E18F0B1, 0x1E26DC59,
    0x1E34CE70, 0x1E42C6FB, 0x1E50C5FC, 0x1E5ECB76,
    0x1E6CD76D, 0x1E7AE9E2, 0x1E8902DA, 0x1E972257,
    0x1EA5485C, 0x1EB374ED, 0x1EC1A80C, 0x1ECFE1BC,

    // B-0
    0x1EDE2200, 0x1EEC68DC, 0x1EFAB652, 0x1F090A66,
    0x1F17651A, 0x1F25C672, 0x1F342E70, 0x1F429D18,
    0x1F51126D, 0x1F5F8E72, 0x1F6E112A, 0x1F7C9A98,
    0x1F8B2AC0, 0x1F99C1A3, 0x1FA85F47, 0x1FB703AC,
    0x1FC5AED8, 0x1FD460CC, 0x1FE3198C, 0x1FF1D91B,
    0x20009F7D, 0x200F6CB4, 0x201E40C3, 0x202D1BAE,
    0x203BFD79, 0x204AE625, 0x2059D5B6, 0x2068CC30,
    0x2077C996, 0x2086CDEA, 0x2095D931, 0x20A4EB6D   

};


// b8 GAXTracker_generate_audio
// https://decomp.me/scratch/zDrka - beanieaxolotl
// accuracy -> 56.46%
// ==========================
// extremely WIP / unfinished

b8 GAXTracker_generate_audio(GAX_channel* ch, GAX_player* replayer, GAXSongData* song_data, u32 flag) {

    int* wave_addr;
    
    int temp, temp1, temp2;
    int new_wave_position;
    u8  volenv_volume, wave_volume, instrument_volume, mixing_volume, global_volume;
    b8  use_modulation;
    GAX_instrument *instrument;
    u8* memsize;
    
    // unidentified / temp
    int  unk_uint_1;
    u16* psVar2;
    int  local_30;
    int  local_38;
    u32  timer_reload;
    int  local_48;
    u32  uVar1;

    b8 enable_modulation;
    
    // this seems to check if we can generate audio, judging by it returning either TRUE or FALSE
    
    if (!ch->instrument || ch->semitone_pitch == -30000 || ch->waveslot_idx >= 4) {
        /* return 0 if:
           there is no instrument playing
           the semitone pitch is -30000
           the wave slot index is more than 0x3 (a failsafe measure) */
        return FALSE;
    }
    
    /* get the ROM address of the instrument's current waveform.
       this is typically set to after the instrument data. */
    wave_addr = (void*)song_data->waveform_data + (ch->instrument->waves[ch->waveslot_idx]*8);
    if (wave_addr == NULL) {
        // if we encounter an unused waveform, discard immediately
        return FALSE;
    }

    // temp is the channel's pitch. we also apply the calculated vibrato pitch
    temp = ch->semitone_pitch + ch->vibrato_pitch;
    if (!ch->is_fixed && (temp = temp+ch->cur_pitch, temp1 = 0)) {
        // apply the pattern's transpose value to the base channel pitch
        temp += ch->order[replayer->pattern].transpose * 32;
    }

    // apply finetune
    temp += ch->instrument->wave_params[ch->waveslot_idx].tune;
    GAX_CLAMP(temp, 0, 0xEF1); // clamp the pitch into bounds
   
    if (temp >= 0xD80) {
        temp = GAX_freqtable[temp - 0xD80] << 1;
    } else {
        temp2 = 0;
        for (temp -= 3072; temp < 0; temp += 384) {
            temp2++;
        }
        temp = GAX_freqtable[temp] >> temp2;
    }

    // set the volume variables
    volenv_volume     = ch->volenv_volume;
    wave_volume       = ch->wave_volume;
    instrument_volume = ch->instrument_volume;
    mixing_volume     = ch->mixing_volume;
    global_volume     = replayer->global_volume;

    // mixing volume handler
    unk_uint_1 = temp1*4;
    temp2 = (((mixing_volume * ((((wave_volume * volenv_volume) >> 8) * 
               instrument_volume) >> 8)) >> 8) * global_volume) >> 8;
    if (temp1 == 0) {
        temp2 *= song_data->volume >> 8;
    }
    temp2 >>= GAX_ram->volboost_level;
    *psVar2 = GAX_ram->dc_correction_val + 2;
    *psVar2 += temp2<<GAX_ram->volboost_level;
    
    local_30 = _call_via_r3(temp, GAX_ram->unk4C[temp1 - 3],
                           unk_uint_1, GAX_umull);

    
    // modulator handler
    use_modulation = FALSE;
    instrument = ch->instrument;
    temp       = ch->waveslot_idx;
    
    if (instrument->wave_params[ch->waveslot_idx].modulation 
       && instrument->wave_params[ch->waveslot_idx].min < instrument->wave_params[ch->waveslot_idx].max) {
        use_modulation = TRUE;
    } 
    uVar1    = wave_addr[1];
    timer_reload = GAX_ram->current_buf->timer_reload;
    
    if (use_modulation) {
        local_38 = (instrument->wave_params[ch->waveslot_idx].max
                   - instrument->wave_params[ch->waveslot_idx].min) * 2048;
    }
    temp = uVar1 << 11;
    enable_modulation = ch->enable_modulation;
    
    if (enable_modulation) {
        local_38   = instrument->wave_params[ch->waveslot_idx].mod_size;
        temp       = (ch->modulate_position + local_38) * 2048;
        local_38 <<= 11;
    }
    local_48          = *wave_addr + (temp >> 11);
    new_wave_position = ch->wave_position - temp;

    if (!use_modulation) {
        if (enable_modulation) {
            goto unset_modulation;
        }
        //local_38 = (local_48 - (*song_data->waveform_data)
    }

    // ping-pong loop handler
    if ((!enable_modulation && use_modulation)
       && instrument->wave_params[ch->waveslot_idx].pingpong) {
        
        local_30 *= ch->wave_direction;

        // call void GAXTracker_asm_end()
        _call_via_r1(&local_48, GAX_ram->gax_tracker_asm_end);

        if (local_30 < 0) {
            // end of forwards loop
            ch->wave_direction = -1;
            ch->wave_position  = (new_wave_position + temp) - local_38;
        } else {
            // end of backwards loop
            ch->wave_direction = 1;
            ch->wave_position  = new_wave_position + temp;
        }
        return TRUE;
        
    }

    unset_modulation:
    
        _call_via_r1(&local_48, GAX_ram->gax_tracker_asm);
        ch->wave_position  = new_wave_position + temp;
        if (new_wave_position + temp < 0) {
            ch->semitone_pitch = -30000;
            ch->wave_porta_val = 0;
            if (ch->perfstep_speed == 0) {
                ch->priority = 1 << 31;
            }
        }
    
        return TRUE;
    
}

// u32 GAXTracker_open
// https://decomp.me/scratch/7tzcx - AArt1256, beanieaxolotl
// accuracy -> 100%

void GAXTracker_open(GAX_channel *ch) {
    
    ch->wave_position  = 0;
    ch->waveslot_idx   = 0;
    ch->instrument     = NULL;
    ch->semitone_pitch = -30000;

    ch->wave_direction    = 1;
    
    ch->instrument_volume = 255;
    ch->mixing_volume     = 255;
    
    ch->ignore            = FALSE;
    ch->enable_modulation = FALSE;
    
    ch->rle_delay         = 0;
    ch->empty_track       = FALSE;
    ch->delay_frames      = 0;
    ch->toneporta_lerp    = 0;
    ch->target_pitch      = 0;
    ch->priority          = 1 << 31; // set MSB of s32
    ch->unk1              = 0;

    if (GAX_ram->music_pitch == 0) {
        GAX_ram->music_pitch = 0x100000000 / GAX_ram->current_buf->mixrate;
    }
}


// u32 GAXTracker_process_envelope
// https://decomp.me/scratch/ZAb83 - beanieaxolotl, christianttt
// accuracy -> 92.16%

u8 GAXTracker_process_envelope(GAX_channel *ch, const GAX_volenv *volenv, u16 *envpos) {
    

    u16 current_time;
    int i;
    const GAX_volenv_point* cur_point;
    const GAX_volenv_point* prev_point;
    u16 prev_time;
    u8  prev_volume;
    s16 cur_lerp;
    s32 val;
    
    current_time = (*envpos)++;
    
    if (volenv->sustain_point != 0xFF && !ch->is_note_off && current_time == (&volenv->env_point)[volenv->sustain_point].x) {
        *envpos = current_time;
    }

    i = volenv->point_count - 1;
    if (current_time >= (&volenv->env_point)[i].x) {
        if ( (*(u8*)&((&volenv->env_point)[i+1])) == 0 ) {
            if (volenv->loop_end == 0xFF || volenv->loop_end < i) {
                ch->semitone_pitch = -30000;
                ch->wave_porta_val = 0;
                ch->priority = 1 << 31;
            }
        }
        *envpos = current_time;
    }

    if (!ch->is_note_off && volenv->loop_start != 0xFF && volenv->loop_end != 0xFF) {
        if (current_time == (&volenv->env_point)[volenv->loop_end].x) {
            *envpos = (&volenv->env_point)[volenv->loop_start].x;
        }
    }
    
    i = 0;
    while ((&volenv->env_point)[i].x < current_time) {
        i++;
    }

    cur_point = (&volenv->env_point) + i;

    if (current_time == cur_point->x) {
        return *(u8*)((u8*)cur_point + 8);
    } else {
        prev_point = (&volenv->env_point) + (i - 1);
        
        prev_time    = *(u16*)((u8*)prev_point + 4);
        prev_volume  = *(u8*)((u8*)prev_point + 8);
        cur_lerp     = *(s16*)((u8*)cur_point + 6);
        
        val = (s32)(current_time - prev_time) * cur_lerp;
        val = (val >> 8) + prev_volume;
        
        return (u8)val;
    }
}

// void GAXTracker_process_frame
// https://decomp.me/scratch/nCWzw - beanieaxolotl
// accuracy -> 83.58%

void GAXTracker_process_frame(GAX_channel *ch) {

    u32 volenv_volume;
    int vibrato_pitch;
    s16 pitch;
    int old_modulate_position;
    int newvol;
    int temp2;
    
    GAX_instrument* instrument;
    
    if (ch->instrument == NULL) {
        // instrument is empty, don't bother!
        goto instrument_tick;
    }
    
    // calculate the current location in the volume envelope
    ch->volenv_volume = GAXTracker_process_envelope(
        ch, ch->instrument->volume_envelope, &ch->volenv_timer);

    // handle vibrato stuff
    if (ch->instrument->vibrato_depth > 0) {
        if (ch->vibrato_wait <= 0) {
            // only apply vibrato if we finish waiting
            ch->vibtable_index = 
                (ch->vibtable_index + ch->instrument->vibrato_speed & (sizeof(GAX_vibtable)-1));
        } else {
            // wait for our vibrato to finish
            ch->vibrato_wait--;
        }
        vibrato_pitch = GAX_vibtable[ch->vibtable_index] * ch->instrument->vibrato_depth >> 8;
    }
    ch->vibrato_pitch = vibrato_pitch;


    // handle modulation stuff

    if (!ch->enable_modulation || (
        temp2 = ch->modulate_timer - 1, ch->modulate_timer = temp2,
        temp2 * 1<<24 > 0)) {
        goto instrument_tick;
    }
    
    old_modulate_position = ch->modulate_position;
    instrument            = ch->instrument;

    ch->modulate_timer = instrument->wave_param[ch->waveslot_idx].mod_speed;

    if (ch->modulate_direction > 0) {
        // forwards modulation
        ch->modulate_position += instrument->wave_param[ch->waveslot_idx].mod_step;
        if (ch->modulate_position > instrument->wave_param[ch->waveslot_idx].mod_size) {
            ch->modulate_position  -= instrument->wave_param[ch->waveslot_idx].mod_step*2;
            ch->modulate_direction = -1;
        }         
    } else {
        // backwards modulation
        ch->modulate_position -= instrument->wave_param[ch->waveslot_idx].mod_step;
        if (ch->modulate_position < instrument->wave_param[ch->waveslot_idx].mod_size) {
            ch->modulate_position  += instrument->wave_param[ch->waveslot_idx].mod_step*2;
            ch->modulate_direction = 1;
        } 
    }
    ch->wave_position += (old_modulate_position * -2048) + (ch->modulate_position * 2048);

    
    instrument_tick:
    
        // song-side volume slide handler
    
        newvol = ch->vol_slide_val + ch->instrument_volume;
        // clamp the volume slide into bounds
        newvol = GAX_CLAMP(newvol, 0, 255);
        ch->instrument_volume = newvol;

        // perf-list/instrument volume slide handler
    
        newvol = ch->wave_vol_slide_val + ch->wave_volume;
        // clamp the waveform volume slide into bounds
        newvol = GAX_CLAMP(newvol, 0, 255);
        ch->wave_volume = newvol;

        pitch               = ch->cur_pitch + ch->porta_val;
        ch->semitone_pitch += ch->wave_porta_val;

        if (ch->toneporta_lerp && (
            vibrato_pitch = pitch, pitch = ch->toneporta_lerp+pitch,
            (ch->target_pitch - vibrato_pitch & 1<<31) != (ch->target_pitch - pitch & 1<<31))) {
            
            ch->toneporta_lerp = 0;
            pitch = ch->target_pitch;
            ch->target_pitch = 0;
            
        }
}

// void GAXTracker_process_perflist
// https://decomp.me/scratch/7QIir - beanieaxolotl
// accuracy -> 65.14%

void GAXTracker_process_perflist(GAX_channel *ch) {
    
    GAX_instrument* instrument;
    GAX_perflist*   perflist;
    GAX_wave_param* wave;

    u32 i;
    int wave_init;
    
    
    if (ch->cur_perfstep >= ch->instrument->perfstep_count) { 
        
        instrument = ch->instrument;
        // stop perf list processing
        ch->perfstep_speed = 0; // override perflist step speed to 0 ticks
        if (ch->semitone_pitch == -30000) {
            ch->priority = 1 << 31; // free channel from memory
        }
        
    } else {

        // get the current perf step list
        perflist = (void*)instrument->perflist + (ch->cur_perfstep * 8); 
        ch->cur_perfstep++; // advance to the next step

        if (perflist->note) { // only when a note is defined
            ch->semitone_pitch = (perflist->note-2) * 32; // > convert note to frequency space
            ch->is_fixed       =  perflist->fixed;        // > is the frequency fixed?
            
            if (perflist->wave_idx) { // only if a wave index is defined
                
                // correct wave slot index to start at 0
                ch->waveslot_idx = perflist->wave_idx-1;
                
                // get the wave data
                wave = (GAX_wave_param*)(ch->waveslot_idx * sizeof(GAX_wave_param));
                wave++;
    
                // initialize waveform playback variables
                ch->wave_position     = 0;     // start of waveform data
                ch->wave_direction    = 1;     // process wave data forwards
                ch->wave_volume       = 255;   // full volume if no set volume is defined
                ch->enable_modulation = FALSE; // disable modulator
    
                if ((wave->modulation) && ((wave->min < wave->max)) 
                    && (wave->mod_size > 0) && (wave->mod_speed > 0) 
                    && (wave->mod_step > 0)) {   
                    ch->enable_modulation = TRUE; // enable modulator
                } 
                
                wave_init              = wave->init;
                ch->modulate_position  = wave_init;
                ch->wave_position      = wave_init << 11;
                ch->modulate_timer     = wave->mod_speed;
                
                if ((wave_init + wave->mod_size) > wave->max) {
                    ch->modulate_direction = -1; // process backwards
                } else {
                    ch->modulate_direction = 1;
                }

            }
        }
        
        // fx command processing

        ch->wave_vol_slide_val = 0; // reset volume slide value
        ch->wave_porta_val     = 0; // reset portamento value
        
        for (i = 0; i < 2; i++) {
            
            u32 fx_command; // effect type
            fx_command = *(u16*)&perflist->commands[i];
            
            if (fx_command >> 8 != 0) {
                
                u32 fx_param;                 // effect param
                fx_param = fx_command & 0xFF; // get the upper byte
                
                switch (fx_command >> 8) { // get the lower byte
                    
                    case PERF_PORTA_UP:
                        // 1xx - slides the note up by X units
                        ch->wave_porta_val = fx_param;
                        break;
                    
                    case PERF_PORTA_DOWN:
                        // 2xx - slides the note down by X units
                        ch->wave_porta_val = -fx_param;
                        break;

                    case PERF_GOTO_STEP:
                        // 5xx - jumps to perf step X after the current step     
                        if (ch->perfstep_loop_counter == 0 || 
                           (ch->perfstep_loop_counter--, 
                            ch->perfstep_loop_counter != 0)) {
                            ch->cur_perfstep = fx_param;
                        }
                        break;

                    case PERF_REPEAT_STEP:
                        // 6xx - repeats the current step X times
                        if (ch->perfstep_loop_counter == 0) {
                            if (fx_param != 0) {
                                ch->perfstep_loop_counter = fx_param+1;
                            } else {
                                ch->perfstep_loop_counter = 0;
                            }
                        }
                        break;

                    case PERF_VOLSLIDE_UP:
                        // Axx - slides the volume up by X units
                        ch->wave_vol_slide_val = fx_param;
                        break;

                    case PERF_VOLSLIDE_DOWN:
                        // Bxx - slides the volume down by X units
                        ch->wave_vol_slide_val = -fx_param;
                        break;

                    case PERF_SET_VOLUME:
                        // Cxx - sets the waveform volume
                        ch->wave_volume = fx_param;
                        break;

                    case PERF_SET_SPEED:
                        // Fxx - sets the speed value (ticks per step)
                        ch->perfstep_speed = fx_param;
                        break;

                    default:
                        break;

                }   
                
            }
        }
    }
}

// void GAXTracker_process_step
// https://decomp.me/scratch/kRxBN - AArt1256, beanieaxolotl, anegoda1995
// accuracy -> 66%

void GAXTracker_process_step(GAX_channel *ch, GAX_player* replayer, b8 flag) {

    u16 note;
    u32 instrument_idx;
    u8  fx_type;
    u8  fx_param;

    GAX_instrument*   instrument;
    GAXChannelInfo*   channel_info;

    u8* seq_data;
    u8* seq_byte;

    // step data processing
    // reset these at the start of each step
    ch->vol_slide_val = 0; // volume slide
    ch->porta_val     = 0; // portamento
    ch->delay_frames  = 0; // note delay

    if (flag == FALSE) {

        if (replayer->pattern_finished) {

            // load the track data
            seq_data = replayer->song->track_data 
                + ch->order[(s16)replayer->pattern].sequence_offset;
            ch->sequence    = seq_data;
            
            ch->rle_delay   = 0;              // reset RLE counter
            ch->empty_track = *(b8*)seq_data; // is the track empty?
            ch->sequence++;                   // if not, align to the first sequence byte
        }

        if (ch->empty_track) {
            // ignore the empty track
            return;
            
        } else if (ch->rle_delay) {
            ch->rle_delay--;
            return;
            
        } else {
            seq_byte = (u8*)ch->sequence;
            
            // read the step data here
            if (seq_byte[0] == RUN_LENGTH) {   // RLE decompression
                ch->rle_delay = seq_byte[1]-1; // number of empty steps
                ch->sequence  = seq_byte+2;
                return;
                
            } else {
                
                if (seq_byte[0] & 0x80) {
                    
                    if ((seq_byte[0] & EMPTY_STEP) == 0) {
                        // empty step
                        ch->sequence = seq_byte+1;
                        return;
                        
                    } else if ((seq_byte[0] & EMPTY_STEP) < INSTR_ONLY) {
                        // note + instrument
                        note           = seq_byte[0] & EMPTY_STEP;
                        instrument_idx = seq_byte[1];
                        fx_param       = 0;
                        fx_type        = 0;
                        ch->sequence = seq_byte+2;
                        
                    } else {
                        // effect only
                        note           = 0;
                        instrument_idx = 0;
                        fx_type        = seq_byte[1];
                        fx_param       = seq_byte[2];
                        ch->sequence = seq_byte+3;
                    }
                    
                } else {
                    // note + instrument + fx
                    note           = seq_byte[0];
                    instrument_idx = seq_byte[1];
                    fx_type        = seq_byte[2];
                    fx_param       = seq_byte[3];
                    ch->sequence = seq_byte+4;
                }
            }

            // special handler for note delay
            if (fx_type == NOTE_DELAY && (fx_param >> 4) == 0xD) {
                // EDx - delays the current note by X ticks
                ch->delay_frames    = fx_param & 0xF;
                ch->next_semitone   = note;
                ch->next_instrument = instrument_idx;
                return;
            }
        }
    } else {
        // get the next note + instrument
        note           = ch->next_semitone;
        instrument_idx = ch->next_instrument;
        fx_param       = 0; // effects are ignored
        fx_type        = 0;
    }

    if (fx_type != TONE_PORTA) {
        
        // note off handler
        if (note == 1) {
            if (ch->instrument && ch->instrument->volume_envelope->sustain_point == 0xFF) {
                // hard cut the note if 
                // the instrument has no sustain point               
                ch->semitone_pitch = -30000;  
                ch->wave_porta_val = 0;       // end portamento
                ch->priority       = 1 << 31; // free channel
                ch->instrument     = NULL;    // free instrument
            }
            ch->is_note_off = TRUE;
            
        } else if (note > 1) {
            // convert note into pitch frequency
            // this subtracts 2 from the note
            // value since 0x0 and 0x1 are reserved
            ch->cur_pitch = (note-2)<<5;
            ch->is_note_off = FALSE;
        }
        
    }

    // instrument data processing
    if (instrument_idx) {

        // get the instrument header
        instrument = (GAX_instrument*)(replayer->song->instrument_data + instrument_idx*4);

        // instrument initialization
        ch->instrument     = instrument;
        ch->volenv_timer   = 0; // reset volume envelope timer

        // vibrato handler
        ch->vibtable_index = 0; // reset vibrato phase
        ch->vibrato_wait   = instrument->unk8;

        // perflist handler
        ch->cur_perfstep   = 0; // always start at perf step #0
        ch->perflist_timer = 0; // reset perflist timer
        ch->perfstep_delay = 0; // reset perflist delay value

        // miscellaneous
        ch->instrument_volume = 255;   // full volume
        ch->is_note_off       = FALSE; // note on
        ch->toneporta_lerp    = 0;     // reset tone porta lerp
        ch->target_pitch      = 0;
        ch->perfstep_speed    = instrument->perfstep_speed;

        if (instrument->blank) {
            ch->instrument = NULL;
        } else {
            channel_info = GAX_ram->params->channel_info;
            if ((channel_info != 0) && (ch->channel_index >= 0)) {
                channel_info[ch->channel_index].instrument = instrument_idx;
            }
        }
    }

    // fx command processing
    switch(fx_type) {

        case PORTA_UP:
            // 1xx - slides the note up by X units
            ch->porta_val = fx_param;
            break;

        case PORTA_DOWN:
            // 2xx - slides the note down by X units
            ch->porta_val = -fx_param;
            break;

        case TONE_PORTA:
            // 3xx - slides the note to the next one by X units
            // (the portamento is constantly applied each tick unlike XM)
            if (fx_param) {
                ch->target_pitch   = (note-2)*32; // convert current note into frequency space
                ch->toneporta_lerp = (ch->target_pitch-ch->cur_pitch)/fx_param;
            }
            break;

        case MODULATE_SPEED:
            // 7xy - sets the speed numerator and denominator values independently
            // this creates a swing effect or emulates a non-integer tempo
            replayer->speed_buf   = fx_param >> 4 | (fx_param << 8 & 0xF00); // swap nybbles
            replayer->speed_timer = (fx_param >> 4) - 1; // start from the lower nybble
            break;

        case VOLSLIDE_UP:
            // Axx - slides the volume up by X units
            ch->vol_slide_val = fx_param;
            break;

        case VOLSLIDE_DOWN:
            // Bxx - slides the volume down by X units
            ch->vol_slide_val = -fx_param;
            break;

        case SET_VOLUME:
            // Cxx - sets the instrument volume
            ch->instrument_volume = fx_param;
            break;

        case BREAK_PATTERN:
            // Dxx - breaks/skips the current pattern.
            replayer->skip_pattern = TRUE;
            replayer->new_step_idx = fx_param; // ignored
            break;

        case SET_SPEED:
            // Fxx - sets the speed value (ticks per step)
            replayer->speed_buf   = fx_param;
            replayer->speed_timer = fx_param-1;
            break;

        default:
            break;
    }
}

// u8 GAXTracker_render
// https://decomp.me/scratch/GoWdE - AArt1256, beanieaxolotl, christianttt
// accuracy -> 100%
// ======================
// edits by beanieaxolotl

u8 GAXTracker_render(GAX_channel* ch, GAX_player* replayer) {
    
    if (replayer->unknown_delay) {
        ch->instrument = NULL;
    }
    
    if (replayer->is_playing) {
        if ((s16)ch->delay_frames != 0) {
            if ((--ch->delay_frames) == 0) {
                GAXTracker_process_step(ch, replayer, 1);
            }
        }
        
        if (replayer->speed_buf != 0 && replayer->step_finished) {
            GAXTracker_process_step(ch, replayer, 0);
        }
    }

    if (ch->instrument != NULL && ch->perflist_timer == 0) {
        if (ch->perfstep_speed != 0) {
            GAXTracker_process_perflist(ch);
            ch->perflist_timer = ch->perfstep_speed - 1;
        }
    } else {
        ch->perflist_timer--;
    }

    GAXTracker_process_frame(ch);
    
    return (ch->ignore != 0)
        ? 0
        : (u8)GAXTracker_generate_audio(ch, replayer, (u32)replayer->song, 0);

}


// void GAXFx_open
// https://decomp.me/scratch/lR5Xx - beanieaxolotl
// accuracy -> 100%

void GAXFx_open(GAX_channel *fxch) {
    
    GAXTracker_open(fxch);
    
    fxch[1].rle_delay    = 0;
    fxch[1].unk1         = 0;
    fxch[1].ignore       = FALSE;
    fxch[1].waveslot_idx = 0;
    fxch[1].empty_track  = FALSE;

    fxch->unk1          = GAX_ram->unk5B;
    fxch->channel_index = -1;

    if (GAX_ram->fx_pitch == 0) {
        GAX_ram->fx_pitch = 0x100000000 / GAX_ram->current_buf->mixrate;
    } 
    
}

// u8 GAXFx_render
// https://decomp.me/scratch/qxLpH - beanieaxolotl
// accuracy -> 92.07%

u8 GAXFx_render(GAX_channel* ch, GAX_player* player) {

    u8 temp;
    GAX_instrument* instrument;
    GAXChannelInfo* ch_info;

    
    if (player->unknown_delay) {
        ch->instrument = NULL;
    }
    
    if (player->is_playing) {
        
        if ((ch[1].rle_delay == 1 || (ch[1].rle_delay 
          && ch[1].waveslot_idx > 0)) && !player->unknown_delay) {
            
            // this checks the FX channel's current semitone
            // which reuses the rle delay variable
            // we only have to init an FX channel if this is exactly 1.
            
            temp = ch[1].rle_delay; 
            if (temp == 1) {
                // turn off the FX channel
                if (ch->instrument && ch->instrument->volume_envelope->sustain_point == GAX_NOTSET) {
                    ch->semitone_pitch = -30000;
                    ch->wave_porta_val = 0;
                    ch->priority       = 1 << 31;
                    ch->instrument     = NULL;
                }
                ch->is_note_off = TRUE;
                
            } else if (ch[1].rle_delay > 1) {
                // init the FX channel
                ch->cur_pitch   = (ch[1].rle_delay-2)*32;
                ch->is_note_off = FALSE;
            }
            
            if (ch[1].waveslot_idx > 0) {
                
                instrument = *(GAX_instrument **)(GAX_ram->fx_data->instrument_data + (ch[1].waveslot_idx));
        
                // instrument initialization
                ch->instrument     = instrument;
                ch->volenv_timer   = 0; // reset volume envelope timer
                
                // vibrato handler
                ch->vibtable_index = 0;                // reset vibrato phase
                ch->vibrato_wait   = instrument->unk8; // get instrument vib. wait
    
                // perflist handler
                ch->cur_perfstep   = 0; // always start at perf step #0
                ch->perflist_timer = 0; // reset perflist timer
                ch->perfstep_delay = 0; // reset perflist delay value
    
                // miscellaneous
                ch->instrument_volume = 255;                        // full volume
                ch->is_note_off       = FALSE;                      // note on
                ch->toneporta_lerp    = 0;                          // reset tone porta lerp
                ch->target_pitch      = 0;                          // no target pitch!
                ch->perfstep_speed    = instrument->perfstep_speed; // get perf step speed
    
                if (instrument->blank) {
                    // instrument is empty
                    ch->instrument = NULL;
                    
                } else {
                    // instrument has data saved
                    ch_info = GAX_ram->params->channel_info;
                    if ((ch_info != NULL) && (ch->channel_index >= 0)) {
                        // updates the GAXChannelInfo struct in the user's GAXParams if possible
                        ch_info[ch->channel_index].instrument = ch[1].waveslot_idx;
                    }
                }
            }
            
            if (ch[1].rle_delay > 1) {
                ch->cur_pitch += ch[1].unk1;
            }
            
            ch->instrument_volume = ch[1].ignore; // reused variable
            ch->vol_slide_val     = 0;            // reset volume slide variable
            ch->porta_val         = 0;            // reset portamento variable
            
            ch[1].waveslot_idx, ch[1].unk1 = 0;   // wave slot index defaults to 0
            ch[1].rle_delay = 0;                  // reused variable
            
        }
    }
    
    temp = ch->perflist_timer;
    if (ch->instrument && ch->perflist_timer == 0) {
        if (ch->perfstep_speed == 0) { // no perflist?
            // ignore perf list with a step speed of 0
            goto process_audio;
        } else {
            GAXTracker_process_perflist(ch);           // setup / process stuff
            ch->perflist_timer = ch->perfstep_speed-1; // initialize perf timer
        }
        
    }

    process_audio:
        GAXTracker_process_frame(ch);
        if (ch[1].empty_track) {
            ch->is_fixed = FALSE;
        }
        return (ch->ignore != 0) // from https://decomp.me/scratch/GoWdE
            ? 0
            : (u8)GAXTracker_generate_audio(ch, player, GAX_ram->fx_data, 1);
    
}