#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//problem assumptions
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

struct Pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct PixelRow {
    struct Pixel *pixels;
    char *pad;
};

struct boxBlurArgs {
    int height;
    int width;
    int i;
    struct PixelRow * pixelRows;
    struct PixelRow * blurredPixelRows;
};


void print_usage(char **argv) {
    fprintf(stderr, "Usage: ./%s <input_bitmap_file> <output_bitmap_file>\n", argv[0]);
    exit(-1);
}


void * computeBoxBlur(void *blurArgs);

struct BMP_Header readBmpHeader(FILE *infile) {
    struct BMP_Header bmpHeader;
    //read bitmap file header (14 bytes)
	fread(&(bmpHeader.signature), sizeof(char)*2, 1, infile);
    fread(&(bmpHeader.size), sizeof(int), 1, infile);
    fread(&(bmpHeader.reserved1), sizeof(short), 1, infile);
    fread(&(bmpHeader.reserved2), sizeof(short), 1, infile);
    fread(&(bmpHeader.offset_pixel_array), sizeof(int), 1, infile);
    
    printf("signature: %c%c\n", bmpHeader.signature[0], bmpHeader.signature[1]);
	printf("size: %d\n", bmpHeader.size);
	printf("reserved1: %d\n", bmpHeader.reserved1);
	printf("reserved2: %d\n", bmpHeader.reserved2);
	printf("offset_pixel_array: %d\n", bmpHeader.offset_pixel_array);


    
    return bmpHeader;
}

struct DIB_Header readDibHeader(FILE *infile) {
    struct DIB_Header dibHeader;
    fread(&dibHeader.size, sizeof(int), 1, infile);
    fread(&dibHeader.width, sizeof(int), 1, infile);
    fread(&dibHeader.height, sizeof(int), 1, infile);
    fread(&dibHeader.numColorPlanes, sizeof(short), 1, infile);
    fread(&dibHeader.numBitsPerPixel, sizeof(short), 1, infile);
    fread(&dibHeader.BI_RGB, sizeof(int), 1, infile);
    fread(&dibHeader.sizeOfRawBitmapData, sizeof(int), 1, infile);
    fread(&dibHeader.printResHoriz, sizeof(int), 1, infile);
    fread(&dibHeader.printResVert, sizeof(int), 1, infile);
    fread(&dibHeader.numColorsInPalette, sizeof(int), 1, infile);
    fread(&dibHeader.numImportantColors, sizeof(int), 1, infile);

    printf("dibHeader.size: %d\n", dibHeader.size);
    printf("dibHeader.width: %d\n", dibHeader.width);
    printf("dibHeader.height: %d\n", dibHeader.height);
    printf("dibHeader.numColorPlanes: %d\n", dibHeader.numColorPlanes);
    printf("dibHeader.numBitsPerPixel: %d\n", dibHeader.numBitsPerPixel);
    printf("dibHeader.BI_RGB: %d\n", dibHeader.BI_RGB);
    printf("dibHeader.sizeOfRawBitmapData: %d\n", dibHeader.sizeOfRawBitmapData);
    printf("dibHeader.printResHoriz: %d\n", dibHeader.printResHoriz);
    printf("dibHeader.printResVert: %d\n", dibHeader.printResVert);
    printf("dibHeader.numColorsInPalette: %d\n", dibHeader.numColorsInPalette);
    printf("dibHeader.numImportantColors: %d\n", dibHeader.numImportantColors);

    return dibHeader;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv);
    }
    
	FILE* infile = fopen(argv[1], "rb");
    if (!infile) {
        fprintf(stderr, "No filename or bad filename specified\n");
        print_usage(argv);
    }
    
	//read bitmap file header (14 bytes)
    struct BMP_Header bmpHeader = readBmpHeader(infile);
	//read dib file header 
    struct DIB_Header dibHeader = readDibHeader(infile);

    int bytePad = BYTE_PAD(dibHeader.width);

    // Malloc all rows of pixels for original image
    struct PixelRow *pixelRows = malloc( (sizeof(struct Pixel *) + sizeof(char *)) * dibHeader.height );
    if (!pixelRows) {
        fprintf(stderr, "Failed to malloc pixelRows, quitting!\n");
        exit(-1);
    }

    // Malloc all rows of pixels for new image
    struct PixelRow *blurredPixelRows = malloc( (sizeof(struct Pixel *) + sizeof(char *)) * dibHeader.height );
    if (!blurredPixelRows) {
        fprintf(stderr, "Failed to malloc blurredPixelRows, quitting!\n");
        exit(-1);
    }

    // Malloc structures to hold pixel data per row (including possible byte pad)
    for (int i = 0; i < dibHeader.height; i++) {
        pixelRows[i].pixels = malloc( (sizeof(struct Pixel) * dibHeader.width) );
        blurredPixelRows[i].pixels = malloc( (sizeof(struct Pixel) * dibHeader.width) );
        if (! pixelRows[i].pixels ) {
            fprintf(stderr, "Failed to malloc pixels for pixelRows[%d], quitting!\n", i);
            exit(-1);
        }
        
        if (! blurredPixelRows[i].pixels ) {
            fprintf(stderr, "Failed to malloc pixels for blurredPixelRows[%d], quitting!\n", i);
            exit(-1);
        }

        if (bytePad > 0) {
            pixelRows[i].pad = malloc( sizeof(char) * bytePad );
            blurredPixelRows[i].pad = malloc( sizeof(char) * bytePad );
            if (! pixelRows[i].pad ) {
                fprintf(stderr, "Failed to malloc pad for pixelRows[%d], quitting!\n", i);
                exit(-1);
            }
            if (! blurredPixelRows[i].pad ) {
                fprintf(stderr, "Failed to malloc pad for blurredPixelRows[%d], quitting!\n", i);
                exit(-1);
            }
        }
        else {
            pixelRows[i].pad = NULL;
            blurredPixelRows[i].pad = NULL;
        }
    }

    // Read in the pixel data
    for (int i = 0; i < dibHeader.height; i++) {
        size_t pixelRowSize = ((sizeof(unsigned char) * dibHeader.width) * 3) + bytePad;
        
        unsigned char *pixelRow = malloc( pixelRowSize );
        
        if (pixelRow == NULL) {
            fprintf(stderr, "Failed to malloc pixelRow, quitting!\n");
            exit(-1);
        }

        fread(pixelRow, sizeof(unsigned char), (dibHeader.width * 3) + bytePad, infile);
        
        int k = 0;

        for (int j = 0; (j < pixelRowSize - bytePad) && (k < dibHeader.width) ; j+=3) {
            unsigned char b = pixelRow[j];
            unsigned char g = pixelRow[j+1];
            unsigned char r = pixelRow[j+2];

            pixelRows[i].pixels[k].r = r;
            pixelRows[i].pixels[k].g = g;
            pixelRows[i].pixels[k].b = b;
            
            k++;
        }

        for (int j = 0; j < bytePad; j++) {
            char v = pixelRow[ pixelRowSize - bytePad + j ];
            pixelRows[i].pad[j] = v;
        }
        free(pixelRow);
    }
    fclose(infile);

    FILE *outfile = fopen(argv[2], "wb");
    if (!outfile) {
        fprintf(stderr, "Failed to open outfile: %s\n", argv[2]);
        exit(-1);
    }
    
    fwrite(&bmpHeader.signature, sizeof(char)*2, 1, outfile);
    fwrite(&bmpHeader.size, sizeof(int), 1, outfile);
    fwrite(&bmpHeader.reserved1, sizeof(short), 1, outfile);
    fwrite(&bmpHeader.reserved2, sizeof(short), 1, outfile);
    fwrite(&bmpHeader.offset_pixel_array, sizeof(int), 1, outfile);

    fwrite(&dibHeader.size, sizeof(int), 1, outfile);
    fwrite(&dibHeader.width, sizeof(int), 1, outfile);
    fwrite(&dibHeader.height, sizeof(int), 1, outfile);
    fwrite(&dibHeader.numColorPlanes, sizeof(short), 1, outfile);
    fwrite(&dibHeader.numBitsPerPixel, sizeof(short), 1, outfile);
    fwrite(&dibHeader.BI_RGB, sizeof(int), 1, outfile);
    fwrite(&dibHeader.sizeOfRawBitmapData, sizeof(int), 1, outfile);
    fwrite(&dibHeader.printResHoriz, sizeof(int), 1, outfile);
    fwrite(&dibHeader.printResVert, sizeof(int), 1, outfile);
    fwrite(&dibHeader.numColorsInPalette, sizeof(int), 1, outfile);
    fwrite(&dibHeader.numImportantColors, sizeof(int), 1, outfile);

    // compute box-blur with pthreads
    for (int i = 0; i < dibHeader.height; i+=4) {
        pthread_t exampleThread[4];
        struct boxBlurArgs b[4];
        for (int j = 0; j < 4; j++) {
            b[j].height = dibHeader.height;
            b[j].width = dibHeader.width;
            b[j].i = i+j;
            b[j].pixelRows = pixelRows;
            b[j].blurredPixelRows = blurredPixelRows;

            if ( pthread_create( &exampleThread[j], NULL, computeBoxBlur, &b[j])) {
                fprintf(stderr, "Error creating thread %d on row %d\n", j, i);
                exit(-1);
            }
        }

        for (int j = 0; j < 4; j++) {
            if ( pthread_join( exampleThread[j], NULL ) ) {
                fprintf(stderr, "Error joining thread %d\n", j);
                exit(-1);
            }
        }
    }
    
    // write out file
    for (int i = 0; i < dibHeader.height; i++) {
        char *pad = pixelRows[i].pad;
        for (int j = 0; j < dibHeader.width; j++) {
            char r = blurredPixelRows[i].pixels[j].r;
            char g = blurredPixelRows[i].pixels[j].g;
            char b = blurredPixelRows[i].pixels[j].b;
            fputc(b, outfile);
            fputc(g, outfile);
            fputc(r, outfile);
        }
        if (pad) {
            for (int j = 0; j < bytePad; j++) {
                fputc(pad[j], outfile);
            }
        }
    }

    fclose(outfile);

    // end of program cleanup
    for (int i = 0; i < dibHeader.height; i++) {
        free(pixelRows[i].pixels);
        free(pixelRows[i].pad);
    }
    free(pixelRows);
    return 0;
}


void * computeBoxBlur(void *blurArgs) {
    struct boxBlurArgs *a = blurArgs;
    int height = a->height;
    int width = a->width;
    int i = a->i;
    struct PixelRow * pixelRows = a->pixelRows;
    struct PixelRow * blurredPixelRows = a->blurredPixelRows;

    if (i < height) {
        
        for (int j = 0; j < width; j++) {

            int x = i;
            int y = j;

            if (i == 0) {
                
                if (j == 0) {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j;
                    unsigned char r_u = pixelRows[x].pixels[y].r;
                    unsigned char g_u = pixelRows[x].pixels[y].g;
                    unsigned char b_u = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j+1;
                    unsigned char r_ur = pixelRows[x].pixels[y].r;
                    unsigned char g_ur = pixelRows[x].pixels[y].g;
                    unsigned char b_ur = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j+1;
                    unsigned char r_r = pixelRows[x].pixels[y].r;
                    unsigned char g_r = pixelRows[x].pixels[y].g;
                    unsigned char b_r = pixelRows[x].pixels[y].b;

                    // compute new pixel
                    unsigned char r_new = (r + r_u + r_ur + r_r) / 4;
                    unsigned char g_new = (g + g_u + g_ur + g_r) / 4;
                    unsigned char b_new = (b + b_u + b_ur + b_r) / 4;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }
                else if ( j == width - 1 ) {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j;
                    unsigned char r_u = pixelRows[x].pixels[y].r;
                    unsigned char g_u = pixelRows[x].pixels[y].g;
                    unsigned char b_u = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j-1;
                    unsigned char r_ul = pixelRows[x].pixels[y].r;
                    unsigned char g_ul = pixelRows[x].pixels[y].g;
                    unsigned char b_ul = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j-1;
                    unsigned char r_l = pixelRows[x].pixels[y].r;
                    unsigned char g_l = pixelRows[x].pixels[y].g;
                    unsigned char b_l = pixelRows[x].pixels[y].b;

                    // compute new pixel
                    unsigned char r_new = (r + r_u + r_ul + r_l) / 4;
                    unsigned char g_new = (g + g_u + g_ul + g_l) / 4;
                    unsigned char b_new = (b + b_u + b_ul + b_l) / 4;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }
                else {

                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j;
                    unsigned char r_u = pixelRows[x].pixels[y].r;
                    unsigned char g_u = pixelRows[x].pixels[y].g;
                    unsigned char b_u = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j-1;
                    unsigned char r_ul = pixelRows[x].pixels[y].r;
                    unsigned char g_ul = pixelRows[x].pixels[y].g;
                    unsigned char b_ul = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j+1;
                    unsigned char r_ur = pixelRows[x].pixels[y].r;
                    unsigned char g_ur = pixelRows[x].pixels[y].g;
                    unsigned char b_ur = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j+1;
                    unsigned char r_r = pixelRows[x].pixels[y].r;
                    unsigned char g_r = pixelRows[x].pixels[y].g;
                    unsigned char b_r = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j-1;
                    unsigned char r_l = pixelRows[x].pixels[y].r;
                    unsigned char g_l = pixelRows[x].pixels[y].g;
                    unsigned char b_l = pixelRows[x].pixels[y].b;

                    // compute new pixel
                    unsigned char r_new = (r + r_u + r_ul + r_l + r_ur + r_r) / 6;
                    unsigned char g_new = (g + g_u + g_ul + g_l + g_ur + g_r) / 6;
                    unsigned char b_new = (b + b_u + b_ul + b_l + b_ur + b_r) / 6;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }  
            }
            else if ( i != 0 && i != (height - 1)) {
                if (j == 0) {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j;
                    unsigned char r_u = pixelRows[x].pixels[y].r;
                    unsigned char g_u = pixelRows[x].pixels[y].g;
                    unsigned char b_u = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j+1;
                    unsigned char r_ur = pixelRows[x].pixels[y].r;
                    unsigned char g_ur = pixelRows[x].pixels[y].g;
                    unsigned char b_ur = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j+1;
                    unsigned char r_r = pixelRows[x].pixels[y].r;
                    unsigned char g_r = pixelRows[x].pixels[y].g;
                    unsigned char b_r = pixelRows[x].pixels[y].b;
                    
                    x = i-1;
                    y = j+1;
                    unsigned char r_dr = pixelRows[x].pixels[y].r;
                    unsigned char g_dr = pixelRows[x].pixels[y].g;
                    unsigned char b_dr = pixelRows[x].pixels[y].b; 
                    
                    x = i-1;
                    y = j;
                    unsigned char r_d = pixelRows[x].pixels[y].r;
                    unsigned char g_d = pixelRows[x].pixels[y].g;
                    unsigned char b_d = pixelRows[x].pixels[y].b;

                    // compute new pixel
                    unsigned char r_new = (r + r_u + r_ur + r_r + r_dr + r_d) / 6;
                    unsigned char g_new = (g + g_u + g_ur + g_r + g_dr + g_d) / 6;
                    unsigned char b_new = (b + b_u + b_ur + b_r + b_dr + b_d) / 6;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }
                else if ( j == width - 1 ) {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    y = i+1;
                    y = j;
                    unsigned char r_u = pixelRows[x].pixels[y].r;
                    unsigned char g_u = pixelRows[x].pixels[y].g;
                    unsigned char b_u = pixelRows[x].pixels[y].b;

                    y = i+1;
                    y = j-1;
                    unsigned char r_ul = pixelRows[x].pixels[y].r;
                    unsigned char g_ul = pixelRows[x].pixels[y].g;
                    unsigned char b_ul = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j-1;
                    unsigned char r_l = pixelRows[x].pixels[y].r;
                    unsigned char g_l = pixelRows[x].pixels[y].g;
                    unsigned char b_l = pixelRows[x].pixels[y].b;
                    
                    x = i-1;
                    y = j-1;
                    unsigned char r_dl = pixelRows[x].pixels[y].r;
                    unsigned char g_dl = pixelRows[x].pixels[y].g;
                    unsigned char b_dl = pixelRows[x].pixels[y].b;
                    
                    x = i-1;
                    y = j;
                    unsigned char r_d = pixelRows[x].pixels[y].r;
                    unsigned char g_d = pixelRows[x].pixels[y].g;
                    unsigned char b_d = pixelRows[x].pixels[y].b;

                    // compute new pixel
                    unsigned char r_new = (r + r_u + r_ul + r_l+ r_dl + r_d) / 6;
                    unsigned char g_new = (g + g_u + g_ul + g_l + g_dl + g_d) / 6;
                    unsigned char b_new = (b + b_u + b_ul + b_l+ b_dl + b_d) / 6;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }
                else {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j;
                    unsigned char r_u = pixelRows[x].pixels[y].r;
                    unsigned char g_u = pixelRows[x].pixels[y].g;
                    unsigned char b_u = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j-1;
                    unsigned char r_ul = pixelRows[x].pixels[y].r;
                    unsigned char g_ul = pixelRows[x].pixels[y].g;
                    unsigned char b_ul = pixelRows[x].pixels[y].b;

                    x = i+1;
                    y = j+1;
                    unsigned char r_ur = pixelRows[x].pixels[y].r;
                    unsigned char g_ur = pixelRows[x].pixels[y].g;
                    unsigned char b_ur = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j+1;
                    unsigned char r_r = pixelRows[x].pixels[y].r;
                    unsigned char g_r = pixelRows[x].pixels[y].g;
                    unsigned char b_r = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j-1;
                    unsigned char r_l = pixelRows[x].pixels[y].r;
                    unsigned char g_l = pixelRows[x].pixels[y].g;
                    unsigned char b_l = pixelRows[x].pixels[y].b;
                    
                    x = i-1;
                    y = j;
                    unsigned char r_d = pixelRows[x].pixels[y].r;
                    unsigned char g_d = pixelRows[x].pixels[y].g;
                    unsigned char b_d = pixelRows[x].pixels[y].b;

                    x = i-1;
                    y = j-1;
                    unsigned char r_dl = pixelRows[x].pixels[y].r;
                    unsigned char g_dl = pixelRows[x].pixels[y].g;
                    unsigned char b_dl = pixelRows[x].pixels[y].b;

                    x = i-1;
                    y = j+1;
                    unsigned char r_dr = pixelRows[x].pixels[y].r;
                    unsigned char g_dr = pixelRows[x].pixels[y].g;
                    unsigned char b_dr = pixelRows[x].pixels[y].b;

                    // compute new pixel
                    unsigned char r_new = (r + r_u + r_ul + r_l + r_ur + r_r + r_d + r_dr + r_dl) / 9;
                    unsigned char g_new = (g + g_u + g_ul + g_l + g_ur + g_r + g_d + g_dr + g_dl) / 9;
                    unsigned char b_new = (b + b_u + b_ul + b_l + b_ur + b_r + b_d + b_dr + b_dl) / 9;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }  
            }
            else if (i == (height - 1)) {
                if (j == 0) {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j+1;
                    unsigned char r_r = pixelRows[x].pixels[y].r;
                    unsigned char g_r = pixelRows[x].pixels[y].g;
                    unsigned char b_r = pixelRows[x].pixels[y].b;

                    x = i-1;
                    y = j+1;
                    unsigned char r_dr = pixelRows[x].pixels[y].r;
                    unsigned char g_dr = pixelRows[x].pixels[y].g;
                    unsigned char b_dr = pixelRows[x].pixels[y].b;

                    x = i-1;
                    y = j;
                    unsigned char r_d = pixelRows[x].pixels[y].r;
                    unsigned char g_d = pixelRows[x].pixels[y].g;
                    unsigned char b_d = pixelRows[x].pixels[y].b;
                    
                    // compute new pixel
                    unsigned char r_new = (r + r_r + r_dr + r_d) / 4;
                    unsigned char g_new = (g + + g_r + g_dr + g_d) / 4;
                    unsigned char b_new = (b + + b_r + b_dr + b_d) / 4;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }
                else if ( j == width - 1 ) {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i-1;
                    y = j;
                    unsigned char r_l = pixelRows[x].pixels[y].r;
                    unsigned char g_l = pixelRows[x].pixels[y].g;
                    unsigned char b_l = pixelRows[x].pixels[y].b;

                    x = i-1;
                    y = j-1;
                    unsigned char r_dl = pixelRows[x].pixels[y].r;
                    unsigned char g_dl = pixelRows[x].pixels[y].g;
                    unsigned char b_dl = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j-1;
                    unsigned char r_d = pixelRows[x].pixels[y].r;
                    unsigned char g_d = pixelRows[x].pixels[y].g;
                    unsigned char b_d = pixelRows[x].pixels[y].b;
                    
                    // compute new pixel
                    unsigned char r_new = (r + r_l+ r_dl + r_d) / 4;
                    unsigned char g_new = (g + g_l + g_dl + g_d) / 4;
                    unsigned char b_new = (b + b_l+ b_dl + b_d) / 4;

                    // store in new array blurredPixelRows

                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }
                else {
                    unsigned char r = pixelRows[x].pixels[y].r;
                    unsigned char g = pixelRows[x].pixels[y].g;
                    unsigned char b = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j-1;
                    unsigned char r_l = pixelRows[x].pixels[y].r;
                    unsigned char g_l = pixelRows[x].pixels[y].g;
                    unsigned char b_l = pixelRows[x].pixels[y].b;

                    x = i;
                    y = j+1;
                    unsigned char r_r = pixelRows[x].pixels[y].r;
                    unsigned char g_r = pixelRows[x].pixels[y].g;
                    unsigned char b_r = pixelRows[x].pixels[y].b;

                    x = i - 1;
                    y = j+1;
                    unsigned char r_dr = pixelRows[x].pixels[y].r;
                    unsigned char g_dr = pixelRows[x].pixels[y].g;
                    unsigned char b_dr = pixelRows[x].pixels[y].b;

                    x = i - 1;
                    y = j;
                    unsigned char r_d = pixelRows[x].pixels[y].r;
                    unsigned char g_d = pixelRows[x].pixels[y].g;
                    unsigned char b_d = pixelRows[x].pixels[y].b;

                    x = i - 1;
                    y = j - 1;
                    unsigned char r_dl = pixelRows[x].pixels[y].r;
                    unsigned char g_dl = pixelRows[x].pixels[y].g;
                    unsigned char b_dl = pixelRows[x].pixels[y].b;
                    
                    // compute new pixel
                    unsigned char r_new = (r + r_l + r_r + r_d + r_dr + r_dl) / 6;
                    unsigned char g_new = (g + g_l + g_r + g_d + g_dr + g_dl) / 6;
                    unsigned char b_new = (b + b_l + b_r + b_d + b_dr + b_dl) / 6;

                    // store in new array blurredPixelRows
                    blurredPixelRows[i].pixels[j].r = r_new;
                    blurredPixelRows[i].pixels[j].g = g_new;
                    blurredPixelRows[i].pixels[j].b = b_new;
                }  
            }
        }
    }

    return NULL;
}

