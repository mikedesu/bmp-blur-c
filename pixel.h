#pragma once

struct Pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct PixelRow {
    struct Pixel *pixels;
    char *pad;
};
