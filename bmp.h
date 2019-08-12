#pragma once

typedef struct {
	char signature[2];		//ID field
	int size;		//Size of the BMP file
	short reserved1;		//Application specific
	short reserved2;		//Application specific
	int offset_pixel_array;  //Offset where the pixel array (bitmap data) can be found
} bmp_header;

typedef struct {
    int size; // Number of bytes in DIB Header
    int width; // Width of bitmap in pixels
    int height; // Height
    short numColorPlanes;
    short numBitsPerPixel;
    int BI_RGB;
    int sizeOfRawBitmapData;
    int printResHoriz;
    int printResVert;
    int numColorsInPalette;
    int numImportantColors;
} dib_header;

typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixel;

typedef struct {
    struct Pixel *pixels;
    char *pad;
} pixel_row;

typedef struct {
    bmp_header *bmpheader;
    dib_header *dibheader;
    pixel_row *pixeldata; 
} bmp_image;




bmp_header *malloc_bmp_header() {
    bmp_header *header = (bmp_header *)malloc(sizeof(char)*2
}

