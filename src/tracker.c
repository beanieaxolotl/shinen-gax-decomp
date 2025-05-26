#include "gax.h"


void GAXFx_open() {

}

void GAXFx_render() {

}

void GAXTracker_generate_audio() {

}

void GAXTracker_open() {

}

void GAXTracker_process_envelope() {

}

void GAXTracker_process_frame() {

}

void GAXTracker_process_perflist() {

}

// void GAXTracker_process_step
// https://decomp.me/scratch/vwY7I - beanieaxolotl
// accuracy -> 55.48%

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
                
            } else if (seq_byte[0] & FULL_STEP) {
                // note + instrument + fx
                note           = seq_data[0];
                instrument_idx = seq_data[1];
                fx_type        = seq_data[2];
                fx_param       = seq_data[3];
                seq_data += 4;
                
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


void GAXTracker_render() {

}
