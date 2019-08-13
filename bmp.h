#pragma once

#define BMP_HEADER_SIZE_BYTES 14
#define BMP_DIB_HEADER_SIZE_BYTES 40
#define MAXIMUM_IMAGE_SIZE 256
#define BYTE_PAD(n) (n%4)

struct BMP_Header {
	char signature[2];		//ID field
	int size;		//Size of the BMP file
	short reserved1;		//Application specific
	short reserved2;		//Application specific
	int offset_pixel_array;  //Offset where the pixel array (bitmap data) can be found
};

struct DIB_Header {
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
};


