#ifndef __BRICK_OUT_H__
#define __BRICK_OUT_H__
#include "raylib.h"

////////////////////////////////////////
#define DEBUG 1
////////////////////////////////////////
// Typedefs for convenience
typedef Vector2 Vec2; // 二維向量
typedef Rectangle Rect; // 矩形
// Brick colors enum
typedef enum {
    BC_NULL = 0,
    BC_WHITE, // 白色磚塊 (根據原邏輯，此顏色磚塊無法銷毀)
    BC_RED, // 紅色
    BC_PURPLE, // 紫色
    BC_YELLOW, // 黃色
    BC_GREEN, // 綠色
    BC_BLUE, // 藍色
    BC_MAXCOLOR, // 磚塊顏色種類總數
} BrickColor;
// Screen dimensions and game element sizes
#define SCR_WIDTH 800 // 螢幕寬度
#define SCR_HEIGHT 800 // 螢幕高度
#define BRICK_W 32 // 水平方向磚塊數量
#define BRICK_H 24 // 垂直方向磚塊數量
#define BRICKS (BRICK_W * BRICK_H) // 未使用的宏，已註解
#define BRICK_SIZE 16 // 磚塊貼圖中單個磚塊的尺寸 (像素)
#define BALL_SIZE 16 // 球的尺寸 (像素)
#define BALL_RADIUS (BALL_SIZE / 2.0f) // 球的半徑
#define PADDLE_W 64 // 玩家板寬度
#define PADDLE_H 16 // 玩家板高度
// #define EXPLOD_SIZE 32 // 未使用的宏，已註解

// Game state functions
void GameInit();     // 遊戲初始化
void GameFinish();   // 遊戲結束清理
void GameUpdate();   // 遊戲邏輯更新
void GameDraw();     // 遊戲畫面繪製

#endif