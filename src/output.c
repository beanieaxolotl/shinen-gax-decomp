#include "gax.h"
#include "gax_internal.h"


// void GAXOutput_open
// https://decomp.me/scratch/TnIzW - beanieaxolotl
// accuracy -> 100%

void GAXOutput_open(GAX_player* player) {
    player->timer = 1;
}

u8 GAXOutput_render(GAX_player* player) {
    
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
