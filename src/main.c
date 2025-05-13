#include "brickout.h"
#include "raylib.h"

#include <stdio.h>

// 主函數入口
int main()
{
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(SCR_WIDTH, SCR_HEIGHT, "Raylib :: Brickout Enhanced"); // 初始化 Raylib 視窗
    SetTargetFPS(60); // 設定目標幀率為 60 FPS
    GameInit(); // 初始化遊戲狀態
    // 主遊戲迴圈
    while (!WindowShouldClose()) { // 當視窗未被要求關閉時循環
        GameUpdate(); // 更新遊戲邏輯
        BeginDrawing(); // 開始繪圖模式
        ClearBackground(BLACK); // 清空背景為黑色
        GameDraw(); // 繪製遊戲物件
#ifdef DEBUG
        DrawFPS(10, 10); // 在左上角顯示 FPS
#endif
        EndDrawing();     // 結束繪圖模式
    }
    GameFinish();    // 遊戲結束前的清理工作
    CloseWindow();   // 關閉 Raylib 視窗
    return 0; // 程式正常退出
}