#include "gax.h"

// void GAXSync_open
// https://decomp.me/scratch/YiDDu - beanieaxolotl
// accuracy -> 100%

void GAXSync_open(GAX_player* player) {
    
    player->unk0         = 0;
    player->timer_backup = 0;

    player->is_playing    = FALSE; // not currently playing
    player->unknown_delay = 0;

    player->stop_on_song_end = FALSE; // default: loop playback of music
    player->is_song_end      = FALSE; // song has not ended yet!
    
    player->pattern = -1;    // no pattern loaded
    player->step    = 20000; // no step loaded(?)

    // default speed is 6 ticks per step
    player->speed_timer  = 0; // start as soon as possible
    player->speed_buf    = 6; 
    
    player->global_volume    = 255;   // full volume
    player->step_finished    = FALSE;
    player->pattern_finished = FALSE;
    player->skip_pattern     = FALSE;
    player->new_step_idx     = 0;     // start from step 0
    
}

void GAXSync_render() {

}