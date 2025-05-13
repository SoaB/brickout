#include "raylib.h"
#include <stdio.h> // For snprintf

// Typedefs for convenience
typedef Vector2 Vec2; // 二維向量
typedef Rectangle Rect; // 矩形

// Screen dimensions and game element sizes
#define SCR_WIDTH 800     // 螢幕寬度
#define SCR_HEIGHT 800    // 螢幕高度
#define BRICK_W 40        // 水平方向磚塊數量
#define BRICK_H 30        // 垂直方向磚塊數量
// #define BRICKS (BRICK_W * BRICK_H) // 未使用的宏，已註解
#define BRICK_SIZE 16     // 磚塊貼圖中單個磚塊的尺寸 (像素)
#define BALL_SIZE 16      // 球的尺寸 (像素)
#define BALL_RADIUS (BALL_SIZE / 2.0f) // 球的半徑
#define PADDLE_W 64       // 玩家板寬度
#define PADDLE_H 16       // 玩家板高度
// #define EXPLOD_SIZE 32 // 未使用的宏，已註解

// Brick colors enum
typedef enum {
    BC_WHITE = 0,     // 白色磚塊 (根據原邏輯，此顏色磚塊無法銷毀)
    BC_RED,           // 紅色
    BC_PURPLE,        // 紫色
    BC_YELLOW,        // 黃色
    BC_GREEN,         // 綠色
    BC_BLUE,          // 藍色
    BC_MAXCOLOR,      // 磚塊顏色種類總數
} BrickColor;

// Game state functions
void GameInit();     // 遊戲初始化
void GameFinish();   // 遊戲結束清理
void GameUpdate();   // 遊戲邏輯更新
void GameDraw();     // 遊戲畫面繪製

// Ball functions
void BallInit();     // 球初始化
void BallUpdate();   // 球邏輯更新
void BallDraw();     // 球繪製

// Bricks functions
void BricksInit();   // 磚塊初始化
void BricksDraw();   // 磚塊繪製
void BrickUpdate();  // 磚塊邏輯更新 (例如旋轉動畫)
bool BrickCollision(Vec2 pos, float radius); // 球與磚塊的碰撞檢測

// Graphics asset functions
void InitGfx();      // 初始化圖形資源 (載入貼圖)
void FiniGfx();      // 釋放圖形資源
void DrawBrick(int color, Vec2 pos, float rotation); // 繪製單個磚塊
void DrawBall(Vec2 pos);    // 繪製球
void DrawPaddle(Vec2 pos);  // 繪製玩家板

// Player functions
void PlayerInit(float w, float h); // 玩家初始化
void PlayerUpdate(); // 玩家邏輯更新 (處理輸入)
void PlayerDraw();   // 玩家繪製
void PlayerAddScore(int score); // 增加玩家分數
bool PlayerCollision(Vec2 pos, float radius); // 球與玩家板的碰撞檢測
int PlayerScore();   // 獲取玩家當前分數

// Ball structure
typedef struct {
    Vec2 pos;          // 球的中心位置
    Vec2 acceleration; // 球的加速度方向向量 (單位向量)
    float velocity;    // 球的速度 (純量)
    float radius;      // 球的半徑 (用於碰撞)
} Ball;

Ball ball = { 0 }; // 全域球物件

// 初始化球的狀態
void BallInit()
{
    ball.pos = (Vec2){ SCR_WIDTH / 2.0f, SCR_HEIGHT / 2.0f + 100.0f }; // 球的初始中心位置
    ball.radius = BALL_RADIUS; // 球的半徑
    ball.acceleration = (Vec2){ 0.5f, 1.0f }; // 初始加速度方向 (非單位化，將在Update中被速度影響)
                                              // 為了避免一開始就水平或垂直，給一些初始偏移
    // 正規化加速度向量 (使其長度為1)，這樣速度才能精確控制移動幅度
    float mag = sqrtf(ball.acceleration.x * ball.acceleration.x + ball.acceleration.y * ball.acceleration.y);
    if (mag > 0) {
        ball.acceleration.x /= mag;
        ball.acceleration.y /= mag;
    } else {
        ball.acceleration = (Vec2){0.0f, 1.0f}; // 預設向下
    }
    ball.velocity = 350.0f;   // 球的移動速度 (像素/秒)
}

// 更新球的邏輯
void BallUpdate()
{
    float deltaTime = GetFrameTime(); // 獲取幀間時間差

    // 更新球的位置
    ball.pos.x += ball.velocity * ball.acceleration.x * deltaTime;
    ball.pos.y += ball.velocity * ball.acceleration.y * deltaTime;

    // 球與磚塊的碰撞檢測 (球的中心點與半徑)
    if (BrickCollision(ball.pos, ball.radius)) {
        ball.acceleration.y *= -1; // 碰到磚塊，Y方向反彈
        PlayerAddScore(10);        // 增加分數
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
        float paddleCenterX = player.rect.x + player.rect.width / 2;
        float hitDeltaX = (ball.pos.x - paddleCenterX) / (player.rect.width / 2); // -1 到 1
        ball.acceleration.x += hitDeltaX * 0.5f; // 輕微影響X方向

        // 重新正規化加速度向量
        float mag = sqrtf(ball.acceleration.x * ball.acceleration.x + ball.acceleration.y * ball.acceleration.y);
        if (mag > 0) {
            ball.acceleration.x /= mag;
            ball.acceleration.y /= mag;
        }
        // 确保球向上移动
        if (ball.acceleration.y > -0.1f) { // 如果Y方向太水平或向下，强制向上
             ball.acceleration.y = -0.5f; // 給一個最小的向上速度分量
             //再次正規化
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
    DrawBall(ball.pos); // ball.pos 已經是中心點
}

// Bricks structure
typedef struct {
    Vec2 pos[BRICK_H][BRICK_W];      // 各磚塊的位置 (左上角)
    float rotation[BRICK_H][BRICK_W]; // 各磚塊的旋轉角度
    bool alife[BRICK_H][BRICK_W];    // 各磚塊是否存活
    int color[BRICK_H][BRICK_W];     // 各磚塊的顏色索引
    // Rect rect; // 這個 rect 在 BricksInit 中初始化但未使用，可以移除或賦予實際用途
} Bricks;

Bricks bricks = { 0 }; // 全域磚塊物件

// 初始化磚塊
void BricksInit()
{
    // bricks.rect = (Rect) { 0, 0, 16, 16 }; // 此成員未使用，暫時註解
    for (int y = 0; y < BRICK_H; y++) {
        for (int x = 0; x < BRICK_W; x++) {
            // 磚塊位置以左上角為準，繪圖時會轉換
            bricks.pos[y][x] = (Vec2){ x * (BRICK_SIZE + 2) + 50, y * (BRICK_SIZE + 2) + 40 }; // 磚塊間加一點空隙 (+2)
            bricks.alife[y][x] = true; // 初始時所有磚塊都存在
            bricks.rotation[y][x] = 0; //GetRandomValue(0, 360); // 初始旋轉角度 (可以設為0，或隨機)
            bricks.color[y][x] = GetRandomValue(BC_RED, BC_MAXCOLOR - 1); // 隨機顏色 (避開白色，除非有意設計)
                                                                         // 如果要包含白色，則是 (0, BC_MAXCOLOR - 1)
        }
    }
}

// 更新磚塊狀態 (例如：旋轉動畫)
void BrickUpdate()
{
    float deltaTime = GetFrameTime();
    for (int y = 0; y < BRICK_H; y++) {
        for (int x = 0; x < BRICK_W; x++) {
            if (!bricks.alife[y][x]) { // 如果磚塊已消失，則跳過
                continue;
            }
            // 讓磚塊輕微旋轉 (可選的視覺效果)
            // bricks.rotation[y][x] += deltaTime * bricks.color[y][x] * 10; // 根據顏色決定旋轉速度
            // if (bricks.rotation[y][x] > 360) {
            //     bricks.rotation[y][x] -= 360;
            // }
        }
    }
}

// 繪製所有磚塊
void BricksDraw()
{
    for (int y = 0; y < BRICK_H; y++) {
        for (int x = 0; x < BRICK_W; x++) {
            if (!bricks.alife[y][x]) { // 如果磚塊已消失，則不繪製
                continue;
            }
            // 繪製磚塊時，pos 是左上角，DrawBrick 內部會處理繪圖原點
            DrawBrick(bricks.color[y][x], bricks.pos[y][x], bricks.rotation[y][x]);
        }
    }
}

// 球與磚塊的碰撞檢測
// pos: 球的中心位置, radius: 球的半徑
bool BrickCollision(Vec2 ballCenterPos, float ballRadius)
{
    for (int y = 0; y < BRICK_H; y++) {
        for (int x = 0; x < BRICK_W; x++) {
            if (!bricks.alife[y][x]) { // 如果磚塊已消失，則跳過檢測
                continue;
            }

            // 磚塊的矩形區域 (使用 bricks.pos[y][x] 作為左上角)
            Rect brickRect = { bricks.pos[y][x].x, bricks.pos[y][x].y, BRICK_SIZE, BRICK_SIZE };

            // 使用 Raylib 的圓形與矩形碰撞檢測函數
            if (CheckCollisionCircleRec(ballCenterPos, ballRadius, brickRect)) {
                // 如果白色磚塊 (BC_WHITE, 數值為0) 設計為不可摧毀，則保留此判斷
                // if (bricks.color[y][x] != BC_WHITE) {
                //    bricks.alife[y][x] = false; // 標記磚塊為已摧毀
                //}
                // 若所有顏色磚塊皆可摧毀，則移除顏色判斷：
                bricks.alife[y][x] = false; // 標記磚塊為已摧毀
                return true; // 發生碰撞
            }
        }
    }
    return false; // 未發生碰撞
}

// 全域貼圖變數
static Texture2D brickGr = { 0 };  // 磚塊的雪碧圖 (spritesheet)
static Texture2D paddleGr = { 0 }; // 玩家板的貼圖
static Texture2D ballGr = { 0 };   // 球的貼圖

// 初始化圖形資源
void InitGfx()
{
    // 請確保 asset 資料夾和這些圖片檔案存在於執行檔同級目錄下
    brickGr = LoadTexture("asset/bricks.png");
    paddleGr = LoadTexture("asset/paddle.png");
    ballGr = LoadTexture("asset/ball.png");
}

// 釋放圖形資源
void FiniGfx()
{
    UnloadTexture(brickGr);
    UnloadTexture(paddleGr);
    UnloadTexture(ballGr);
}

// 繪製單個磚塊
// color: 磚塊顏色索引, pos: 磚塊左上角位置, rotation: 旋轉角度
void DrawBrick(int color, Vec2 pos, float rotation)
{
    if (color >= BC_MAXCOLOR) { // 顏色索引超出範圍檢查
        // TraceLog(LOG_WARNING, "Error out of color range!"); // 使用 Raylib 的日誌功能
        return;
    }

    // 從磚塊雪碧圖中選取對應顏色的部分
    Rect src = (Rect) {
        .x = (float)color * BRICK_SIZE, // 根據顏色索引選擇雪碧圖的X偏移
        .y = 0,
        .width = BRICK_SIZE,
        .height = BRICK_SIZE
    };

    // 磚塊在螢幕上的目標繪製區域
    Rect dest = (Rect) { pos.x, pos.y, BRICK_SIZE, BRICK_SIZE };
    // 旋轉和繪製的原點 (磚塊中心)
    Vec2 org = (Vec2) { BRICK_SIZE / 2.0f, BRICK_SIZE / 2.0f };

    // 使用 DrawTexturePro 進行繪製，支援旋轉和縮放
    // 注意：DrawTexturePro 的 dest.x 和 dest.y 是指繪製目標矩形的左上角。
    // 但因為 org 設定為磚塊中心，實際旋轉會繞著 (pos.x + BRICK_SIZE/2, pos.y + BRICK_SIZE/2) 進行。
    DrawTexturePro(brickGr, src,
                   (Rect){ pos.x + BRICK_SIZE / 2.0f, pos.y + BRICK_SIZE / 2.0f, BRICK_SIZE, BRICK_SIZE }, // dest 的 x,y 改為中心點
                   org, rotation, WHITE);
}

// 繪製球 (pos 是球的中心點)
void DrawBall(Vec2 pos)
{
    Rect src = { 0, 0, BALL_SIZE, BALL_SIZE }; // 球在貼圖上的區域 (假設球貼圖只有一個球)
    // 球在螢幕上的目標繪製區域，pos 是中心點，所以要調整 x,y
    Rect dest = (Rect) { pos.x, pos.y, BALL_SIZE, BALL_SIZE }; // dest 的 x,y 是中心
    Vec2 org = (Vec2) { BALL_SIZE / 2.0f, BALL_SIZE / 2.0f }; // 繪圖原點為球的中心

    DrawTexturePro(ballGr, src, dest, org, 0, WHITE); // 球通常不旋轉
}

// 繪製玩家板 (pos 是板的左上角位置)
void DrawPaddle(Vec2 pos)
{
    DrawTextureV(paddleGr, pos, WHITE); // 直接使用左上角位置繪製
}

// Player structure
typedef struct {
    Rect rect;      // 玩家板的矩形區域 (x, y, width, height)
    float velocity; // 玩家板的移動速度
    int score;      // 玩家分數
    // float w;     // 寬高已包含在 rect 中，可移除
    // float h;
} Player;

Player player = { 0 }; // 全域玩家物件

// 初始化玩家
void PlayerInit(float w, float h)
{
    // player.w = w; // 已整合到 rect
    // player.h = h; // 已整合到 rect
    player.rect = (Rect){ SCR_WIDTH / 2.0f - w / 2.0f, SCR_HEIGHT - h - 20.0f, w, h }; // 初始位置在底部中央
    player.score = 0;          // 初始分數為0
    player.velocity = 500.0f;  // 移動速度 (像素/秒)
}

// 更新玩家狀態 (處理鍵盤輸入)
void PlayerUpdate()
{
    float deltaTime = GetFrameTime(); // 獲取幀間時間差

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
    DrawPaddle(pos);
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

// 遊戲整體初始化
void GameInit()
{
    InitGfx();               // 初始化圖形
    BricksInit();            // 初始化磚塊
    PlayerInit(PADDLE_W, PADDLE_H); // 初始化玩家，使用宏定義的尺寸
    BallInit();              // 初始化球
}

// 遊戲結束清理
void GameFinish()
{
    FiniGfx(); // 釋放圖形資源
}

// 遊戲邏輯更新 (每幀調用)
void GameUpdate()
{
    BrickUpdate();  // 更新磚塊狀態 (例如動畫)
    PlayerUpdate(); // 更新玩家狀態 (處理輸入)
    BallUpdate();   // 更新球的狀態 (移動和碰撞)
}

// 遊戲畫面繪製 (每幀調用)
void GameDraw()
{
    BricksDraw(); // 繪製磚塊
    PlayerDraw(); // 繪製玩家板
    BallDraw();   // 繪製球

    // 繪製分數文字
    char text[64]; // 足夠長的字串緩衝區
    snprintf(text, sizeof(text), "SCORE: %d", PlayerScore()); // 使用 snprintf 更安全
    DrawText(text, 10, SCR_HEIGHT - 40, 30, YELLOW); // 分數顯示在左下角
}

// 主函數入口
int main()
{
    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Raylib :: Brickout Enhanced"); // 初始化 Raylib 視窗
    SetTargetFPS(60); // 設定目標幀率為 60 FPS

    GameInit(); // 初始化遊戲狀態

    // 主遊戲迴圈
    while (!WindowShouldClose()) { // 當視窗未被要求關閉時循環
        GameUpdate(); // 更新遊戲邏輯

        BeginDrawing();   // 開始繪圖模式
        ClearBackground(BLACK); // 清空背景為黑色

        GameDraw();       // 繪製遊戲物件

        DrawFPS(10, 10); // 在左上角顯示 FPS
        EndDrawing();     // 結束繪圖模式
    }

    GameFinish();    // 遊戲結束前的清理工作
    CloseWindow();   // 關閉 Raylib 視窗

    return 0; // 程式正常退出
}