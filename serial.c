/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * serial.c ... receives serial data from Amsat Boards
 * 
 * the Downconverter and Upconverter send serial data in the
 * format: 9600,n,8,1
 * 
 * */

#include "amsatdisplay.h"

int read_port();
int process_byte(int rxdatabyte);
void closeSerial();
int activate_serial();
void scanSerialPorts();

void *serproc(void *pdata);
pthread_t serial_tid; 
char serportarr[4][50];
int serportnum = 0;

#define SERSPEED_CIV B9600

int fd_ser = -1; // handle of the ser interface
char serdevice[20] = {"/dev/ttyUSB0"};
int ttynum = 0;

char rxbuf[DOWN_MAXRXLEN+1]; // one extra byte for string terminator
char databuf[DOWN_MAXRXLEN+1]; // one extra byte for string terminator
int rxidx = 0;
int dataready = 0;

// creates a thread to run all serial specific jobs
// call this once after program start
// returns 0=failed, 1=ok
int serial_init()
{
    while(serportnum == 0)
    {
        printf("no serial port found, please connect a USB/serial converter\n");
        scanSerialPorts();
        sleep(1);
    }
    
    // automatically choose an USB port
    // start a new process
    int ret = pthread_create(&serial_tid,NULL,serproc, 0);
    if(ret)
    {
        printf("catproc NOT started\n");
        return 0;
    }
    
    printf("OK: catproc started\n");
    return 1;
}

void *serproc(void *pdata)
{
int status = 0;
int rxbyte = 0;

    pthread_detach(pthread_self());
    
    while(1)
    {
        if(fd_ser == -1)
        {
            // somebody closed the serial IF, start from beginning
            status = 0;
        }
        else
        {
            // serial IF is open, receive one byte
            // or returns -1 if nothing available
            rxbyte = read_port();
            if(rxbyte != -1)
            {
                //printf("rx: %d\n",rxbyte);
                process_byte(rxbyte);
            }
        }
            
        switch (status)
        {
            case 0: // ser IF is closed, open it
                    sleep(1);
                    if(activate_serial() == 0)
                    {
                        status = 1;
                    }
                    else
                    {
                        // if open tty failed, then try the next ttyUSBx number
                        if(++ttynum >= 4) ttynum = 0;
                    }
                    break;
                    
            case 1: // normal processing
                    // do nothing here
                    // in case of a serial error, set fd_ser to -1
                    // and the serial interface will be re-opened
                    break;
        }
 
        usleep(100);
    }
    
    printf("exit serial thread\n");
    closeSerial();
    pthread_exit(NULL);
}

void closeSerial()
{
    if(fd_ser != -1) 
	{
        close(fd_ser);
	    fd_ser = -1;
		printf("serial interface closed\n");
	}
}

void scanSerialPorts()
{
    // look for available serial interfaces
    // print all avaiable and use a pipe to read back the outputs
    printf("scan for serial ports\n");
    char s[250];
    sprintf(s,"ls -1 /dev/ttyU*");
    serportnum = 0;
    FILE *fp = popen(s,"r");
    if(fp)
    {
        while (fgets(s, sizeof(s)-1, fp) != NULL) 
        {
            // delete \n
            if(strlen(s) > 5 && s[strlen(s)-1] == '\n')
                s[strlen(s)-1] = 0;
            strcpy(serportarr[serportnum++],s);
            if(serportnum >=4) break;
        }
        pclose(fp);
    }
    else
        printf("ERROR: cannot execute ls command\n");

    printf("%d ports found:\n",serportnum);
    for(int i=0; i<serportnum; i++)
        printf("%s\n", serportarr[i]);
}

// Öffne die serielle Schnittstelle
int activate_serial()
{
	struct termios tty;
    char serdevice[30];
    
    sprintf(serdevice,"/dev/ttyUSB%d",ttynum);
    
	closeSerial();
    
    printf("Open: /dev/ttyUSB%d\n",ttynum);
    
	fd_ser = open(serdevice, O_RDWR | O_NDELAY);
	if (fd_ser < 0) {
		printf("error when opening %s, errno=%d\n", serdevice,errno);
		return -1;
	}

	if (tcgetattr(fd_ser, &tty) != 0) {
		printf("error %d from tcgetattr %s\n", errno,serdevice);
		return -1;
	}

	cfsetospeed(&tty, SERSPEED_CIV);
	cfsetispeed(&tty, SERSPEED_CIV);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
	tty.c_iflag &= ~ICRNL; // binary mode (no CRLF conversion)
	tty.c_lflag = 0;

	tty.c_oflag = 0;
	tty.c_cc[VMIN] = 0; // 0=nonblocking read, 1=blocking read
	tty.c_cc[VTIME] = 0;

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);

	tty.c_cflag |= (CLOCAL | CREAD);

	tty.c_cflag &= ~(PARENB | PARODD);
	tty.c_cflag |= CSTOPB;
	tty.c_cflag &= ~CRTSCTS;
    

	if (tcsetattr(fd_ser, TCSANOW, &tty) != 0) {
		printf("error %d from tcsetattr %s\n", errno, serdevice);
		return -1;
	}
	
	// set RTS/DTR
    int flags;
    ioctl(fd_ser, TIOCMGET, &flags);
    flags &= ~TIOCM_DTR;
    flags &= ~TIOCM_RTS;
    ioctl(fd_ser, TIOCMSET, &flags);
    
    printf("/dev/ttyUSB%d opened sucessfully\n",ttynum);
    
	return 0;
}

// read one byte non blocking
int read_port()
{
static unsigned char c;

    // test if serial IF is still available
    int flags;
    if(ioctl(fd_ser, TIOCMGET, &flags) == -1)
    {
        // serial IF lost
        fd_ser = -1;
        return -1;
    }
    
    if(fd_ser != -1)
    {
        int rxlen = read(fd_ser, &c, 1);
        if( rxlen <= 0)
            return -1;

        return (unsigned int)c;
    }
    return -1;
}

int process_byte(int rxdatabyte)
{
static int idx = 0;

    //printf("%c",rxdatabyte);
    
    switch (idx)
	{
	case 0: if(rxdatabyte == 'O')
				idx++;
			break;
	case 1: if(rxdatabyte == 'L')
				idx++;
			else
				idx = 0;
			break;
	case 2: if(rxdatabyte == 'D')
				idx++;
			else
				idx = 0;
			break;
	case 3:
			if(rxdatabyte == ' ')
			{
				// header found, read all data until '\n'
				rxidx = 0;
				idx++;
			}
			else
				idx = 0;
			break;

	case 4:	// read all data until '\n'
			if(rxidx >= DOWN_MAXRXLEN)
			{
				// too long, ignore
				idx = 0;
				break;
			}

			rxbuf[rxidx++] = rxdatabyte;

			if(rxdatabyte == '\n')
			{
				// finished
				rxbuf[rxidx] = 0;	// string terminator
				memcpy(databuf,rxbuf,DOWN_MAXRXLEN);
                databuf[DOWN_MAXRXLEN-1] = 0;
                // delete \n
                if(strlen(databuf) > 5 && databuf[strlen(databuf)-1] == '\n')
                    databuf[strlen(databuf)-1] = 0;
				dataready = 1;
				idx = 0;
				break;
			}
			break;
	}
    
    return 0;
}
