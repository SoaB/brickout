#include "enemy.h"
#include "animframe.h"
#include "brickout.h" // 推測：遊戲主標頭檔或共用定義
#include "raylib.h"
#include "raymath.h"
#include "timer.h" // 提供 gTimer 的標頭檔
#include <math.h> // 因 cosf, sinf (PI 預期在 raymath.h 中定義)
#include <stdint.h> // 因 int16_t
#include <stdio.h> // 因 printf (DEBUG 時)

// ----------------------------------------------------------------------------------
// 定義 (原程式碼中沒有，但有助於閱讀或視需要調整的項目)
// ----------------------------------------------------------------------------------
#ifndef MAX_POINTS // 組成路徑的最大點數
#define MAX_POINTS 72
#endif
#ifndef MAX_ENEMYS // 可同時存在的最大敵人數
#define MAX_ENEMYS 50 // 假設值，原程式碼應有定義
#endif
#ifndef REACH_THRESH // 敵人到達路徑上下一個點的距離閾值
#define REACH_THRESH 5.0f
#endif

// 外部定義的 enum 假設 (預期在 enemy.h 或 brickout.h 中)
/*
typedef enum {
    ENEMY_NONE = 0, // 無敵人
    ENEMY_FLY,      // 飛行敵人
    // 其他敵人類型
} EnemyType;

typedef enum {
    SPRITE_NONE = 0, // 無精靈
    SPRITE_FLY,      // 飛行精靈
    // 其他精靈類型
} SpriteType;
*/

// Vec2 型別是否與 raylib.h 的 Vector2 相同，或為自訂義，尚不清楚。建議統一使用 Vector2。
// 此處將原 pArr 的 Vec2 視為 Vector2。
// typedef Vector2 Vec2; // 若 Vec2 為 Vector2 的別名，可如此設定

// ----------------------------------------------------------------------------------
// 敵人路徑相關
// ----------------------------------------------------------------------------------
typedef struct {
    Vector2 points[MAX_POINTS]; // 組成路徑的點陣列
    int pointCount; // 路徑中的點數量
} EnemyPath;

EnemyPath enemyPath[5] = { 0 }; // 最多5種敵人路徑

/**
 * @brief 根據指定參數產生路徑座標點
 *
 * @param pointsOutput 儲存產生座標點的陣列
 * @param numPoints 要產生的點數量
 * @param scaleX X軸方向縮放比例
 * @param scaleY Y軸方向縮放比例
 * @param offsetX X軸方向偏移量 (中心X)
 * @param offsetY Y軸方向偏移量 (中心Y)
 * @param initialAngleValue 角度計算初始值
 * @param angleDecrementValue 每一步驟角度減少量
 * @param cosDiv cos計算時角度的除數 (對應原碼的90.0f或180.0f)
 * @param sinDiv sin計算時角度的除數 (對應原碼的360.0f或180.0f)
 */
static void CreatePathPoints(Vector2* pointsOutput, int numPoints, float scaleX, float scaleY, float offsetX, float offsetY, int initialAngleValue, int angleDecrementValue, float cosDiv, float sinDiv)
{
    float x, y;
    int angle = initialAngleValue; // 用於角度計算的變數
    for (int i = 0; i < numPoints; i++) {
        // 使用三角函數計算點的座標
        x = scaleX * cosf((float)angle * PI / cosDiv);
        y = scaleY * sinf((float)angle * PI / sinDiv);
        pointsOutput[i].x = x + offsetX; // 加上偏移量得到最終座標
        pointsOutput[i].y = y + offsetY;
        angle -= angleDecrementValue; // 更新角度以供下一點使用
    }
}

/**
 * @brief 初始化敵人移動路徑
 * 產生5種不同的路徑
 */
static void EnemyPathInit()
{
    // 路徑0 (原程式碼的路徑)
    CreatePathPoints(enemyPath[0].points, MAX_POINTS, 320.0f, 150.0f, 400.0f, 180.0f, 360 * 4, 20, 90.0f, 360.0f);
    enemyPath[0].pointCount = MAX_POINTS;

    // 路徑1 (近似圓形的軌道，較小，約在畫面左上角)
    // 將縮放設為相同，cos/sin除數設為相同，可使軌道接近圓形。調整角度步進以完成一週。
    // 範例：360度除以MAX_POINTS作為每步角度變化(度數)
    // (float)(360 / MAX_POINTS) 作為 angleDecrementValue, initialAngleValue=0, cosDiv=180, sinDiv=180
    CreatePathPoints(enemyPath[1].points, MAX_POINTS, 100.0f, 100.0f, 200.0f, 150.0f, 0, (int)(5 * (180.0f / PI)), 180.0f, 180.0f); // angleDecrementValue 可能需要調整 (此處5為範例，表示每步5個單位角度)
    // 更簡單的範例：稍微修改原路徑參數
    // CreatePathPoints(enemyPath[1].points, MAX_POINTS, 150.0f, 320.0f, 200.0f, 250.0f, 360 * 4, 20, 90.0f, 360.0f); // X/Y縮放反轉，偏移量變更
    enemyPath[1].pointCount = MAX_POINTS;

    // 路徑2 (長條橢圓形，畫面中央上部)
    CreatePathPoints(enemyPath[2].points, MAX_POINTS, 300.0f, 100.0f, 400.0f, 120.0f, 0, 15, 180.0f, 180.0f);
    enemyPath[2].pointCount = MAX_POINTS;

    // 路徑3 (直立橢圓形，畫面右側)
    CreatePathPoints(enemyPath[3].points, MAX_POINTS, 100.0f, 250.0f, 650.0f, 300.0f, 360 * 2, 25, 180.0f, 180.0f);
    enemyPath[3].pointCount = MAX_POINTS;

    // 路徑4 (稍微修改原路徑參數：起始角度、偏移量、縮放)
    CreatePathPoints(enemyPath[4].points, MAX_POINTS, 250.0f, 120.0f, 400.0f, 450.0f, 0, 22, 120.0f, 270.0f);
    enemyPath[4].pointCount = MAX_POINTS;

    // 注意：上述路徑1至4的參數僅為範例。請根據期望的軌道進行調整。
    // 特別是 angleDecrementValue 與 cosDiv/sinDiv 的組合會影響軌道的形狀及是否閉合。
}

// ----------------------------------------------------------------------------------
// 敵人實體相關
// ----------------------------------------------------------------------------------
typedef struct {
    Vector2 pos[MAX_ENEMYS]; // 敵人目前位置
    Vector2 dirVec[MAX_ENEMYS]; // 敵人目前移動方向向量 (已正規化)
    int pathSelect[MAX_ENEMYS]; // 各敵人使用的路徑索引 (0-4)
    int currPathCount[MAX_ENEMYS]; // 各敵人目前指向的路徑上點的索引
    EnemyType eType[MAX_ENEMYS]; // 敵人種類
    SpriteType sType[MAX_ENEMYS]; // 精靈種類 (動畫用)
    float speed[MAX_ENEMYS]; // 敵人移動速度
    float frameTime[MAX_ENEMYS]; // 動畫影格經過時間
    int16_t frameCount[MAX_ENEMYS]; // 目前動畫影格編號
    int count; // 目前活動中的敵人數量
    AnimFrame af[ENEMY_NUMS - 1]; // 敵人精靈圖資訊 (所有敵人共用)
} Enemys;

Enemys enemys = { 0 }; // 敵人管理結構的全域實體

/**
 * @brief 使指定索引的敵人朝向路徑的下一個點
 *
 * @param index 目標敵人的索引
 */
static void EnemySwitchNext(int index)
{
    enemys.currPathCount[index] += 1; // 前往下一個點
    // 到達路徑終點後回到起點 (循環)
    if (enemys.currPathCount[index] >= enemyPath[enemys.pathSelect[index]].pointCount) {
        enemys.currPathCount[index] = 0;
    }
    // 新的目標點
    Vector2 target = enemyPath[enemys.pathSelect[index]].points[enemys.currPathCount[index]];
    // 計算並正規化至新目標的方向向量
    enemys.dirVec[index] = Vector2Normalize(Vector2Subtract(target, enemys.pos[index]));
}

/**
 * @brief 初始化敵人系統
 */
void EnemyInit()
{
    EnemyPathInit(); // 初始化敵人移動路徑
    enemys.af[ENEMY_FLY - 1] = AnimFrameLoad("asset/demon2.png", 64, 64); // 載入動畫影格資訊
    enemys.af[ENEMY_BUG - 1] = AnimFrameLoad("asset/enemy-01.png", 48, 48); // 載入動畫影格資訊
    enemys.af[ENEMY_SHIT - 1] = AnimFrameLoad("asset/enemy-02.png", 48, 48); // 載入動畫影格資訊
    enemys.af[ENEMY_CAKE - 1] = AnimFrameLoad("asset/enemy-03.png", 48, 48); // 載入動畫影格資訊

    for (int i = 0; i < MAX_ENEMYS; i++) {
        enemys.pathSelect[i] = 0; // 預設路徑
        enemys.currPathCount[i] = 0; // 從路徑起點開始
        enemys.speed[i] = 200.0f; // 預設速度
        enemys.eType[i] = ENEMY_NONE; // 初始狀態為非活動
        // 將初始位置設為路徑的起點
        enemys.pos[i] = enemyPath[enemys.pathSelect[i]].points[0];
        // 初始目標點 (路徑的第二個點) // 假設 pointCount > 1
        Vector2 target = enemyPath[enemys.pathSelect[i]].points[1];
        // 初始移動方向向量
        enemys.dirVec[i] = Vector2Normalize(Vector2Subtract(target, enemys.pos[i]));
        enemys.sType[i] = SPRITE_NONE; // 初始無精靈
        enemys.frameTime[i] = 0; // 重設動畫時間
        enemys.frameCount[i] = 0; // 重設動畫影格
    }
    enemys.count = 0; // 活動中敵人數為0
}

/**
 * @brief 結束敵人系統 (資源釋放等)
 */
void EnemyFini()
{
    for (int i = 0; i < ENEMY_NUMS - 1; i++) {
        AnimFrameUnload(&enemys.af[enemys.eType[i]]); // 卸載已載入的動畫影格資訊
    }
}

/**
 * @brief 嘗試產生新敵人
 *
 * @param eType 要新增的敵人種類
 * @param pathSel 使用的路徑索引
 * @param speed 敵人速度
 */
void EnemyTryAdd(EnemyType eType, int pathSel, float speed)
{
    if (enemys.count >= MAX_ENEMYS) { // 檢查是否已達敵人最大數量
#ifdef DEBUG
        printf("警告：已達到敵人數量上限。\n");
#endif
        return;
    }
    int i = enemys.count; // 新敵人的索引 (陣列末端新增)
    enemys.eType[i] = eType;
    enemys.pathSelect[i] = pathSel;
    enemys.currPathCount[i] = 1; // 初始目標為路徑的 points[1]
    enemys.speed[i] = speed;
    enemys.pos[i] = enemyPath[pathSel].points[0]; // 初始位置為路徑的 points[0]
    Vector2 target = enemyPath[pathSel].points[1]; // 初始目標
    enemys.dirVec[i] = Vector2Normalize(Vector2Subtract(target, enemys.pos[i]));
    enemys.sType[i] = SPRITE_FLY; // 假設設定為飛行型精靈
    enemys.frameTime[i] = 0;
    enemys.frameCount[i] = 0;
    enemys.count += 1; // 增加活動中敵人數量

#ifdef DEBUG
    printf("敵人數量：%d\n", enemys.count);
#endif
}

/**
 * @brief 更新敵人狀態 (移動、動畫等)
 */
void EnemyUpdate()
{
    float deltaTime = gTimer.DeltaTime(); // 取得自上一影格的經過時間
    const float reachThreshSqr = REACH_THRESH * REACH_THRESH; // 到達判定的閾值 (平方值比較)

    for (int i = 0; i < enemys.count; i++) {
        if (enemys.eType[i] == ENEMY_NONE)
            continue; // 跳過非活動的敵人

        // 更新敵人位置 (目前位置 + 方向向量 * 速度 * 經過時間)
        enemys.pos[i] = Vector2Add(enemys.pos[i], Vector2Scale(enemys.dirVec[i], enemys.speed[i] * deltaTime));

        // 目前目標點
        Vector2 target = enemyPath[enemys.pathSelect[i]].points[enemys.currPathCount[i]];
        // 計算至目標的距離平方
        float distSqr = Vector2DistanceSqr(enemys.pos[i], target);

        // 若已足夠接近目標，則切換至下一個目標
        if (distSqr < reachThreshSqr) {
            EnemySwitchNext(i);
        }

        // 更新動畫影格
        enemys.frameTime[i] += deltaTime;
        if (enemys.frameTime[i] >= 0.16f) { // 每0.16秒更新一次影格 (約6FPS動畫)
            enemys.frameTime[i] -= 0.16f;
            enemys.frameCount[i] = (enemys.frameCount[i] + 1) % enemys.af[enemys.eType[i] - 1].xCellCount; // 循環動畫影格
        }
    }
}

/**
 * @brief 移除指定索引的敵人
 *
 * @param index 要移除的敵人索引
 */
void EnemyRemove(int index)
{
    // 索引有效性檢查
    if (index < 0 || index >= enemys.count) {
#ifdef DEBUG
        printf("警告：無效的敵人移除索引 %d。\n", index);
#endif
        return;
    }
    // 若已為非活動則不進行任何操作
    if (enemys.eType[index] == ENEMY_NONE) {
#ifdef DEBUG
        printf("警告：索引 %d 的敵人已為非活動狀態。\n", index);
#endif
        return;
    }

    // 若被移除的元素不是陣列最後一個，則將最後一個元素移至該位置以填補空缺
    if (index < enemys.count - 1) {
        enemys.currPathCount[index] = enemys.currPathCount[enemys.count - 1];
        enemys.pathSelect[index] = enemys.pathSelect[enemys.count - 1];
        enemys.dirVec[index] = enemys.dirVec[enemys.count - 1];
        enemys.speed[index] = enemys.speed[enemys.count - 1];
        enemys.pos[index] = enemys.pos[enemys.count - 1];
        enemys.eType[index] = enemys.eType[enemys.count - 1];
        enemys.sType[index] = enemys.sType[enemys.count - 1];
        enemys.frameTime[index] = enemys.frameTime[enemys.count - 1];
        enemys.frameCount[index] = enemys.frameCount[enemys.count - 1];
    }
    // 將陣列最後一個元素（或被移動的原始元素）設為非活動
    enemys.eType[enemys.count - 1] = ENEMY_NONE;
    enemys.count -= 1; // 減少活動中敵人數量

#ifdef DEBUG
    printf("已從索引 %d 移除敵人。新的敵人數量：%d\n", index, enemys.count);
#endif
}

/**
 * @brief 繪製敵人
 */
void EnemyDraw()
{
    for (int i = 0; i < enemys.count; i++) {
        if (enemys.eType[i] == ENEMY_NONE)
            continue; // 不繪製非活動的敵人

        // 計算動畫影格 (假設僅有水平方向動畫)
        int frame_col = enemys.frameCount[i] % enemys.af[enemys.eType[i]-1].xCellCount;
        int frame_row = 0; // Y方向的儲存格固定為第0列 (若需依sType等變更則調整)

        // 來源精靈圖上的繪製矩形區域
        Rectangle sourceRec = {
            (float)(frame_col * enemys.af[enemys.eType[i]-1].cellW), // 影格欄 * 儲存格寬度
            (float)(frame_row * enemys.af[enemys.eType[i]-1].cellH), // 影格列 * 儲存格高度
            (float)enemys.af[enemys.eType[i]-1].cellW,
            (float)enemys.af[enemys.eType[i]-1].cellH
        };
        // 目標畫面上繪製矩形區域 (位置為敵人中心，大小為儲存格大小)
        Rectangle destRec = {
            enemys.pos[i].x, // 繪製位置 X (中心)
            enemys.pos[i].y, // 繪製位置 Y (中心)
            (float)enemys.af[enemys.eType[i]-1].cellW,
            (float)enemys.af[enemys.eType[i]-1].cellH
        };
        // 旋轉軸心 (精靈中心)
        Vector2 origin = { (float)enemys.af[enemys.eType[i]-1].centerW, (float)enemys.af[enemys.eType[i]-1].centerH };

        // 繪製紋理
        DrawTexturePro(enemys.af[enemys.eType[i]-1].tex, sourceRec, destRec, origin, 0.0f, WHITE);
    }
}

/**
 * @brief 執行指定圓形與敵人之間的碰撞偵測
 *
 * @param ballCenterPos 圓心座標
 * @param ballRadius 圓半徑
 * @param index 若發生碰撞，儲存碰撞敵人索引的指標
 * @return true 若發生碰撞
 * @return false 若未發生碰撞
 */
bool EnemyCollision(Vector2 ballCenterPos, float ballRadius, int* index)
{
    for (int i = 0; i < enemys.count; i++) {
        if (enemys.eType[i] == ENEMY_NONE) continue; // 不對非活動的敵人進行偵測

        // 定義敵人碰撞偵測用的矩形
        // 敵人位置 (enemys.pos[i]) 指向精靈的中心
        // 將 32x32 的矩形設定於中心位置
        // 左上X = 中心X - 寬度/2 = enemys.pos[i].x - 16
        // 左上Y = 中心Y - 高度/2 = enemys.pos[i].y - 16
        Rectangle enemyRect = {
            enemys.pos[i].x - 16.0f, // 修正點：原程式碼 enemys.pos[i].x + 16 位置錯誤
            enemys.pos[i].y - 16.0f, // 修正點：原程式碼 enemys.pos[i].y + 16 位置錯誤
            32.0f,                   // 寬度
            32.0f                    // 高度
        };

        // 圓形與矩形的碰撞偵測
        if (CheckCollisionCircleRec(ballCenterPos, ballRadius, enemyRect)) {
            *index = i;    // 儲存碰撞敵人的索引
            return true; // 偵測到碰撞
        }
    }
    return false; // 無碰撞
}

// ----------------------------------------------------------------------------------
// 敵人產生相關
// ----------------------------------------------------------------------------------
static float spawnTime = 0.0f; // 自上次產生後的經過時間

/**
 * @brief 隨時間經過產生敵人
 */
void EnemySpawn()
{
    spawnTime += gTimer.DeltaTime(); // 累加經過時間
    if (spawnTime > 2.0f) { // 每2秒產生一個新敵人
        // 新增 ENEMY_FLY 類型敵人，使用隨機路徑 (0-4)，速度200
        int enemyRand = GetRandomValue(1,4);
        EnemyTryAdd(enemyRand, GetRandomValue(0, 4), 200.0f);
        spawnTime = 0.0f; // 重設產生計時器
    }
}