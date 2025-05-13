#include "timer.h"
#include "raylib.h"

// 計時器結構體
typedef struct {
    float startTime; // 計時器啟動時間(毫秒)
    float pauseTime; // 暫停時的時間戳(非零表示處於暫停狀態)
    float time; // 當前幀的時間戳
    float deltaTime; // 上一幀到當前幀的時間間隔(毫秒)
} Timer;

// 全局計時器實例
static Timer timer;

// 初始化計時器
static void init()
{
    timer.startTime = (float)GetTime();
    timer.pauseTime = 0.0F; // 0表示未暫停
    timer.time = timer.startTime;
    timer.deltaTime = 0.0F; // 初始deltaTime為0
}

// 暫停計時器
static void pause()
{
    // 如果已經暫停則不做任何操作
    if (timer.pauseTime == 0.0F) {
        timer.pauseTime = (float)GetTime();
    }
}

// 恢復計時器
static void resume()
{
    // 只有當處於暫停狀態時才執行恢復操作
    if (timer.pauseTime != 0.0F) {
        float delta = (float)GetTime() - timer.pauseTime;
        timer.pauseTime = 0.0F; // 重置暫停狀態
        timer.startTime += delta; // 調整起始時間以補償暫停期間
    }
}

// 更新計時器(應在每幀調用)
static void update()
{
    // 如果處於暫停狀態，deltaTime應為0
    if (timer.pauseTime != 0.0F) {
        timer.deltaTime = 0.0F;
        return;
    }
    float ts = (float)GetTime();
    timer.deltaTime = ts - timer.time;
    timer.time = ts;
}

// 獲取上一幀的deltaTime(毫秒)
static float deltaTime(void)
{
    return timer.deltaTime;
}

// 導出的計時器接口
GameTimer gTimer = {
    .Init = init,
    .Pause = pause,
    .Resume = resume,
    .Update = update,
    .DeltaTime = deltaTime,
};