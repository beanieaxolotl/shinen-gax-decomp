#include "gax.h"
#include "gax_internal.h"

// to do
u8 GAXTracker_generate_audio(GAX_channel *ch, GAX_player* replayer, u32 unk, u32 flag) {

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
// https://decomp.me/scratch/5lCKZ - beanieaxolotl
// accuracy -> 70.98%

u8 GAXTracker_process_envelope(GAX_channel *ch, GAX_volenv *volenv, u16 *envpos) {
    
    u32 current_point;
    u16 x1   = *envpos;
    s32 time = 0;
    
    GAX_volenv *y1;
    *envpos       = x1+1;
    current_point = x1;


    if ((volenv->sustain_point != GAX_NOTSET) && !ch->is_note_off 
        && (current_point == (&volenv->env_point)[volenv->sustain_point].x)) {
        // pause the envelope until a note off is commenced
        *envpos = x1;  
    } 
    if (*(u16*)(volenv + time * sizeof(GAX_volenv_point)) == current_point) {
        
        if ((&volenv->point_count)[volenv->point_count * sizeof(GAX_volenv_point)]
            && (volenv->loop_end == GAX_NOTSET || (volenv->loop_end < time))) {
            // free up this channel when the envelope stops
            ch->semitone_pitch = -30000;
            ch->wave_porta_val = 0;
            ch->priority       = 1 << 31;
        }
        *envpos = x1;
    }

    time = volenv->point_count-1; // gets length of the envelope in frames

    if (!ch->is_note_off && volenv->loop_start != GAX_NOTSET
        && volenv->loop_end != GAX_NOTSET 
        && current_point == (&volenv->env_point)[volenv->loop_end].x) {
        // loop back to the previously defined loop start point
        *envpos = (&volenv->env_point)[volenv->loop_start].x;
    }

    for (x1 = volenv->env_point.x; x1 < current_point; time++) {
        //x1._0_1_ = y1[1].point_count;
        //x1._1_1_ = y1[1].sustain_point;
        y1 = (GAX_volenv*)&y1->env_point.volume;
    }

    if (current_point == (&volenv->env_point)[time].x) {
        // if we're directly on the point, apply the volume we see
        return (&volenv->env_point)[time].volume;
        
    } else {
        // if not, apply the precalculated lerp between the two points
        return ((&volenv->env_point)[time].lerp * 
                (current_point - (u16)((s32)volenv + time-1))) 
              + (&volenv->point_count)[time];
    }   
    
}

// void GAXTracker_process_frame
// https://decomp.me/scratch/nCWzw - beanieaxolotl
// accuracy -> 76.95%

void GAXTracker_process_frame(GAX_channel *ch) {

    u32 volenv_volume;
    int vibrato_pitch;
    u16 pitch;
    int old_modulate_position;
    int newvol;
    int temp2;
    int wave_param_ptr;
    
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
        // do we have vibrato at all?
        if (ch->vibrato_wait > 0) {
            // only apply vibrato if we finish waiting
            ch->vibtable_index += ch->instrument->vibrato_speed & sizeof(GAX_vibtable)-1;
        } else {
            // wait for our vibrato to finish
            ch->vibrato_wait--;
        }
        vibrato_pitch = GAX_vibtable[ch->vibtable_index] * ch->instrument->vibrato_depth;
    }
    ch->vibrato_pitch = vibrato_pitch;


    // handle modulation stuff

    if (!ch->enable_modulation || (
        temp2 = ch->modulate_timer - 1, ch->modulate_timer = temp2,
        temp2 * 1<<24 > 0)) {
        goto instrument_tick;
    }
    
    old_modulate_position = ch->modulate_position;
    instrument = ch->instrument;

    ch->modulate_timer = instrument->wave_param[ch->waveslot_idx].mod_speed;

    if (ch->modulate_direction < 1) {
        // forwards modulation
        ch->modulate_position += instrument->wave_param[ch->waveslot_idx].mod_step;
        if (ch->modulate_position < instrument->wave_param[ch->waveslot_idx].min) {
            ch->modulate_position  += instrument->wave_param[ch->waveslot_idx].mod_step*2;
            ch->modulate_direction = 1;
        }         
    } else {
        // backwards modulation
        ch->modulate_position -= instrument->wave_param[ch->waveslot_idx].mod_step;
        if (instrument->wave_param[ch->waveslot_idx].max < instrument->wave_param[ch->waveslot_idx].mod_size) {
            ch->modulate_position  += instrument->wave_param[ch->waveslot_idx].mod_step*-2;
            ch->modulate_direction = -1;
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

        pitch  = *&ch->cur_pitch + ch->porta_val;
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
// accuracy -> 55%

void GAXTracker_process_perflist(GAX_channel *ch) {
    
    GAX_instrument* instrument;
    GAX_perflist*   perflist;
    GAX_wave_param* wave;

    u32 i;
    
    // use shorthand for ch->instrument
    instrument = ch->instrument;
    
    if (ch->cur_perfstep >= instrument->perfstep_count) { 
        // stop perf list processing
        ch->perfstep_speed = 0; // override perflist step speed to 0 ticks
        if (ch->semitone_pitch == -30000) {
            ch->priority = 1 << 31; // free channel from memory
        }
        
    } else {
        
        perflist = (void *)instrument->perflist + // get the current perf step list
                    (ch->cur_perfstep * 8); 
        
        ch->cur_perfstep++; // advance to the next step

        if (perflist->note) { // only when a note is defined
            ch->semitone_pitch = (perflist->note-2) * 32;
            ch->is_fixed       = perflist->fixed;
            
            if (perflist->wave_idx) { // only if a wave index is defined
                
                // correct wave slot index to start at 0
                ch->waveslot_idx = perflist->wave_idx-1;
                // get the wave data
                wave = (GAX_wave_param*)(ch->waveslot_idx * sizeof(GAX_wave_param));
    
                // initialize waveform playback variables
                ch->wave_position     = 0;     // start of waveform data
                ch->wave_direction    = 1;     // process wave data forwards
                ch->wave_volume       = 255;   // full volume if no set volume is defined
                ch->enable_modulation = FALSE; // disable modulator
    
                if ((wave->tune != 0) && (wave->min < wave->max) && (wave->mod_step >= 0) && (wave->mod_size > 0)) {                  
                    ch->wave_position = *(int*)(&wave->pingpong) << 11;
                } else {
                    
                    int j;
                    ch->enable_modulation = TRUE; // enable modulator
                    
                    // calculate modulator position(?)
                    j = *(int*)(&wave->pingpong + (int)instrument->waves);
                    
                    ch->modulate_position  = j;
                    ch->wave_position      = j << 11;
                    ch->modulate_timer     = wave->mod_speed;
    
                    if ((j + wave->max) > wave->min) {
                        ch->modulate_direction = -1; // process backwards 
                    } else {
                        ch->modulate_direction = 1; // process forwards
                    }
                    
                }
            }
        }
        
        // fx command processing

        ch->wave_vol_slide_val = 0; // reset volume slide value
        ch->wave_porta_val = 0;     // reset portamento value
        
        for (i = 0; i < 2; i++) {
            
            int fx_command; // effect type
            int fx_param;   // effect param
            
            fx_command = *(u16*)&perflist->commands[i];
            
            if (fx_command >> 8 != 0) {
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

                    case PERF_SET_STEP:
                        // 5xx - jumps to perf step X after the current step 
                        if (ch->perfstep_delay == 0 || 
                           (fx_param = ch->perfstep_delay, 
                            ch->perfstep_delay = fx_param, 
                            fx_param != 0)) {
                            ch->cur_perfstep = fx_param;
                        }
                        break;

                    case PERF_DELAY_STEP:
                        // 6xx - delay the perf step by X frames
                        if (ch->perfstep_delay == 0) {
                            if (fx_param >= 1) {
                                fx_param++;
                            } else {
                                fx_param = 0;
                            }
                            ch->perfstep_delay = fx_param;
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
// https://decomp.me/scratch/pnSyh - AArt1256, beanieaxolotl, anegoda1995
// accuracy -> 65.15%

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

    ch->vol_slide_val = 0;
    ch->porta_val     = 0;
    ch->delay_frames  = 0;

    if (flag == 0) {

        if (replayer->pattern_finished) {
            seq_data = replayer->song->track_data + ch->order[(s16)replayer->pattern].sequence_offset;
            ch->sequence    = seq_data;
            ch->rle_delay   = 0;
            ch->empty_track = *(b8*)seq_data; // read the first presence byte
            ch->sequence++;                   // move into actual pattern data
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
            if (seq_byte[0] == RUN_LENGTH) {
                // multiple empty steps (RLE compression)
                ch->rle_delay = seq_byte[1]-1;
                ch->sequence  = seq_byte+2;
                return;
            } else {
                if (seq_byte[0] & 0x80) {
                    if ((seq_byte[0] & EMPTY_STEP) == 0) {
                        ch->sequence = seq_byte+1;
                        return;
                    } else if ((seq_byte[0] & EMPTY_STEP) < INSTR_ONLY) {
                        note = seq_byte[0] & EMPTY_STEP;
                        instrument_idx = seq_byte[1];
                        fx_type = 0;
                        fx_param = 0;
                        ch->sequence = seq_byte+2;
                    } else {
                        // effect only
                        note = 0;
                        instrument_idx = 0;
                        fx_type  = seq_byte[1];
                        fx_param = seq_byte[2];
                        ch->sequence = seq_byte+3;
                    }
                }  else {
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
        note           = ch->next_semitone;
        instrument_idx = ch->next_instrument;
        fx_type = 0;
        fx_param = 0;
    }

    if (fx_type != TONE_PORTA) {
        // note off handler
        if (note == 1) {
            if (ch->instrument && ch->instrument->volume_envelope->sustain_point == 0xFF) {
                // hard cut the note if 
                // the instrument has no sustain point               
                ch->semitone_pitch = -30000;
                ch->wave_porta_val = 0;
                ch->priority       = 1 << 31; // channel is now freed
                ch->instrument     = NULL;
            }
            ch->is_note_off = TRUE;
        } else if (note > 1) {
            // convert note into pitch
            // this subtracts 2 from the note
            // value since 0x0 and 0x1 are reserved
            ch->cur_pitch = (note-2)<<5;
            ch->is_note_off = FALSE;
        }
    }

    // instrument data processing
    if (instrument_idx) {

        instrument = (GAX_instrument*)(replayer->song->instrument_data + instrument_idx*4);

        // instrument initialization
        ch->instrument     = instrument;
        ch->volenv_timer   = 0; // reset volume envelope timer

        // vibrato handler
        ch->vibtable_index = 0; // reset vibrato phase
        ch->vibrato_wait   = instrument->vibrato_wait;

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
                ch->target_pitch = (note-2)*32;
                ch->toneporta_lerp = (ch->target_pitch-ch->cur_pitch)/fx_param;
            }
            break;

        case MODULATE_SPEED:
            // 7xy - sets the speed numerator and denominator values independently
            // this creates a swing effect or emulates a non-integer tempo
            *(u16*)replayer->speed_buf = (fx_param >> 4) | (fx_param << 8) & 0xF00;
            replayer->speed_timer      = (fx_param >> 4) + 0xFF;
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

            // Dxx - breaks/skips the current pattern

            // bug: despite it being set,
            // the step index does not change

            replayer->skip_pattern = TRUE;
            replayer->new_step_idx = fx_param;
            break;

        case SET_SPEED:
            // Fxx - sets the speed value (ticks per step)
            *replayer->speed_buf = fx_param;
            replayer->speed_timer = fx_param;
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
        
        if (*(u16*)replayer->speed_buf != 0 && replayer->step_finished) {
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