
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


int serial_init();
void eval_downconverter(char *s);
void init_udppipe();
char write_udppipe(unsigned char *data, int len);
int read_udppipe(unsigned char *data, int maxlen);
int write_fifo(unsigned char *data, int len);
int read_fifo(unsigned char *data, int maxlen);
void show_display(char *d, char *tit);
void init_displayarray();
void init_fifo();

#define XSIZE   16
#define YSIZE   8

#define DOWN_MAXRXLEN 500

extern int dataready;
extern char databuf[DOWN_MAXRXLEN+1];
