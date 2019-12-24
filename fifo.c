/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * fifo.c ... thread save data for the various formats
 * 
 * */

#include "amsatdisplay.h"

// used for locking and unlocking 
pthread_mutex_t crit_sec;
#define LOCK()      pthread_mutex_lock(&crit_sec)
#define UNLOCK()    pthread_mutex_unlock(&crit_sec)

// data buffer for the downconverter display contents


struct {
    int len;
    DATASET buffer;
} fifodata[FIFO_BUFFER_LENGTH];

int wridx = 0, rdidx = 0;

int write_fifo(char type, unsigned char* data, int len)
{
    LOCK();
    
	// check if we have space in the fifo
	if (((wridx + 1) % FIFO_BUFFER_LENGTH) == rdidx)
    {
        // no more space, ignore element
        UNLOCK();
		return 0;
    }

    // prepare fifo entry
    DATASET ds;
    char s[4];
    sprintf(s,"%03d",len);
    memcpy(ds.length,s,3);
    ds.type = type;
    memcpy(ds.data,data,len);

	// write data into the fifo
    fifodata[wridx].len = len;
    memcpy(&(fifodata[wridx].buffer), &ds, sizeof(DATASET));
	if (++wridx == FIFO_BUFFER_LENGTH) wridx = 0;
    UNLOCK();

	return 1;
}

int read_fifo(DATASET *pds)
{
    LOCK();
	if (rdidx == wridx)
    {
        // no data in fifo
        UNLOCK();
		return 0;
    }

    // get data from fifo
    int len = fifodata[rdidx].len;
    memcpy(pds, &(fifodata[rdidx].buffer), sizeof(DATASET));
	if (++rdidx == FIFO_BUFFER_LENGTH) rdidx = 0;
    
    UNLOCK();

	return len;
}
