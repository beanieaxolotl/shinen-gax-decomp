#include "gax.h"


void GAXFx_open() {

}

void GAXFx_render() {

}

void GAXTracker_generate_audio() {

}

// u32 GAXTracker_open
// https://decomp.me/scratch/BHMdc - beanieaxolotl
// accuracy -> 84%

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
    ch->priority          = 2<<30;
    ch->unk1              = 0;

    if (GAX_ram->music_pitch == 0) {
        //__udivdi3(0,1,*(undefined2 *)((int)GAX_ram->memory_size + 2),0);
        GAX_ram->music_pitch = __udivdi3(GAX_ram->scratch_mem_size,0);
    }
}

// u32 GAXTracker_process_envelope
// https://decomp.me/scratch/5lCKZ - beanieaxolotl
// accuracy -> 68.92%

u32 GAXTracker_process_envelope(GAX_channel *ch, GAX_volenv *volenv, u16 *envpos) {
    
    u32         current_point;
    s32         time = 0;
    u16         x1;
    GAX_volenv *y1;

    x1 = *envpos;
    *envpos = x1+1;
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
            ch->semitone_pitch = 35536;
            ch->wave_porta_val = 0;
            ch->priority       = 2<<30;
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
    return;
    
}

void GAXTracker_process_frame() {

}

void GAXTracker_process_perflist() {

}

// void GAXTracker_process_step
// https://decomp.me/scratch/vwY7I - beanieaxolotl
// accuracy -> 55.58%

void GAXTracker_process_step(GAX_channel *ch) {

    u16 note;
    u32 instrument_idx;
    u8  fx_type;
    u8  fx_param;
    
    GAX_instrument*   instrument;
    GAX_playbackvars* playback_vars;
    GAXChannelInfo*   channel_info;
    
    b8  get_next_note; // what
    u8* seq_data;
    u8* seq_byte;
    
    ch->vol_slide_val = 0;
    ch->porta_val     = 0;
    ch->delay_frames  = 0;

    // step data processing
    
    if (!get_next_note) {
        
        if (playback_vars->unk16) {
            seq_data = (u8*)(*(s32*)(playback_vars->unk0+12) 
                + ch->order[playback_vars->unkC].sequence_offset);
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
            
        } else {
            
            seq_data = ch->sequence;
            seq_byte = (u8*)seq_data;
    
            // read the step data here
            if (seq_byte[0] == RUN_LENGTH) {
                // multiple empty steps (RLE compression)
                ch->rle_delay = seq_data[1]-1;
                ch->sequence  = seq_data+2;
                
            } else {
                
                note = seq_byte[0] & EMPTY_STEP;
                
                if (seq_byte[0] & EMPTY_STEP) {
                    // empty step
                    ch->sequence++;
                } 
                if (note < INSTR_ONLY) {
                    // instrument only
                    instrument_idx = seq_data[1];
                    fx_type, fx_param = 0;
                    seq_data += 2;
                    
                } else {
                    // effect only
                    note, instrument_idx = 0;
                    fx_type  = seq_data[1];
                    fx_param = seq_data[2];
                    seq_data += 3;
                }
            }

            if (seq_byte[0] & FULL_STEP) {
                // note + instrument + fx
                note           = seq_data[0];
                instrument_idx = seq_data[1];
                fx_type        = seq_data[2];
                fx_param       = seq_data[3];
                seq_data += 4;
                
            }
            ch->sequence = seq_data;
            
            // special handler for note delay
            if (fx_type == NOTE_DELAY && fx_param >> 4 == 0xD) {
                ch->delay_frames    = fx_param & 0xF;
                ch->next_semitone   = note;
                ch->next_instrument = instrument_idx;
            }
        }
    } else {
        note           = ch->next_semitone;
        instrument_idx = ch->next_instrument;
        fx_type, fx_param = 0;
    }
    
    if (fx_type != TONE_PORTA) {
        // note off handler
        if (note == 1) {
            if (ch->instrument > 0 && ch->instrument->volume_envelope->sustain_point == 0xFF) {
                // hard cut the note if 
                // the instrument has no sustain point               
                ch->semitone_pitch = 35536;
                ch->wave_porta_val = 0;
                ch->priority       = 2<<30; // channel is now freed
            }
            ch->is_note_off = TRUE;
            
        } else if (note > 1) {
            // convert note into pitch
            // this subtracts 2 from the note
            // value since 0x0 and 0x1 are reserved
            ch->cur_pitch   = (note-2)*32;
            ch->is_note_off = FALSE;
        }
    }

    // instrument data processing
    if (instrument_idx) {
        
        instrument = (GAX_instrument*)((instrument_idx*4) + (playback_vars->unk0+16));
        
        ch->instrument     = instrument;
        ch->volenv_timer   = 0;
        ch->vibtable_index = 0;
        ch->vibrato_wait   = instrument->vibrato_wait;
        
        ch->cur_perfstep, ch->perflist_timer = 0;
        
        ch->instrument_volume = 255;
        ch->is_note_off       = FALSE;
        ch->toneporta_lerp    = 0;
        ch->target_pitch      = 0;
        ch->perfstep_speed    = instrument->perfstep_speed;

        if (instrument->blank) {
            channel_info = GAX_ram->params->channel_info;
            if ((channel_info != 0) && (ch->channel_index > 0)) {
                channel_info[ch->channel_index].instrument = instrument_idx;
            }
        }
        
    }

    // fx command processing
    switch(fx_type) {
        
        case PORTA_UP:
            ch->porta_val = fx_param;
            break;
        
        case PORTA_DOWN:
            ch->porta_val = -fx_param;
            break;
        
        case TONE_PORTA:
            if (fx_param) {
                ch->target_pitch = (note-2)*32;
                ch->toneporta_lerp = (ch->target_pitch-ch->cur_pitch)/fx_param;
            }
            break;
        
        case MODULATE_SPEED:
            playback_vars->speed_hi = (fx_param >> 4) | (fx_param << 8);
            playback_vars->speed_lo = (fx_param >> 4) + 0xff;
            break;
                
        case VOLSLIDE_UP:
            ch->vol_slide_val = fx_param;
            break;
        
        case VOLSLIDE_DOWN:
            ch->vol_slide_val = -fx_param;
            break;      
        
        case SET_VOLUME:
            ch->instrument_volume = fx_param;
            break;     
        
        case BREAK_PATTERN:
            // bug: the current step is not set up correctly
            // resulting in the dest step being 0
            playback_vars->break_pattern = TRUE;
            playback_vars->current_step  = fx_param;
            break;
        
        case SET_SPEED:
            playback_vars->speed_hi, 
            playback_vars->speed_lo = fx_param;
            break;

        default:
            break;
    }
}

// u8 GAXTracker_render
// https://decomp.me/scratch/oJTi6 - beanieaxolotl
// accuracy -> 72.50%

u8 GAXTracker_render(GAX_channel* ch, GAX_player* replayer) {

    u16 delay_frames;
    u8  perf_var;
    s16 song_flags;
    
    if (replayer->no_instruments) {
        ch->instrument = NULL;
    }
    
    if (replayer->skip_pattern) {
        if (ch->delay_frames &&
            (delay_frames = ch->delay_frames-1,
             ch->delay_frames = delay_frames,
             delay_frames = 0)) {
            GAXTracker_process_step(ch);
        }
        // TO DO
        if (song_flags || (&replayer->unk14 > 0)) {
            GAXTracker_process_step(ch);
        }
    }
    perf_var = ch->perflist_timer;
    if (ch->instrument && ch->perflist_timer == 0) {
        if (ch->perfstep_speed > 0) {
            goto process_audio;
        }
        GAXTracker_process_perflist(ch);
        perf_var = ch->perfstep_speed;
    }
    ch->perflist_timer = perf_var-1;

    process_audio:
        GAXTracker_process_frame(ch);
        if (ch->ignore) {
            return GAXTracker_generate_audio(ch, replayer, replayer->unk0, 0);
        } else {
            return 0;
        }
}
