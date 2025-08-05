// defines

#define MATCHING   TRUE

#define GAX_NOTSET 0xFF
#define RESUMED    0
#define PAUSED     2

#define MAX_CHANNELS    32
#define MAX_FX_CHANNELS 8
#define MAX_WAVES       4


// constants

static const char* GAX_copyright;
static const u16 GAX_table[];
static const u8 gax_default_package[];
static const char GAX_font[281];       // to do: data needs to be documented!
static const s8 GAX_vibtable[64];
static const u32 GAX_periodtable[384];


// enums //

enum perfstep_command {
    PERF_PORTA_UP       = 0x1,
    PERF_PORTA_DOWN     = 0x2,
    PERF_SET_STEP       = 0x5,
    PERF_DELAY_STEP     = 0x6,
    PERF_VOLSLIDE_UP    = 0xA,
    PERF_VOLSLIDE_DOWN  = 0xB,
    PERF_SET_VOLUME     = 0xC,
    PERF_SET_SPEED      = 0xF
};

enum step_command {
    PORTA_UP       = 0x1,
    PORTA_DOWN     = 0x2,
    TONE_PORTA     = 0x3,
    MODULATE_SPEED = 0x7,
    VOLSLIDE_UP    = 0xA,
    VOLSLIDE_DOWN  = 0xB,
    SET_VOLUME     = 0xC,
    BREAK_PATTERN  = 0xD,
    NOTE_DELAY     = 0xE,
    SET_SPEED      = 0xF
};

enum step_mask {
    NOTE_OFF   = 0x1,
    INSTR_ONLY = 0x7A,
    EMPTY_STEP = 0x7F,
    FULL_STEP  = 0x80,
    RUN_LENGTH = 0xFF
};


// macros //

#define GAX_ALIGN(a, b) (u32)(((u32)(a) + (b)) & -(b))

#define GAX_ALLOC(sz) ({\
    u8 *out; \
    u8 *scratchptr = GAX_ram->scratch_mem; \
    if ((u32)scratchptr & 3) { \
        u32 newsize = GAX_ALIGN(scratchptr, 4) - (u32)scratchptr; \
        GAX_ram->scratch_mem = (u8 *)GAX_ALIGN(scratchptr, 4); \
        GAX_ram->scratch_mem_size -= newsize; \
    } \
    if ((GAX_ram->params->debug) && ((sz) > GAX_ram->scratch_mem_size)) { \
        GAX_ASSERT("GAX2_INIT", "OUT OF MEMORY"); \
    } \
    GAX_ram->scratch_mem_size -= (sz); \
    out = GAX_ram->scratch_mem; \
    GAX_ram->scratch_mem += (sz); \
    out; \
    })

#define GAX_CLAMP(a, min, max) ({\
    a = a > max ? max : a; \
    a = a < min ? min : a; \
    })


// structs //

typedef struct GAXSongData {
    u8    num_channels;
    u16   num_steps_per_pattern;
    u16   num_patterns_per_channel;
    u16   loop_point;
    u16   volume;
    void* track_data;
    void* instrument_data;
    void* waveform_data;
    u16   mixing_rate;
    u16   fx_mixing_rate;
    u8    fx_channels;
    u8    pad1D[0xbc - 0x1d];
    u32   unkBC;
    u32   unkC0;
    u32   unkC4;
    // ...
} GAXSongData;


typedef struct GAXSong {
    u32 unk0, unk4, unk8;
    s16 unkC, unkE;
    u16 unk10;
    u8  unk12, unk13, unk14, unk15, unk16, unk17, unk18, unk19, unk1A, unk1B;
    u16 unk1C;
} GAXSong;


typedef struct GAX_order {
    u16 sequence_offset;
    s8  transpose;
    u8  pad3;
} GAX_order;


typedef struct GAX_replayer {

    s32 unk0;
    u8  unk1[0x8];
    s16 unkC;
    u8  unkE[0x2];
    u16 unk10;
    u8  unk12[0xA];
    s16 unk1C;

} GAX_replayer;


typedef struct GAX_volenv_point {

    u16 x;
    s16 lerp;
    u8 volume;

} GAX_volenv_point;


typedef struct GAX_volenv {

    u8 point_count;
    u8 sustain_point;
    u8 loop_start;
    u8 loop_end;
    GAX_volenv_point env_point;

} GAX_volenv;


typedef struct GAX_perf_cmd {

    u8 fx_param;
    u8 fx_type;

} GAX_perf_cmd;


typedef struct GAX_perflist {

    u8 note;
    b8 fixed;
    u8 wave_idx;
    GAX_perf_cmd commands[2];

} GAX_perflist;


typedef struct GAX_wave_param {

    // names sourced from GHX2 editor screenshot

    s16 tune;      // finetune
    b8  modulation;
    b8  pingpong;
    u32 init;      // waveform offset
    s32 min;       // loop start
    s32 max;       // loop end
    s32 mod_size;
    u16 mod_step;
    u16 mod_speed;

} GAX_wave_param;


typedef struct GAX_instrument {

    b8 blank;            // set to true if no instrument is saved
    u8 waves[MAX_WAVES]; // numerical indices into the waveform bank

    // vibrato settings
    u8 vibrato_wait;
    u8 vibrato_depth;
    u8 vibrato_speed;
    u8 unk8;

    GAX_volenv* volume_envelope;

    // performance list settings
    u8 perfstep_speed;
    u8 perfstep_count;
    u16 unk1A;
    GAX_perflist* perflist;

    GAX_wave_param wave_params[MAX_WAVES];

} GAX_instrument;


typedef struct GAX_channel {

    b8  ignore;
    u8  unk1;
    b8  empty_track;
    u8  rle_delay;
    u8  waveslot_idx;
    s8  wave_direction;
    b8  enable_modulation;
    s8  modulate_direction;
    u8  modulate_timer;
    s8  vibrato_wait;
    u8  volenv_volume;
    u8  wave_volume;
    u8  mixing_volume;
    s8  channel_index;
    s16 semitone_pitch;
    s16 vibrato_pitch;
    u8  perfstep_speed;
    b8  is_fixed;
    u8  instrument_volume;
    u8  perflist_timer;
    u8  perfstep_delay;
    b8  is_note_off;
    s16 vol_slide_val;
    s16 porta_val;
    s16 wave_vol_slide_val;
    s16 wave_porta_val;
    u16 target_pitch;
    s16 toneporta_lerp;
    u16 delay_frames;
    u16 cur_perfstep;
    u16 volenv_timer;
    u16 vibtable_index;

    GAX_instrument* instrument;
    u8* sequence;
    GAX_order* order;

    s32 wave_position;
    s32 modulate_position;
    s32 priority;
    u16 cur_pitch;
    u8  next_semitone;
    u8  next_instrument;

} GAX_channel;


typedef struct GAX_FX_channel {

    GAX_channel fxch;
    u8          fxvol;
    u8          fxnote;
    b8          nofixedfreq;
    u8          fxfreq;
    u32         fxid;

} GAX_FX_channel;


typedef struct GAX_unk0 {
    // speech related
    u8 unk0[594];
    u16 unk252;
    u8 unk254[732];
    u8 unk530;
    u8 unk531;
    u8 unk532[338];
    u8 unk684;
    u8 unk685;
    u8 unk686[178];
    u8 unk738;
    u8 unk739;
    u8 unk73A[78];
    void* unk788;
    void* unk78C;
    void* unk790;
} GAX_unk0;


typedef struct GAX_playbackvars {

    s32 unk0;
    u8  unk4[8];
    s16 unkC;
    u8  unkE[2];
    u8  speed_hi;
    u8  speed_lo;
    u8  unk15;
    u8  unk16;
    u8  unk17[0x3];
    b8  break_pattern;
    u8  unk1B;
    s16 current_step;

} GAX_playbackvars;


typedef struct GAX_player {

    GAXSongData* song;

    u32 timer_backup;
    u32 unk0;
    u16 pattern;
    u16 step;

    u16 speed_buf;
    b8  is_playing;
    u8  unknown_delay;
    u8  speed_timer;
    b8  step_finished;
    b8  pattern_finished;
    u8  global_volume;
    b8  stop_on_song_end;
    b8  is_song_end;
    b8  skip_pattern;
    b8  unk1B;

    u16 new_step_idx;
    u16 unk1E;

    GAX_channel* channels;
    GAXSongData* song_2;

    u8  unk28[32];
    u32 timer;

} GAX_player;


typedef struct GAX_playback_buffer {
    u8 unused;
    b8 update;
    u16 mixrate;
    u16 timer_reload;
} GAX_playback_buffer;


struct {

    // header

    u32             signature;     // set to GAX3
    GAXParams*      params;
    GAXSongData*    fx_data;
    GAX_FX_channel* fx_channels;
    u32             num_fx_channels;
    u32*            speech_unk;
    GAX_player*     replayer;

    // buffer properties / variables

    u8*                  unused1C;
    u32*                 unused20;
    GAX_playback_buffer* buf_header_dma1;
    GAX_playback_buffer* buf_header_dma2;
    GAX_playback_buffer* current_buf;
    vu32                 buffer_dma1;
    vu32                 buffer_dma2;
    GAX_playback_buffer* mix_buffer;
    u16                  timer_reload_dma1;
    u16                  timer_reload_dma2;
    u32                  music_pitch;
    u32                  fx_pitch;
    b8                   buffer_unk;

    u32 unk4C[0x2];
    u32 irq_state;
    u8  volboost_level;
    u8  unk59;
    u8  playback_state;
    u8  unk5B;
    u8  unk5C;
    b8  irq_finished;
    b8  unk5E;
    u8  unk5F;

    // asm code

    void* gax_tracker_asm;
    void* gax_tracker_asm_end;
    void* render_asm;
    void* gax_output_render_asm_end;
    u32   gax_filt;

    u16   unk74;
    u16   dc_correction_val;
    void* unk78;
    u32   fxch_memsize;
    u32   unk80;
    u32   free_mem;
    u8    fx_indexes[MAX_FX_CHANNELS];
    u8*   scratchmem;
    u32   scratchmem_size;

    GAX_FX_channel fxch[MAX_FX_CHANNELS];

}* GAX_ram;

