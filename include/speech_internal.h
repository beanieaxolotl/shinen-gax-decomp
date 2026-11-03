// defines //

// https://github.com/timothytylee/libgsm/blob/master/inc/private.h

#define MIN_WORD -32768
#define MAX_WORD 32767
#define GSM_MAGIC 0xD

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


typedef struct speech_state speech_state;
typedef void (*speech_filter_fn)(struct speech_state*, s16*, int, s16*, s8*);

struct speech_state {
    s16 dp0[120];              // 0x000
    s16 drp[40];               // 0x0f0
    s16 LARpp[2][8];           // 0x140
    s16 j;                     // 0x160
    u8 pad_162[0x268 - 0x162]; // 0x162
    s16 erp[40];               // 0x268
    s16 wt[160];               // 0x2b8
    s16 LARc[8];               // 0x3f8
    s16 Nc[4];                 // 0x408
    s16 Mc[4];                 // 0x410
    s16 bc[4];                 // 0x418
    s16 xmaxc[4];              // 0x420
    s16 xmc[52];               // 0x428
    u8 pad_490[0x788 - 0x490]; // 0x490
    speech_filter_fn filter;   // 0x788
    void (*SLTSF)(struct speech_state*, s16, u16, s16*, s16*); // 0x78c
    void* unk790;              // 0x790
};

typedef struct GAXSpeech_unk {
    u32      unk00;
    GAX_unk0 unk04;
    u32      unk798;
    u32      unk79C;
    u32      unk7A0;
} GAXSpeech_unk;

void GAXSpeech_internal0(s16* LARc, s16* LARpp);
void GAXSpeech_internal1(s16* LARp);
void GAXSpeech_internal3(struct speech_state* S, s16* LARc, s16* s, s8* d);
void GAXSpeech_internal5(s16* xMc, s16 mant, s16 exp, s16* xMp);
void GAXSpeech_internal6(struct speech_state *S, s16 xmaxcr, s16 Mcr, s16* xMcr, s16* erp);
void GAXSpeech_mix(s16* out, s16* in, int count, int step);
s16 speech_asl(s16 a, s32 n);
void speech_create(GAX_unk0* speech_unk);
s32 speech_decode(struct speech_state *s, u8* c, s32 target);
void STSF();
void SLTSF(GAX_unk0* S, s16 Ncr, u16 bcr, s16* erp, s16* drp);

