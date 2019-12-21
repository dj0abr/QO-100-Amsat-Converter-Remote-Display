/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * pipe.c ... send data to php on request from php
 * 
 * 
 * */

#include "amsatdisplay.h"

#define PORTNUM  46972
#define REQPORTNUM  46973

int pipe_sock;
int req_sock;

void init_udppipe()
{
    // open pipe for writing data to php
    pipe_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (pipe_sock == -1)
    {
        printf("Failed to create socket,errno=%d\n", errno);
        return;
    }
    
    int enable = 1;
    if (setsockopt(pipe_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");

    struct sockaddr_in sinc4fm;
    memset(&sinc4fm, 0, sizeof(struct sockaddr_in));
    sinc4fm.sin_family = AF_INET;
    sinc4fm.sin_port = htons(PORTNUM);
    //printf("open sock: %d\n", PORTNUM);
    sinc4fm.sin_addr.s_addr = INADDR_ANY;	// nur intern

    if (bind(pipe_sock, (struct sockaddr *)&sinc4fm, sizeof(struct sockaddr_in)) != 0)
    {
        printf("Failed to bind PIPE socket, errno=%d\n", errno);
        close(pipe_sock);
        pipe_sock = -1;
        return;
    }
    
    // open pipe for reading data from php
    req_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (req_sock == -1)
    {
        printf("Failed to create socket,errno=%d\n", errno);
        return;
    }
    
    enable = 1;
    if (setsockopt(req_sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");

    memset(&sinc4fm, 0, sizeof(struct sockaddr_in));
    sinc4fm.sin_family = AF_INET;
    sinc4fm.sin_port = htons(REQPORTNUM);
    //printf("open sock: %d\n", REQPORTNUM);
    sinc4fm.sin_addr.s_addr = INADDR_ANY;	// nur intern

    if (bind(req_sock, (struct sockaddr *)&sinc4fm, sizeof(struct sockaddr_in)) != 0)
    {
        printf("Failed to bind PIPE socket, errno=%d\n", errno);
        close(req_sock);
        req_sock = -1;
        return;
    }
}

// write data to php
char write_udppipe(unsigned char *data, int len)
{
	if (pipe_sock != -1)
	{
		struct sockaddr_in sin;
		sin.sin_family = AF_INET;
		sin.sin_port = htons(PORTNUM);
		sin.sin_addr.s_addr = inet_addr("127.0.0.1");
		//printf("write to sock: %d\n",  PORTNUM);
		sendto(pipe_sock, (char *)data, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
	}
	return 1;
}

// read from php
int read_udppipe(unsigned char *data, int maxlen)
{
	socklen_t fromlen;
	int len = 0;

	fromlen = sizeof(struct sockaddr_in);
	struct sockaddr_in frm_dmr;
    //printf("wait for data\n");
	len = recvfrom(req_sock, (char *)data, maxlen, 0, (struct sockaddr *)&frm_dmr, &fromlen);
	//printf("read %d bytes from sock: %d\n", len, req_sock);
	return len;
}
