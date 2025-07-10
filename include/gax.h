#include "gba\gba.h"

/********************************************************************
 *                     GAX Sound Engine                             *
 * use only in compliance with a signed Audio Development Agreement * 
 *          Copyright (C) 2000-2002 Shin'en Multimedia              * 
 ********************************************************************/

// Version: 3.04

#ifndef _GAX_H_included
#define _GAX_H_included

#ifdef __cplusplus
extern "C" {
#endif


// made redundant with include/gba/types.h

/*------------------------------------------------------------------*/
/*                      Redefine Data Size                          */
/*------------------------------------------------------------------*/
/*#ifndef _AGB_TYPES_H
	typedef     unsigned char           u8;
	typedef     unsigned short int      u16;
	typedef     unsigned int            u32;
	typedef     unsigned long long int  u64;
	
	typedef     signed char             s8;
	typedef     signed short int        s16;
	typedef     signed int              s32;
	typedef     signed long long int    s64;
	
	typedef     float                   f32;
	typedef     double                  f64;
	
	
	#define     vl                      volatile
	
	typedef     vl u8                   vu8;
	typedef     vl u16                  vu16;
	typedef     vl u32                  vu32;
	typedef     vl u64                  vu64;
	
	typedef     vl s8                   vs8;
	typedef     vl s16                  vs16;
	typedef     vl s32                  vs32;
	typedef     vl s64                  vs64;
	
	typedef     vl f32                  vf32;
	typedef     vl f64                  vf64;
#endif*/


#ifndef b8
	typedef	u8 b8;
#endif

#define GAX_DEFAULT         (~0)
#define GAX_REVERB          1
#define GAX_FX_REVERB       2
#define GAX_LOWPASS_FILTER  4
#define GAX_STOP_ON_SONGEND 8
#define GAX_NO_JINGLES      16
#define GAX_HALFRATE_FX     32
#define GAX_SPEECH          64

typedef struct GAXReverb {
	u32 delay;
	u32 decay;
} GAXReverb;

typedef struct GAXChannelInfo {
	u8 instrument;
} GAXChannelInfo;
 
typedef struct GAXParams {
// input
	u8  *workmem;
	u32 workmem_size;
	u16 mixing_rate, fx_mixing_rate;
	u16 filter_depth;
	u16 flags;
	u16 fx_channels;
	u16 volume;
	s8  speed_adjust;
	GAXReverb reverb[3];
	const void* fx_data;
	const void* song_data;
	GAXChannelInfo* channel_info;

	b8  debug;
// output
	b8  is_songend;
	b8  is_jingleend;
	u8  mixermode;
} GAXParams;

typedef struct GAXFXParams {
	u16 fxid;
	u16 fxch;
	s32 prio;
	u32 note;
	u16 volume;
} GAXFXParams;

void GAX2_new(GAXParams* params);
void GAX2_new_fx(GAXFXParams* fxparams);
void GAX2_calc_mem(GAXParams* params);
  b8 GAX2_init(GAXParams* params);
  b8 GAX2_jingle(const void* jingle);
 u32 GAX2_fx(const GAXFXParams* fxparams);
  
  ///////////////////////////ENDE NEU/////////////////////////////

typedef struct GAXVoid {
	void* dummy;
} GAXVoid;

// main
void GAX_irq();
void GAX_play();
void GAX_pause();
void GAX_resume();
void GAX_pause_music();
void GAX_resume_music();
 u32 GAX_backup_fx(s32 fxch, void* buf);
void GAX_restore_fx(s32 fxch, const void* buf);
 u32 GAX_fx(u32 fxid);
 u32 GAX_fx_ex(u32 fxid, s32 fxch, s32 prio, s32 note);
void GAX_fx_note(s32 fxch, s32 note);
 u32 GAX_fx_status(s32 fxch);
void GAX_stop_fx(s32 fxch);
void GAX_set_music_volume(s32 ch, u32 vol);
void GAX_set_fx_volume(s32 fxch, u32 vol);
void GAX_stop();

// internal, don't use!
 u32 GAX_get_free_mem();

#ifdef __cplusplus
}
#endif

#endif