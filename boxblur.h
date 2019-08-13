#pragma once

#include "pixel.h"

struct boxBlurArgs {
    int height;
    int width;
    int i;
    struct PixelRow * pixelRows;
    struct PixelRow * blurredPixelRows;
};

void * computeBoxBlur(void *blurArgs);
