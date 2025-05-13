#include "explod.h"
#include "animframe.h"
#include "brickout.h"
#include "raylib.h"
#include "timer.h"
#include <stdint.h>
#include <stdio.h>

#define MAX_EXPLODS 100
#define EXPLOD_TIME 0.1f

typedef struct {
    AnimFrame af;
    Vec2 pos[MAX_EXPLODS];
    float frameTime[MAX_EXPLODS];
    float lifeTime[MAX_EXPLODS];
    int16_t frameCount[MAX_EXPLODS];
    int32_t count;
} Explod;

Explod explods = { 0 };

void ExplodInit()
{
    explods.af = AnimFrameLoad("asset/explod.png", 32, 32);
    for (int i = 0; i < MAX_EXPLODS; i++) {
        explods.pos[i] = (Vec2) { 0, 0 };
        explods.frameTime[i] = 0;
        explods.lifeTime[i] = 0;
        explods.frameCount[i] = 0;
    }
    explods.count = 0;
}
void ExplotFini()
{
    AnimFrameUnload(&explods.af);
}

void ExplodTryAdd(Vec2 pos)
{
    for (int i = 0; i < MAX_EXPLODS; i++) {
        if (explods.lifeTime[i] <= 0) { // 如果生命週期 <= 0，表示此爆炸效果已結束或未使用
            explods.pos[i] = pos;
            explods.lifeTime[i] = 1.0f;
            explods.frameTime[i] = 0;
            explods.frameCount[i] = 0;
            // 如果使用的索引超出了目前的計數器，則擴大計數器範圍
            // 這確保了 Update 和 Draw 迴圈會檢查到這個新啟動的爆炸
            if (i >= explods.count) {
                explods.count = i + 1; // 更新計數器為目前使用的最大索引 + 1
            }
            return; // 找到並啟動後即可返回
        }
    }
    // 如果迴圈結束仍未找到閒置的，表示爆炸效果池已滿，無法再添加
#ifdef DEBUG
    printf("Warning: Explod pool is full. Cannot add explosion.\n");
#endif
}

void ExplodUpdate()
{
    float deltaTime = gTimer.DeltaTime();
    // 遍歷到目前為止使用過的最高索引
    for (int i = 0; i < explods.count; i++) {
        if (explods.lifeTime[i] > 0) { // 只更新生命週期 > 0 的活躍爆炸
            explods.lifeTime[i] -= deltaTime;
            if (explods.lifeTime[i] <= 0) {
                explods.lifeTime[i] = 0;
            }
            explods.frameTime[i] += deltaTime;
            if (explods.frameTime[i] >= EXPLOD_TIME) {
                explods.frameTime[i] -= EXPLOD_TIME;
                explods.frameCount[i]++;
                if (explods.frameCount[i] >= explods.af.xCellCount) {
                    explods.frameCount[i] = 0;
                }
            }
        }
    }
}

void ExplodDraw()
{
    for (int i = 0; i < explods.count; i++) {
        if (explods.lifeTime[i] == 0) {
            continue;
        }
        int frame_col = explods.frameCount[i] % explods.af.xCellCount;
        int frame_row = 0;
        Rect sourceRec = {
            (float)(frame_col * explods.af.cellW), // X 座標 = 列號 * 單元寬度
            (float)(frame_row * explods.af.cellH), // Y 座標 = 行號 * 單元高度 (修正 Bug)
            (float)explods.af.cellW, // 寬度 = 單元寬度
            (float)explods.af.cellH // 高度 = 單元高度
        };
        Rect destRec = {
            explods.pos[i].x, // 目標 X 座標
            explods.pos[i].y, // 目標 Y 座標
            (float)explods.af.cellW, // 繪製寬度
            (float)explods.af.cellH // 繪製高度
        };
        Vec2 origin = (Vec2) { (float)explods.af.centerW, (float)explods.af.centerH };
        DrawTexturePro(explods.af.tex, sourceRec, destRec, origin, 0.0F, WHITE);
    }
}