/*
 * Amsat Remote Display
 * ====================
 * by DJ0ABR 
 * 
 * 
 * */

#include "amsatdisplay.h"

// cleans white spaces from beginning, middle and end of a string
char *cleanString(char *str, int cleanspace)
{
static char hs[256];
char *hp = str;
int idx = 0;

    // putze den Anfang
    while(*hp == ' ' || *hp == ',' || *hp == '\n' || *hp == '\r' || *hp == '\'' || *hp == '\"')
    {
 hp++;
    }
    
    // putze die Mitte
    if(cleanspace)
    {
 while(*hp)
 {
     if(*hp != ' ' && *hp != ',' && *hp != '\n' && *hp != '\r' && *hp != '\'' && *hp != '\"')
  hs[idx++] = *hp;
     hp++;
 }
    }
    else
    {
 while(*hp)
     hs[idx++] = *hp++;
    }
    
    // putze das Ende
    hp = hs+idx-1;

    while(*hp == ' ' || *hp == ',' || *hp == '\n' || *hp == '\r' || *hp == '\'' || *hp == '\"')
    {
 *hp = 0;
 hp--;
    }
    

    hs[idx] = 0;

    return hs;
}

// check if it is already running
void isRunning()
{
    int num = 0;
    char s[256];
    sprintf(s,"ps -e | grep amsatdisplay");
    
    FILE *fp = popen(s,"r");
    if(fp)
    {
        // gets the output of the system command
        while (fgets(s, sizeof(s)-1, fp) != NULL) 
        {
            if(strstr(s,"amsatdisplay") && !strstr(s,"grep"))
            {
                if(++num == 2)
                {
                    printf("amsatdisplay is already running, do not start twice !");
                    pclose(fp);
                    exit(0);
                }
            }
        }
        pclose(fp);
    }
}

int sret;
void *pret;
char htmldir[256] = { "." };

void searchHTMLpath()
{
    if (getuid())
    {
        //printf("\nYou are not root, HTML files are not automatically copied into the Apache folder!\n");
        return;
    }
    
    if(htmldir[0] == '.')
    {
        // search for the apache woking directory
        sret = system("find  /srv -xdev -name htdocs  -print > pfad 2>/dev/null");
        sret = system("find  /var -xdev -name htdocs  -print >> pfad 2>/dev/null");
        sret = system("find  /var -xdev -name html  -print >> pfad 2>/dev/null");
        sret = system("find  /usr -xdev -name htdocs  -print >> pfad 2>/dev/null");
        // if the directory was found its name is in file: "pfad"
        FILE *fp=fopen("./pfad","r");
        if(fp)
        {
            char p[256];
            pret = fgets(p,255,fp);
            char *cp= cleanString(p,0);
            if(strlen(cp)>3)
            {
                strcpy(htmldir,cp);
                printf("Webserver Path: %s\n",htmldir);
            }
            else
            {
                printf("Path to apache webserver files not found");
                exit(0);
            }
            
            fclose(fp);
        }
        else
        {
            printf("Path to apache webserver files not found");
            exit(0);
        }
    }
}

void installHTMLfiles()
{
    // the HTML files are located in the html folder below the current folder
    // copy these files into the Apache HTML folder
    int i;
    if ((i=getuid()))
    {
        //printf("\nYou are not root, HTML files are not automatically copied into the Apache folder!\n");
        return;
    }
    
    char fn[512];
    snprintf(fn,sizeof(fn),"cp -v ./html/* %s",htmldir);
    printf("copy Web Site files to: %s\n",htmldir);
    FILE *fp = popen(fn,"r");
    if(fp)
    {
        // Liest die Ausgabe von cp
        while (fgets(fn, sizeof(fn)-1, fp) != NULL) 
        {
            printf("copy: %s",fn);
        }
        pclose(fp);
    }
    
    if(getuid() == 0)
    {
        printf("\n==================================================\n");
        printf("Installation complete\nnow start the program as normal user by entering:\n./ad\n");
        printf(  "==================================================\n");
        exit(0);
    }
}

void sighandler(int signum)
{
	printf("signal %d, exit program\n",signum);
    
    exit(0);
}

int main()
{
char s[512];
char pip[512];

    // check if it is already running, if yes then exit
    isRunning();
    
    // look for the apache HTML path
    searchHTMLpath();
    
    // Install or Update the html files
    installHTMLfiles();
    
    // signal handler, mainly used if the user presses Ctrl-C
    struct sigaction sigact;
    sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	//sigaction(SIGPIPE, &sigact, NULL); // signal 13
    
    // switch off signal 13 (broken pipe)
    // instead handle the return value of the write or send function
    signal(SIGPIPE, SIG_IGN);

    memset(s,' ',512);
    init_fifo();
    init_displayarray();
    init_udppipe();
    serial_init();
    
    while(1)
    {
        // wait for request from browser via php
        int len = read_udppipe((unsigned char *)pip,511);
        if(len > 0)
        {
            // browser asks for new data, check if we have data in the fifo
            // only take the latest data from the fifo
            while(1)
            {
                int len = read_fifo((unsigned char *)(s+1),511);
                if(len == 0)
                    break;
            }
            
            s[0] = 0x35;   // marker: Downconverter Display data follows
            //printf("%16.16s\n",s+1);
            write_udppipe((unsigned char *)s,XSIZE*YSIZE+1);
        }
    }
        
    return 0;
}
