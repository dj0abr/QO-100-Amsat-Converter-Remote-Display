/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * 
 * */

#include "amsatdisplay.h"

int main()
{
char s[512];
char pip[512];

    memset(s,' ',512);
    init_fifo();
    init_displayarray();
    init_udppipe();
    serial_init();
    
    while(1)
    {
        // wait for request from browser via php
        int len = read_udppipe((unsigned char *)pip,511);
        if(len > 0)
        {
            // browser asks for new data, check if we have data in the fifo
            // only take the latest data from the fifo
            while(1)
            {
                int len = read_fifo((unsigned char *)(s+1),511);
                if(len == 0)
                    break;
            }
            
            s[0] = 0x35;   // marker: Downconverter Display data follows
            //printf("%16.16s\n",s+1);
            write_udppipe((unsigned char *)s,XSIZE*YSIZE+1);
        }
    }
        
    return 0;
}
