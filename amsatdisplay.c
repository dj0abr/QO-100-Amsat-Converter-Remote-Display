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
    serial_init();
    
    while(1)
    {
        eval_serialdata();
    }
        
    return 0;
}
