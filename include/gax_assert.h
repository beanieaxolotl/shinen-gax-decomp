#include "GAX_font.h"


#ifndef GAX_ASSERT_H
#define GAX_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif


void GAX_ASSERT(const char* fn, const char* msg);
void GAX_ASSERT_PRINT(int x, int y, const char* string);


#ifdef __cplusplus
}
#endif

#endif
