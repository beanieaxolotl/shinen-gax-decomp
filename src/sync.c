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

// void GAXSync_render
// https://decomp.me/scratch/QBX05 - beanieaxolotl
// accuracy -> 90.66%

void GAXSync_render(GAX_player* player, int unk) {

    GAXSongData* song;
    
    u16 speed;
    int new_speed;
    u8  speed_timer;
    u16 speed_buffer;
    
    if (player->timer_backup != unk && player->is_playing) {

        speed_buffer = player->speed_buf;
        speed_timer  = player->speed_timer;
        
        if (speed_buffer == 0 || (!player->is_song_end, 
            player->speed_timer != 0, player->speed_timer <= 0)) {     
            
            // start of a step
            if (player->skip_pattern) {
                // the song encounters a pattern break effect
                player->skip_pattern = FALSE;
                player->step = player->song->step_count; // load the last step + 1
            } else {
                player->step++;      // load the next step
                song = player->song; // reload the song for the step data
            }

            speed = player->speed_buf >> 8;
            if (speed != 0) {
                player->speed_buf = speed | ((player->speed_buf & 0xFF) << 8);
            }
            
            new_speed = GAX_ram->params->speed_adjust + ((u8)player->speed_buf-1);
            if (new_speed < 0) {
                new_speed = 0;
            }
            player->speed_timer = new_speed;

            if ((s16)player->step >= song->step_count) {
                
                player->step             = 0;
                player->new_step_idx     = 0;
                player->pattern_finished = TRUE;
                player->pattern++;
                
                if (song->song_length >= player->pattern) {
                    if (player->stop_on_song_end) {
                        player->is_playing = FALSE;
                        player->speed_buf  = 0;
                    }
                    player->is_song_end = TRUE;
                    player->pattern     = song->restart_position;
                } else {
                    player->pattern_finished = TRUE;
                }
            }
            player->step_finished = TRUE;            
            
        } else {
            // in-between wait between steps
            player->speed_timer--;         // wait one tick
            player->step_finished = FALSE; // step is not finished yet
        }
        
        if (player->speed_buf == 0) {
            player->is_song_end = TRUE;
        }
        
        player->timer_backup = unk;
        if (player->unk0 == 0) {
            player->unk0 = unk;
        }
        if (player->unknown_delay != 0) {
            player->unknown_delay--;
        }
        
    }
}
