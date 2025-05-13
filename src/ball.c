#include "animframe.h"
#include "brick.h"
#include "brickout.h"
#include "enemy.h"
#include "explod.h"
#include "gfx.h"
#include "player.h"
#include "raylib.h"
#include "timer.h"
#include <math.h>

// Ball structure
typedef struct {
    Vec2 pos; // 球的中心位置
    Vec2 acceleration; // 球的加速度方向向量 (單位向量)
    float velocity; // 球的速度 (純量)
    float radius; // 球的半徑 (用於碰撞)
    AnimFrame af;
} Ball;

Ball ball = { 0 }; // 全域球物件

// 初始化球的狀態
void BallInit()
{
    ball.af = AnimFrameLoad("asset/ball.png", 16, 16);
    ball.pos = (Vec2) { SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f + 100.0f }; // 球的初始中心位置
    ball.radius = BALL_RADIUS; // 球的半徑
    ball.acceleration = (Vec2) { 0.5f, 1.0f }; // 初始加速度方向 (非單位化，將在Update中被速度影響)
                                               // 為了避免一開始就水平或垂直，給一些初始偏移
    // 正規化加速度向量 (使其長度為1)，這樣速度才能精確控制移動幅度
    float mag = sqrtf(ball.acceleration.x * ball.acceleration.x + ball.acceleration.y * ball.acceleration.y);
    if (mag > 0) {
        ball.acceleration.x /= mag;
        ball.acceleration.y /= mag;
    } else {
        ball.acceleration = (Vec2) { 0.0f, 1.0f }; // 預設向下
    }
    ball.velocity = 350.0f; // 球的移動速度 (像素/秒)
}
void BallFini()
{
    AnimFrameUnload(&ball.af);
}
// 更新球的邏輯
void BallUpdate()
{
    float deltaTime = gTimer.DeltaTime(); // 獲取幀間時間差
    // 更新球的位置
    ball.pos.x += ball.velocity * ball.acceleration.x * deltaTime;
    ball.pos.y += ball.velocity * ball.acceleration.y * deltaTime;
    // 球與磚塊的碰撞檢測 (球的中心點與半徑)
    int index = 0;
    if (EnemyCollision(ball.pos, ball.radius, &index)) {
        ball.acceleration.y *= -1; // 碰到磚塊，Y方向反彈
        PlayerAddScore(10); // 增加分數
        ExplodTryAdd(ball.pos);
        EnemyRemove(index);
    }
    // 球與牆壁的碰撞檢測
    // 左牆或右牆
    if ((ball.pos.x - ball.radius) < 0) {
        ball.pos.x = ball.radius; // 防止穿透
        ball.acceleration.x *= -1;
    }
    if ((ball.pos.x + ball.radius) > SCR_WIDTH) {
        ball.pos.x = SCR_WIDTH - ball.radius; // 防止穿透
        ball.acceleration.x *= -1;
    }
    // 上牆 (遊戲中通常不會撞到上牆就結束，除非是特殊規則)
    if ((ball.pos.y - ball.radius) < 0) {
        ball.pos.y = ball.radius; // 防止穿透
        ball.acceleration.y *= -1;
    }
    // 下牆 (球掉落，遊戲結束的邏輯通常在這裡，但目前只是反彈)
    if ((ball.pos.y + ball.radius) > SCR_HEIGHT) {
        // 實際遊戲中，這裡可能是 Game Over 或 扣生命值
        // 目前為了測試，先讓它反彈
        // ball.pos.y = SCR_HEIGHT - ball.radius; // 防止穿透
        // ball.acceleration.y *= -1;
        // 或者重置球
        BallInit();
        PlayerInit(PADDLE_W, PADDLE_H); // 可以選擇是否重置玩家分數
    }
    // 球与玩家板的碰撞檢測
    if (PlayerCollision(ball.pos, ball.radius)) {
        ball.acceleration.y *= -1; // 碰到板子，Y方向反彈
        // 可以根據碰撞點微調X方向，增加遊戲性
        ball.acceleration.x += PlayerPaddleDiff(ball.pos) * 0.5f; // 輕微影響X方向
        // 重新正規化加速度向量
        float mag = sqrtf(ball.acceleration.x * ball.acceleration.x + ball.acceleration.y * ball.acceleration.y);
        if (mag > 0) {
            ball.acceleration.x /= mag;
            ball.acceleration.y /= mag;
        }
        // 确保球向上移动
        if (ball.acceleration.y > -0.1f) { // 如果Y方向太水平或向下，强制向上
            ball.acceleration.y = -0.5f; // 給一個最小的向上速度分量
            // 再次正規化
            mag = sqrtf(ball.acceleration.x * ball.acceleration.x + ball.acceleration.y * ball.acceleration.y);
            if (mag > 0) {
                ball.acceleration.x /= mag;
                ball.acceleration.y /= mag;
            }
        }
    }
}

// 繪製球
void BallDraw()
{
    Rect sourceRec = {
        0.0f,0.0f,
        (float)ball.af.cellW, // 源矩形寬度
        (float)ball.af.cellH // 源矩形高度
    };
    Rect destRec = {
        ball.pos.x, // 目標矩形 x
        ball.pos.y, // 目標矩形 y
        (float)ball.af.cellW, // 目標矩形寬度
        (float)ball.af.cellH // 目標矩形高度
    };
    Vec2 origin = { (float)ball.af.centerW, (float)ball.af.centerH }; // 繪製原點
    DrawTexturePro(ball.af.tex, sourceRec, destRec, origin, 0.0F, WHITE); 
}