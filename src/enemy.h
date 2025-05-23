#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "brickout.h"

typedef enum EnemyType EnemyType;

typedef enum SpriteType SpriteType;

void EnemyInit();
void EnemyFini();
void EnemyTryAdd(EnemyType eType, int pathSel, float speed);
void EnemyUpdate();
void EnemyDraw();
bool EnemyCollision(Vec2 ballCenterPos, float ballRadius, int* index);
void EnemyRemove(int index);
void EnemySpawn();

#endif