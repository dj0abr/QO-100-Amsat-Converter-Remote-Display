/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * evaldata.c ... build a simulated 128x64 display divided into 8 lines each 16 characters
 * 
 * */

#include "amsatdisplay.h"

void dc_getruntime();

char display[YSIZE][XSIZE];    // 16x8 display (128x64 pixel, each character is 8x8)
struct timeval tv_start;

void init_displayarray()
{
    // init display array with spaces
    memset(display,' ',YSIZE*XSIZE);
    
    gettimeofday(&tv_start, NULL);
}

void eval_downconverter(char *s)
{
    dc_getruntime();
    
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
    int len = strlen(text);
    
    // check if display must be cleared
    // clear full display
    if(!memcmp(text,"AMSAT",5))
    {
        init_displayarray();
        
        // this text comes only once after the downconverter was powered-up
        // store the actual datetime
        gettimeofday(&tv_start, NULL);
    }
    
    // clear display beginning at line 1
    if(!memcmp(text,"Firmware",8))
        memset(display[1],' ',(YSIZE-1)*XSIZE);
    
    // clean LNB line
    if(!memcmp(text,"LNB:",4))
        len = XSIZE-x;      // write to the end of the display: clears old text
        
    // detect time
    if(text[2]==':' && text[5] == ':')
    {
        display[1][8] = ' ';            // clear old char after the time
        memset(display[2],' ',XSIZE);   // clear line under the time
    }
    
    // fill into display array
    memcpy(display[y]+x,text,len);
    
    // write modified display array into the fifo
    write_fifo('5', (unsigned char *)display,XSIZE*YSIZE);
}

void dc_getruntime()
{
    struct timeval  tv_act;
    gettimeofday(&tv_act, NULL);
    
    // compute and print the elapsed time in millisec
    unsigned long tspan;
    tspan = tv_act.tv_sec - tv_start.tv_sec;
    // tspan is the runtime in s
    int day = tspan / 24 / 3600;
    int hour = (tspan / 3600)%24;
    int min = (tspan / 60) % 60;
    int sec = tspan % 60;
    static char s[50];
    sprintf(s,"%dd %02d:%02d:%02d",day,hour,min,sec);

    // write into fifo
    write_fifo('6', (unsigned char *)s, strlen(s));
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
