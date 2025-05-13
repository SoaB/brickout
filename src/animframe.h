#ifndef __ANIMFRAME_H__
#define __ANIMFRAME_H__

#include "raylib.h"
#include <stdint.h>

typedef struct AnimFrame {
    Texture2D tex; // Raylib 的紋理對象
    int cellW; // 單個網格單元的寬度 (像素)
    int cellH; // 單個網格單元的高度 (像素)
    int centerW; // 網格單元中心的 X 座標 (相對於單元左上角)
    int centerH; // 網格單元中心的 Y 座標 (相對於單元左上角)
    int xCellCount; // 紋理在 X 軸方向上的網格單元數量
    int yCellCount; // 紋理在 Y 軸方向上的網格單元數量
} AnimFrame;
/**
 * @brief 從文件加載紋理並創建 AnimFrame
 * @param fname 紋理圖片文件的路徑
 * @param cell_width Sprite Sheet 中每個單元的寬度
 * @param cell_height Sprite Sheet 中每個單元的高度
 * @return 初始化後的 AnimFrame 結構。如果加載失敗，返回的 AnimFrame.tex.id 為 0。
 */
AnimFrame AnimFrameLoad(const char* fname, uint16_t cell_width, uint16_t cell_height);

/**
 * @brief 卸載 AnimFrame 中的紋理資源
 * @param af 指向要卸載的 AnimFrame 的指標
 */
void AnimFrameUnload(AnimFrame* af);

#endif