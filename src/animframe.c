#include "animframe.h"
#include <stdio.h>
/**
 * @brief 從文件加載紋理並創建 AnimFrame
 */
AnimFrame AnimFrameLoad(const char* fname, uint16_t cell_width, uint16_t cell_height)
{
    AnimFrame a = { 0 }; // 初始化結構體

    a.tex = LoadTexture(fname); // 使用 Raylib 加載紋理
    // 檢查紋理是否加載成功 (Raylib 中，失敗時 id 為 0)
    if (a.tex.id == 0) {
        printf("Error: Failed to load texture from %s\n", fname);
        // 加載失敗，直接返回包含無效紋理 ID 的結構
        // 調用者應檢查返回的 a.tex.id
        return a;
    }
    // 檢查傳入的單元格尺寸是否有效
    if (cell_width <= 0 || cell_height <= 0) {
        // 如果尺寸無效，將相關尺寸設為 0，避免後續計算出錯
        a.cellW = 0;
        a.cellH = 0;
        a.centerW = 0;
        a.centerH = 0;
        a.xCellCount = 0;
        a.yCellCount = 0;
        printf("Warning: Invalid cell width or height (%d, %d) for texture %s\n", cell_width, cell_height, fname);
    } else {
        // 尺寸有效，計算相關屬性
        a.cellW = cell_width;
        // 計算中心點 (整數除法，對於奇數寬高可能會有 0.5px 的偏差)
        a.centerW = a.cellW / 2;
        a.cellH = cell_height;
        a.centerH = a.cellH / 2;
        // 計算 X 和 Y 方向上的單元格數量
        // POTENTIAL ISSUE: 如果紋理尺寸不能被 cellW/cellH 整除，這裡會取整，可能導致最後一行/列不完整或計算錯誤
        a.xCellCount = a.tex.width / a.cellW;
        a.yCellCount = a.tex.height / a.cellH;
    }
    return a; // 返回初始化後的 AnimFrame
}
/**
 * @brief 卸載 AnimFrame 中的紋理資源
 */
void AnimFrameUnload(AnimFrame* af)
{
    // 使用 Raylib 的函數卸載紋理，釋放 GPU 內存
    UnloadTexture(af->tex);
    // 可以選擇性地將 af->tex.id 設為 0，表示已卸載
    af->tex.id = 0;
}
