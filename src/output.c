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
// accuracy -> 91.11%
// ======================
// formatting tweaks by beanieaxolotl

void GAXOutput_stream(GAX_player *player, void *dest) {

    u32 size;
    
    size = GAX_ram->current_buf->timer_reload * GAX_ram->current_buf->update;
    GAX_ram->dc_correction_val = 0;
    GAX_clear_mem(GAX_ram->mix_buffer, size * 2);

    if (GAXOutput_render(player)) {
        
        // This is the C code that generates the target assembly.
        RenderArgs args; // <-- This will generate `sub sp, #0x10`
        
        args.mix_buffer        = GAX_ram->mix_buffer;
        args.dest              = dest;
        args.size              = size;
        args.dc_correction_val = GAX_ram->dc_correction_val;
        
        ((GAX_RenderFunc)GAX_ram->render_asm)(&args); // <-- This will generate the call

    } else {
        GAX_clear_mem(dest, size);
    }

}