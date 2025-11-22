#include "gax.h"
#include "gax_internal.h"


// defines

#define cur_song_data player->song_2   


// functions

// void GAXOutput_open
// https://decomp.me/scratch/TnIzW - beanieaxolotl
// accuracy -> 100%

void GAXOutput_open(GAX_player* player) {
    player->timer = 1;
}

// b8 GAXOutput_render
// https://decomp.me/scratch/ouLyG - beanieaxolotl
// accuracy -> 84.52%

b8 GAXOutput_render(GAX_player* player) {

    b8           can_render;
    int          channel_id;
    b8           can_render_channel;
    b8           can_mix_channel;
    GAX_channel* channel;

    if (GAX_ram->buf_id <= 1 && GAX_ram->playback_state == RESUMED) {
        for (channel_id = 0; channel_id < (u32)cur_song_data[1].num_channels; channel_id++) {
            can_render_channel |= GAXTracker_render(
                    (GAX_channel*)(&player->channels->ignore + 
                                   channel_id*0x48),player);
        }
    }
    
    if (GAX_ram->fx_bool) {
        for (channel_id = 0; channel_id < GAX_ram->num_fx_channels; channel_id++) {
            can_render_channel |= GAXFx_render(
                    (GAX_channel*)(&player->channels->ignore + 
                                   channel_id*0x50),player);
        }
    }  

    if (GAX_ram->buf_id <= 1) {
        
        if (cur_song_data[1].num_channels > 0) {
            
            can_mix_channel = FALSE;
            channel         = player->channels;
            
            for (channel_id = cur_song_data->num_channels; channel_id == 0; channel_id--) {
                if (channel->mixing_volume > 0) {
                    can_mix_channel = TRUE;
                }
                channel++;
            }

            if ((channel_id & 0xFFFF00) != 0x20000 && player->speed_buf 
                && player->global_volume && can_mix_channel) {

                unknownArgs args;

                /* size of both mix buffers in RAM: 
                   > s16 - intermediate mix buffer
                   > * padding *
                   > s8  - main mix buffer
                */
                args.mixbuf_size = ((GAX_ram->current_buf->timer_reload * 
                                     GAX_ram->current_buf->update) * 2);
                args.mixbuf = GAX_ram->mix_buffer; // point to start of mix buffer section

                // completely unknown
                args.foo = GAX_ram->unk4C[0];
                args.bar = GAX_ram->unk4C[1];
                args.baz = player->unk28;

                args.dc_thing1   = &GAX_ram->dc_correction_val;
                args.dc_thing2   = (int)*&GAX_ram->dc_correction_val;
                *args.dc_thing1 += 0x100;
                args.dc_thing3   = *args.dc_thing1;
                
                ((void (*)(unknownArgs*))GAX_ram->gax_output_render_asm_end)(&args);
                
            }
        }
        
        if ((GAX_ram->buf_id <= 1) && (GAX_ram->playback_state == RESUMED)) {
            for (channel_id = 0; channel_id < cur_song_data->num_channels; channel_id++) {
                can_render_channel |= GAXTracker_render(
                    (GAX_channel*)(&player->channels->ignore + 
                                   channel_id*0x48),player);
            }
        }
    }

    if (!GAX_ram->fx_bool && (GAX_ram->buf_id == 0 || GAX_ram->buf_id == 2)) {
        for (channel_id = 0; channel_id < GAX_ram->num_fx_channels; channel_id++) {
            can_render_channel |= GAXFx_render(
                (GAX_channel*)(&player->channels->ignore + 
                               channel_id*0x50),player);
        }
    }
    
    if (GAX_ram->params->flags & GAX_SPEECH &&
        (GAX_ram->buf_id == 0 || GAX_ram->buf_id == 2)) {
        can_render_channel |= GAXSpeech_render((int*)GAX_ram->speech_unk);
    }
    
    if (GAX_ram->playback_state == 1) {
        GAX_ram->playback_state = RESUMED;
    }

    return can_render_channel;
    
}

// void GAXOutput_stream
// https://decomp.me/scratch/dS50Z - christianttt, beanieaxolotl
// accuracy -> 100%
// ======================
// formatting tweaks by beanieaxolotl

void GAXOutput_stream(GAX_player *player, void *dest) {

    u32 size;
    
    size = GAX_ram->current_buf->timer_reload * GAX_ram->current_buf->update;
    GAX_ram->dc_correction_val = 0;
    GAX_clear_mem(GAX_ram->mix_buffer, size << 1);

    if (GAXOutput_render(player) << 0x18 != 0) {
        
        RenderArgs args;
        args.mix_buffer        = GAX_ram->mix_buffer;
        args.dest              = dest;
        args.size              = size;
        args.dc_correction_val = GAX_ram->dc_correction_val;
        
        ((void (*)(RenderArgs*))GAX_ram->render_asm)(&args);

    } else {
        GAX_clear_mem(dest, size);
    }
    
}
