// https://github.com/timothytylee/libgsm/blob/master/inc/private.h

#define MIN_WORD -32768
#define MAX_WORD 32767

#define GSM_ADD(a, b) ((a) + (b))
#define GSM_SUB(a, b)    ((a) - (b))
#define GSM_MULT_R(a, b) ((a) * (b) + 0x4000) >> 0xF
