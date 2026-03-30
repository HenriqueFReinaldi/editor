#ifndef RENDER
#define RENDER

#include "gap.h"

extern int relative_mode;
extern char* cursor;
void render(GapBuffer gb, int s, int down_offset);

#endif