#include "boxblur.h"

#define NULL 0

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
