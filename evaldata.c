/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * evaldata.c ... build a simulated 128x64 display divided into 8 lines each 16 characters
 * 
 * */

#include "amsatdisplay.h"

char display[YSIZE][XSIZE];    // 16x8 display (128x64 pixel, each character is 8x8)

void init_displayarray()
{
    // init display array with spaces
    memset(display,' ',YSIZE*XSIZE);
}

void eval_downconverter(char *s)
{
    // format: OLD_xx_yy_text
    //         0123456789
    s[6] = s[9] = 0;        // terminate x and y
    
    int x = atoi(s+4);      // get x
    x /= 8;                 // x is sent as pixel, make it to character position
    if(x >= XSIZE) return;  // x position
    
    int y = atoi(s+7);      // get y
    if(y >= YSIZE) return;  // invalid line
    
    char text[XSIZE+1];     // get text and terminate it
    memset(text,' ',XSIZE);
    memcpy(text,s+10,XSIZE);
    text[XSIZE] = 0;
    
    // fill into display array
    int len = XSIZE-x;      // write to the end of the display: clears old text
    memcpy(display[y]+x,text,len);
    
    // write modified display array into the fifo
    // the data array in the fifo is NOT null terminated, just raw display contents
    //show_display((char *)display,"ORIG");
    write_fifo((unsigned char *)display,XSIZE*YSIZE);
}

/*
void show_display(char *d, char *tit)
{
    printf("%s ================\n",tit);
    for(int y=0; y<YSIZE; y++)
    {
        char t[XSIZE+1];
        memcpy(t,d+(y*XSIZE),XSIZE);
        t[XSIZE]=0;
        for(int x=0; x<16; x++)
        {
            printf("%c",d[(y*XSIZE)+x]);
        }
        printf("\n");
    }
    printf("================\n");
}
*/
