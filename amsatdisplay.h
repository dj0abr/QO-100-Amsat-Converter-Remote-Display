
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/file.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <math.h>
#include <signal.h>

#define LOCK(crit_sec)      pthread_mutex_lock(&crit_sec)
#define UNLOCK(crit_sec)    pthread_mutex_unlock(&crit_sec)

#define DOWN_MAXRXLEN 500       // serial RX
#define FIFO_BUFFER_LENGTH 100  // fifo length
#define MAXDATALEN 512          // max length of one data string

#define XSIZE   16
#define YSIZE   8

typedef struct {
    char length[3];             // length of data as ascii string (better for XML/http)
    char type;                  // '5' = display contents follows
    char data[YSIZE][XSIZE];    // 16x8 display (128x64 pixel, each character is 8x8)
} DISPLAY;

typedef struct {
    char length[3];             // length of data as ascii string (better for XML/http)
    char type;                  // '6' = display contents follows
    char data[14];              // runtime as text
} DN_TIME;

typedef struct {
    char length[3];             // length of data as ascii string (better for XML/http)
    char type;                  // '7' = display contents follows
    char data[6];               // runtime as text
} QTHLOC;

int serial_init();
void eval_downconverter(char *s);
void init_udppipe();
char write_udppipe(unsigned char *data, int len);
int read_udppipe(unsigned char *data, int maxlen);
void show_display(char *d, char *tit);
void init_displayarray();
DISPLAY get_Display();
DN_TIME get_dn_time();
QTHLOC get_qthloc();
void init_downtime();


extern int dataready;
extern char databuf[DOWN_MAXRXLEN+1];
