#ifndef __DMA2D_H_
#define __DMA2D_H_
#include "main.h"

void _DMA2D_Fill(void * pDst, uint32_t OffLine, uint32_t xSize, uint32_t ySize, uint32_t ColorIndex, uint32_t PixelFormat);
void _DMA2D_Copy(void * pSrc, uint32_t OffLineSrc, void * pDst, uint32_t OffLineDst, uint32_t xSize, uint32_t ySize, uint32_t PixelFormat);
void _DMA2D_MixColorsBulk(uint32_t * pColorFG, uint32_t OffLineSrcFG, uint32_t * pColorDst, uint32_t OffLineDst, uint32_t xSize, uint32_t ySize, uint8_t Intens);
void _DMA2D_AlphaBlendingBulk(uint32_t * pColorFG, uint32_t OffLineSrcFG, uint32_t * pColorBG, uint32_t OffLineSrcBG, uint32_t * pColorDst, uint32_t OffLineDst, uint32_t xSize, uint32_t ySize);
void _DMA2D_DrawAlphaBitmap(void  * pDst, uint32_t OffLineSrc, void  * pSrc, uint32_t OffLineDst, uint32_t xSize, uint32_t ySize, uint32_t PixelFormat);

#endif