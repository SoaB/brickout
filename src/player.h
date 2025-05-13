#ifndef __PADDLE_H__
#define __PADDLE_H__
#include "brickout.h"

// Player functions
void PlayerInit(float w, float h); // 玩家初始化
void PlayerFini();
void PlayerUpdate(); // 玩家邏輯更新 (處理輸入)
void PlayerDraw(); // 玩家繪製
void PlayerAddScore(int score); // 增加玩家分數
bool PlayerCollision(Vec2 pos, float radius); // 球與玩家板的碰撞檢測
int PlayerScore(); // 獲取玩家當前分數
float PlayerPaddleDiff(Vec2 pos);
#endif