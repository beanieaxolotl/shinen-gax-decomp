#include "gax.h"
#include "gax_internal.h"


// void GAXOutput_open
// https://decomp.me/scratch/TnIzW - beanieaxolotl
// accuracy -> 100%

void GAXOutput_open(GAX_player* player) {
    player->timer = 1;
}

// u8 GAXOutput_render
// https://decomp.me/scratch/Z5T0a - beanieaxolotl
// accuracy -> 23.07%
// ==========================
// extremely WIP / unfinished

u8 GAXOutput_render(GAX_player* player) {

    // identified vars
    u32 channel_id;
    int offset;

    // unknown vars
    int iVar5;
    u32 uVar9;
    u8  bVar2;
    b8  bVar1;
    GAX_channel* iVar6;

    uVar9 = 0;
    
    if (GAX_ram->buf_id < 2 && GAX_ram->playback_state == 0 &&
       (channel_id = 0, &player->song_2[0].step_count + 1) != 0) {
        
        offset = 0;
        for (channel_id = 0; channel_id < player->song_2[1].step_count + 1; channel_id++) {
            uVar9 |= GAXTracker_render(
                    (GAX_channel*)(&player->channels->ignore + offset),
                     player);
            iVar6 = (GAX_channel*)&iVar6[1].modulate_timer;
            offset += 0x48;
        }
    }

    if (GAX_ram->unk59 && (channel_id = 0, GAX_ram->num_fx_channels != 0)) {
        
        iVar6 = 0;
        for (; channel_id < GAX_ram->num_fx_channels; channel_id++) {
            uVar9 |= GAXFX_render(
                    (GAX_channel*)(&player->channels->ignore + (int)GAX_ram->fx_channels),
                     player);
            iVar6 = (GAX_channel*)&iVar6->modulate_timer;
        }
        
    }  

    if (GAX_ram->buf_id < 2) {
        if (&player->song_2[1].step_count + 1 != 0) {
            bVar1 = FALSE;
            for (channel_id = player->song_2->num_channels; channel_id != 0; channel_id--) {
                if (player->channels->mixing_volume != 0) {
                    bVar1 = TRUE;
                }
            }
        }
    }

    // incomplete!
    
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
        
        ((GAX_RenderFunc)GAX_ram->render_asm)(&args);

    } else {
        GAX_clear_mem(dest, size);
    }
}
