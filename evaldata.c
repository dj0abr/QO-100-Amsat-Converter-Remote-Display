/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * evaldata.c ... evaluate the data from amsat boards
 * 
 * 
 * */

#include "amsatdisplay.h"

void eval_serialdata()
{
    if(dataready)
    {
        printf("%s\n",databuf);
        dataready = 0;
    }
}
