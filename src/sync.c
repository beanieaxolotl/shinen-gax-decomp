#include "gax.h"

// void GAXSync_open
// https://decomp.me/scratch/YiDDu - beanieaxolotl
// accuracy -> 100%

void GAXSync_open(GAX_player* player) {
    
    player->unk0         = 0;
    player->timer_backup = 0;

    player->is_playing    = FALSE; // not currently playing
    player->unknown_delay = 0;

    player->stop_on_songend  = FALSE; // default: loop playback of music
    player->songend          = FALSE; // song has not ended yet!
    
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

// void GAXSync_render
// https://decomp.me/scratch/QBX05 - beanieaxolotl
// accuracy -> 94.12%

u32 GAXSync_render(GAX_player* player, int unk) {

    GAXSongData* song;
    
    int new_speed;
    u8  speed_timer;
    u16 speed_buffer;
    
    if (player->timer_backup != unk && player->is_playing) {

        speed_timer  = player->speed_timer;
        speed_buffer = player->speed_buf;

        // todo: if-statement is incorrect
        if ((u8)player->speed_buf == 0 || (!player->songend, player->speed_timer <= 0)) {     
            
            // start of a step
            if (player->skip_pattern) {
                // the song encounters a pattern break effect
                player->skip_pattern = FALSE;
                player->step = player->song->step_count; // load the last step + 1
                song = player->song;                     // reload the song for the step data
            } else {
                player->step++;      // load the next step
                song = player->song; // reload the song for the step data
            }

            if ((player->speed_buf >> 8) != 0) { 
                // if the leftwards speed byte is not 0 (i.e end of a song),
                // swap the speed buffer bytes to allow for speed modulation
                player->speed_buf = speed_buffer | ((speed_buffer & 0xFF) << 8);
            }

            // adjust the speed via GAXParams->speed_adjust
            new_speed = GAX_ram->params->speed_adjust + ((u8)player->speed_buf-1);
            player->speed_timer = GAX_CLAMP(new_speed, 0, new_speed);
            
            if ((s16)player->step >= player->song->step_count) {
                // if there are no more steps to process
                player->step             = 0;    // restart from step #0
                player->new_step_idx     = 0;    // same here
                player->pattern_finished = TRUE; // pattern is now finished
                player->pattern++;
                
                if (song->song_length >= player->pattern) {
                    // if there are no more patterns to process
                    if (player->stop_on_songend) {
                        // handler for the GAX_STOP_ON_SONGEND flag
                        player->is_playing = FALSE;
                        player->speed_buf  = 0;
                    }
                    player->songend = TRUE;
                    player->pattern = song->restart_position;
                    
                } else {
                    // when a pattern simply finishes 
                    // and there are more patterns after it
                    player->pattern_finished = TRUE;
                }
            }
            player->step_finished = TRUE;     
            
        } else {
            // in-between wait between steps
            player->speed_timer--;         // wait one tick
            player->step_finished = FALSE; // step is not finished yet
        }
        
        if ((player->speed_buf & 0xFF) == 0) {
            player->songend = TRUE;
        }
        
        player->timer_backup = unk;
        if (player->unk0 == 0) {
            player->unk0 = unk;
        }
        
        if (player->unknown_delay != 0) {
            player->unknown_delay--;
        }
        
    }
    
    return 0;

}
