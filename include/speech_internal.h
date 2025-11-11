// defines //

// https://github.com/timothytylee/libgsm/blob/master/inc/private.h

#define MIN_WORD -32768
#define MAX_WORD 32767

#define GSM_ADD(a, b) ((a) + (b))
#define GSM_SUB(a, b)    ((a) - (b))
#define GSM_MULT_R(a, b) ((a) * (b) + 0x4000) >> 0xF


// structs //

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


typedef struct GAXSpeech_unk {
    u32      unk00;
    GAX_unk0 unk04;
    u32      unk798;
    u32      unk79C;
    u32      unk7A0;
} GAXSpeech_unk;