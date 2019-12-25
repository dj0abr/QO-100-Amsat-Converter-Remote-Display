
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
#define UPELEMENTS  15          // max number of elements of upconverter
#define MAXUPTEXTLEN  30        // max element length of upconverter

#define XSIZE       16
#define XSIZE_PAC   32
#define YSIZE       8

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
    char data[6];               // qth locator
} QTHLOC;

typedef struct {
    char length[3];             // length of data as ascii string (better for XML/http)
    char type;                  // '8' = display contents follows
    char data[UPELEMENTS][MAXUPTEXTLEN];// all text data for max. 20 y IDs
} UPCONV;

typedef struct {
    char length[3];             // length of data as ascii string (better for XML/http)
    char type;                  // '8' = display contents follows
    char data[YSIZE][XSIZE_PAC];// 32x8 display (256x64 pixel, each character is 8x8)
} PAC;

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
UPCONV get_upconv();
void init_downtime();
void eval_upconverter(char *s);
void eval_pacontroller(char *s);
void init_upconv();
PAC get_pac();
void init_pacarray();

extern int dataready;
extern char databuf[DOWN_MAXRXLEN+1];
extern int pac_avail;
extern int upc_avail;
