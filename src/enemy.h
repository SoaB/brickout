#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "brickout.h"

#define MAX_POINTS 100
#define MAX_ENEMYS 100
#define REACH_THRESH 3.01f

typedef enum {
    ENEMY_NONE = 0,
    ENEMY_FLY,
    ENEMY_NUMS,
} EnemyType;

typedef enum {
    SPRITE_NONE = 0,
    SPRITE_FLY,
    SPRITE_NUMS,
} SpriteType;

void EnemyInit();
void EnemyFini();
void EnemyTryAdd(EnemyType eType, int pathSel, float speed);
void EnemyUpdate();
void EnemyDraw();
bool EnemyCollision(Vec2 ballCenterPos, float ballRadius, int* index);
void EnemyRemove(int index);
void EnemySpawn();

#endif