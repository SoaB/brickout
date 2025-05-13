#ifndef __EXPLOD_H__
#define __EXPLOD_H__

#include "animframe.h"
#include "brickout.h"

void ExplodInit();
void ExplotFini();
void ExplodTryAdd(Vec2 pos);
void ExplodUpdate();
void ExplodDraw();

#endif