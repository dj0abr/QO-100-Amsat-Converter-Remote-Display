
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

int serial_init();
void eval_serialdata();

#define DOWN_MAXRXLEN 500

extern int dataready;
extern char databuf[DOWN_MAXRXLEN+1];
