#include "gax.h"


// void GAXOutput_open
// https://decomp.me/scratch/TnIzW - beanieaxolotl
// accuracy -> 100%

void GAXOutput_open(GAX_player* player) {
    player->timer = 1;
}

void GAXOutput_render() {

}

// void GAXOutput_stream
// https://decomp.me/scratch/3NNLq - beanieaxolotl
// accuracy -> 85.47%

void GAXOutput_stream(GAX_channel* ch, u32 dest) {
    
    u32   dc_val;
    void* buf;
    u32   size;
    
    //u32   temp;
    //u32   mem_size;

    size = (GAX_ram->current_buf->timer_reload) * (GAX_ram->current_buf->update);
    GAX_ram->dc_correction_val = 0;
    
    GAX_clear_mem((u32)GAX_ram->mix_buffer, size*2);
    
    if (GAXOutput_render(ch)) {
        buf    = GAX_ram->mix_buffer;
        dc_val = GAX_ram->dc_correction_val;
        _call_via_r1(&buf, GAX_ram->render_asm);   
    } else {
        GAX_clear_mem(dest, size);
    }
    
}