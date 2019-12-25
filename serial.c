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

#define SERSPEED B9600
//#define SERSPEED B115200

int fd_ser = -1; // handle of the ser interface
char serdevice[20] = {"/dev/ttyUSB0"};
int ttynum = 0;

char rxbuf[DOWN_MAXRXLEN+1]; // one extra byte for string terminator
char databuf[DOWN_MAXRXLEN+1]; // one extra byte for string terminator
int rxidx = 0;

// creates a thread to run all serial specific jobs
// call this once after program start
// returns 0=failed, 1=ok
int serial_init()
{
    while(serportnum == 0)
    {
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
            rxbyte = read_port();
            if(rxbyte != -1)
            {
                process_byte(rxbyte);
            }
            else
            {
                usleep(5);
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
                        if(++ttynum >= serportnum) ttynum = 0;
                    }
                    break;
                    
            case 1: // normal processing
                    // do nothing here
                    // in case of a serial error, set fd_ser to -1
                    // and the serial interface will be re-opened
                    break;
        }
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
    
    sprintf(serdevice,"%s",serportarr[ttynum]);
    
	closeSerial();
    
    //printf("Open: %s\n",serdevice);
    
	fd_ser = open(serdevice, O_RDWR | O_NDELAY);
	if (fd_ser < 0) {
		printf("error when opening %s, errno=%d\n", serdevice,errno);
		return -1;
	}

	if (tcgetattr(fd_ser, &tty) != 0) {
		printf("error %d from tcgetattr %s\n", errno,serdevice);
		return -1;
	}

	cfsetospeed(&tty, SERSPEED);
	cfsetispeed(&tty, SERSPEED);

	tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
	tty.c_iflag &= ~ICRNL; // binary mode (no CRLF conversion)
	tty.c_lflag = 0;

	tty.c_oflag = 0;
	tty.c_cc[VMIN] = 0; // 0=nonblocking read, 1=blocking read
	tty.c_cc[VTIME] = 0;

	tty.c_iflag &= ~(IXON | IXOFF | IXANY);

	tty.c_cflag |= (CLOCAL | CREAD);

	tty.c_cflag &= ~(PARENB | PARODD | CSTOPB);
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
    
    printf("%s opened\n",serdevice);
    
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

    //printf("%d: %c\n",idx,rxdatabyte);
    
    switch (idx)
	{
	case 0: if(rxdatabyte == 'O')
				idx++;
            if(rxdatabyte == 'U')
				idx = 101;
            if(rxdatabyte == 'P')
				idx = 201;
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
                memset(rxbuf,0,sizeof(rxbuf));
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
				// finished reading a complete sentence
				rxbuf[rxidx] = 0;	      // add string terminator
				strcpy(databuf,"OLD ");   // copy to databuf, add header which is not in rxbuf
				memcpy(databuf+4,rxbuf,DOWN_MAXRXLEN-4);
                databuf[DOWN_MAXRXLEN-1] = 0;   // terminate at the end, just to be sure
                
                // trim any non-text char at the end of the line
                for(int i=0; i<strlen(databuf); i++)
                {
                    if(databuf[i] < ' ' || databuf[i] > 'z')
                        databuf[i] = 0;
                }
                
                // the complete sentence is in databuf
                // build a simulated display
                eval_downconverter(databuf);
				idx = 0;
				break;
			}
			break;

    case 101: if(rxdatabyte == 'P')
				idx++;
			else
				idx = 0;
			break;
	case 102: if(rxdatabyte == 'C')
                idx++;
            else
                idx = 0;
            break;
	case 103:
			if(rxdatabyte == ' ')
			{
				// header found, read all data until '\n'
				rxidx = 0;
                memset(rxbuf,0,sizeof(rxbuf));
				idx++;
			}
			else
				idx = 0;
			break;

	case 104:	// read all data until '\n'
			if(rxidx >= DOWN_MAXRXLEN)
			{
				// too long, ignore
				idx = 0;
				break;
			}

			rxbuf[rxidx++] = rxdatabyte;

			if(rxdatabyte == '\n')
			{
				// finished reading a complete sentence
				rxbuf[rxidx] = 0;	      // add string terminator
				strcpy(databuf,"UPC ");   // copy to databuf, add header which is not in rxbuf
				memcpy(databuf+4,rxbuf,DOWN_MAXRXLEN-4);
                databuf[DOWN_MAXRXLEN-1] = 0;   // terminate at the end, just to be sure
                
                // trim any non-text char at the end of the line
                for(int i=0; i<strlen(databuf); i++)
                {
                    if(databuf[i] < ' ' || databuf[i] > 'z')
                        databuf[i] = 0;
                }
                
                // the complete sentence is in databuf
                // build a simulated display
                eval_upconverter(databuf);
				idx = 0;
				break;
			}
			break;

    case 201: 
            if(rxdatabyte == 'A')
				idx++;
			else
				idx = 0;
			break;
            
	case 202: if(rxdatabyte == 'C')
                idx++;
            else
                idx = 0;
            break;
	case 203:
			if(rxdatabyte == ' ')
			{
				// header found, read all data until '\n'
				rxidx = 0;
                memset(rxbuf,0,sizeof(rxbuf));
				idx++;
			}
			else
				idx = 0;
			break;

	case 204:	// read all data until '\n'
			if(rxidx >= DOWN_MAXRXLEN)
			{
				// too long, ignore
				idx = 0;
				break;
			}

			rxbuf[rxidx++] = rxdatabyte;

			if(rxdatabyte == '\n')
			{
				// finished reading a complete sentence
				rxbuf[rxidx] = 0;	      // add string terminator
				strcpy(databuf,"PAC ");   // copy to databuf, add header which is not in rxbuf
				memcpy(databuf+4,rxbuf,DOWN_MAXRXLEN-4);
                databuf[DOWN_MAXRXLEN-1] = 0;   // terminate at the end, just to be sure
                
                // trim any non-text char at the end of the line
                for(int i=0; i<strlen(databuf); i++)
                {
                    if(databuf[i] < ' ' || databuf[i] > 'z')
                        databuf[i] = 0;
                }
                
                // the complete sentence is in databuf
                // build a simulated display
                eval_pacontroller(databuf);
				idx = 0;
				break;
			}
			break;
    }
    
    return 0;
}
