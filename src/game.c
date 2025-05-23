#include "ball.h"
#include "brickout.h"
#include "enemy.h"
#include "explod.h"
#include "player.h"
#include "timer.h"
#include <stdio.h>

// 遊戲整體初始化
void GameInit()
{
    EnemyInit();
    PlayerInit(PADDLE_W, PADDLE_H); // 初始化玩家，使用宏定義的尺寸
    BallInit(); // 初始化球
    ExplodInit();
    gTimer.Init();
}

// 遊戲結束清理
void GameFinish()
{
    ExplotFini();
    BallFini();
    PlayerFini();
    EnemyFini();
}

// 遊戲邏輯更新 (每幀調用)
void GameUpdate()
{
    gTimer.Update();
    // BrickUpdate(); // 更新磚塊狀態 (例如動畫)
    EnemyUpdate();
    PlayerUpdate(); // 更新玩家狀態 (處理輸入)
    BallUpdate(); // 更新球的狀態 (移動和碰撞)
    ExplodUpdate();
    EnemySpawn();
}

// 遊戲畫面繪製 (每幀調用)
void GameDraw()
{
    EnemyDraw();
    PlayerDraw(); // 繪製玩家板
    BallDraw(); // 繪製球
    ExplodDraw();
    // 繪製分數文字
    char text[64]; // 足夠長的字串緩衝區
    snprintf(text, sizeof(text), "SCORE: %d", PlayerScore()); // 使用 snprintf 更安全
    DrawText(text, 100, 10, 30, YELLOW); // 分數顯示在左下角
}