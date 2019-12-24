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

pthread_mutex_t crit_sec_dsp;
pthread_mutex_t crit_sec_dntime;
struct timeval tv_start;

// access to these variables must be thread safe !
DN_TIME dn_time;
DISPLAY display;

void init_displayarray()
{
    // init display array with spaces
    LOCK(crit_sec_dsp);
    display.length[0] = '1';
    display.length[1] = '2';
    display.length[2] = '8';
    display.type = '5';
    memset(display.data,' ',YSIZE*XSIZE);
    UNLOCK(crit_sec_dsp);
}

void init_downtime()
{
    LOCK(crit_sec_dntime);
    dn_time.length[0] = '0';
    dn_time.length[1] = '1';
    dn_time.length[2] = '4';
    dn_time.type = '6';
    UNLOCK(crit_sec_dntime);
    
    gettimeofday(&tv_start, NULL);
    dc_getruntime();
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
    {
        LOCK(crit_sec_dsp);
        memset((display.data)[1],' ',(YSIZE-1)*XSIZE);
        UNLOCK(crit_sec_dsp);
    }
    
    // clean LNB line
    if(!memcmp(text,"LNB:",4))
        len = XSIZE-x;      // write to the end of the display: clears old text
        
    // detect time
    if(text[2]==':' && text[5] == ':')
    {
        LOCK(crit_sec_dsp);
        (display.data)[1][8] = ' ';         // clear old char after the time
        memset((display.data)[2],' ',XSIZE);// clear line under the time
        UNLOCK(crit_sec_dsp);
    }
    
    // fill into display array
    LOCK(crit_sec_dsp);
    memcpy((display.data)[y]+x,text,len);
    UNLOCK(crit_sec_dsp);
}

// runtime of the down converter
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
    sprintf(s,"% 4dd %02d:%02d:%02d",day,hour,min,sec);

    LOCK(crit_sec_dntime);
    memcpy(dn_time.data,s,sizeof(dn_time.data));
    UNLOCK(crit_sec_dntime);
}

// these are the variables used to get the data and send it to PHP
// thread safty is not required
DISPLAY display_php;
DN_TIME dn_time_php;

DISPLAY get_Display()
{
    LOCK(crit_sec_dsp);
    memcpy(&display_php,&display,sizeof(DISPLAY));
    UNLOCK(crit_sec_dsp);
    return display_php;
}

DN_TIME get_dn_time()
{
    LOCK(crit_sec_dntime);
    memcpy(&dn_time_php,&dn_time,sizeof(DN_TIME));
    UNLOCK(crit_sec_dntime);
    return dn_time_php;
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
