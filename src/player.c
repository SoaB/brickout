
#include "animframe.h"
#include "brickout.h"
#include "raylib.h"
#include "timer.h"

// Player structure
typedef struct {
    Rect rect; // 玩家板的矩形區域 (x, y, width, height)
    float velocity; // 玩家板的移動速度
    int score; // 玩家分數
    AnimFrame af;
} Player;

Player player = { 0 }; // 全域玩家物件

// 初始化玩家
void PlayerInit(float w, float h)
{
    player.af = AnimFrameLoad("asset/paddle.png", w, h);
    player.rect = (Rect) { SCR_WIDTH / 2.0f - w / 2.0f, SCR_HEIGHT - h - 20.0f, w, h }; // 初始位置在底部中央
    player.score = 0; // 初始分數為0
    player.velocity = 500.0f; // 移動速度 (像素/秒)
}
void PlayerFini()
{
    AnimFrameUnload(&player.af);
}
// 更新玩家狀態 (處理鍵盤輸入)
void PlayerUpdate()
{
    float deltaTime = gTimer.DeltaTime(); // 獲取幀間時間差

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) { // 左移
        player.rect.x -= player.velocity * deltaTime;
        if (player.rect.x < 0) { // 防止移出左邊界
            player.rect.x = 0;
        }
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) { // 右移
        player.rect.x += player.velocity * deltaTime;
        if ((player.rect.x + player.rect.width) > SCR_WIDTH) { // 防止移出右邊界
            player.rect.x = SCR_WIDTH - player.rect.width;
        }
    }
}
// 繪製玩家板
void PlayerDraw()
{
    Vec2 pos = { player.rect.x, player.rect.y };
    DrawTextureV(player.af.tex, pos, WHITE); // 直接使用左上角位置繪製
}
// 增加玩家分數
void PlayerAddScore(int score)
{
    player.score += score;
}
// 球與玩家板的碰撞檢測
// pos: 球的中心位置, radius: 球的半徑
bool PlayerCollision(Vec2 ballCenterPos, float ballRadius)
{
    // 使用 Raylib 的圓形與矩形碰撞檢測函數
    if (CheckCollisionCircleRec(ballCenterPos, ballRadius, player.rect)) {
        return true; // 發生碰撞
    }
    return false; // 未發生碰撞
}
// 獲取玩家當前分數
int PlayerScore()
{
    return player.score;
}
// get paddle hit point
float PlayerPaddleDiff(Vec2 pos)
{
    float paddleCenterX = player.rect.x + player.rect.width / 2;
    float hitDeltaX = (pos.x - paddleCenterX) / (player.rect.width / 2); // -1 到 1
    return hitDeltaX;
}