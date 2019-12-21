/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * fifo.c ... thread save fifo to send data from the serial thread to the main thread
 * 
 * the fifo can store BUFFER_LENGTH elements, each element is a text string with maxlength BUFFER_ELEMENT_SIZE
 * 
 * */

#include "amsatdisplay.h"

pthread_mutex_t crit_sec;
#define LOCK()	pthread_mutex_lock(&crit_sec)
#define UNLOCK()	pthread_mutex_unlock(&crit_sec)


#define BUFFER_ELEMENT_SIZE 512
#define BUFFER_LENGTH 100
unsigned char buffer[BUFFER_LENGTH][BUFFER_ELEMENT_SIZE+1];

int wridx = 0, rdidx = 0;

void init_fifo()
{
    memset(buffer,0,BUFFER_LENGTH*(BUFFER_ELEMENT_SIZE+1));
}

int write_fifo(unsigned char *data, int len)
{
	// check if element contains data and is not too long
	if (len == 0 || len >= (BUFFER_ELEMENT_SIZE - 2)) 
    {
        printf("invalid element size: %d\n",len);
        return 0;
    }

    LOCK();
	// check if we have space in the fifo
	if (((wridx + 1) % BUFFER_LENGTH) == rdidx)
    {
        // no more space, ignore element
        UNLOCK();
        printf("FIFO full\n");
		return 0;
    }

	// write data into the fifo
	// write the length into the firast two bytes, 16 bit MSB first
	buffer[wridx][0] = len>>8;
    buffer[wridx][1] = len;
	// insert the data
	memcpy(buffer[wridx]+2, data, len);
	if (++wridx == BUFFER_LENGTH) wridx = 0;
    UNLOCK();

	return 1;
}

int read_fifo(unsigned char *data, int maxlen)
{
    LOCK();
	if (rdidx == wridx)
    {
        // no data in fifo
        UNLOCK();
		return 0;
    }

    // read the length of the data
    int len;
    len = buffer[rdidx][0];
    len <<= 8;
    len += buffer[rdidx][1];
    if(len > maxlen)
    {
        printf("readfifo: invalid length:%d allowed:%d\n",len,maxlen);
        UNLOCK();
        return 0;
    }
    
	memcpy(data, buffer[rdidx]+2, len);
	if (++rdidx == BUFFER_LENGTH) rdidx = 0;
    
    UNLOCK();
    
	//printf("from fifo:%d:%s\n\n",len,data);
	return len;
}
