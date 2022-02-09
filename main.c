

#include <stdlib.h>
#include <stdio.h> 
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <string.h>
#include <errno.h> 
#include <sys/select.h>





static const char klipperPath[] = "/tmp/printer";
int hFD; 

int readMaxTime(int fd, char *buf, int len, int timeout_uSec)
{
  fd_set set;
  struct timeval timeout;
  int rv;
    FD_ZERO(&set); /* clear the set */
  FD_SET(fd, &set); /* add our file descriptor to the set */

  timeout.tv_sec = 0;
  timeout.tv_usec = timeout_uSec;

  rv = select(fd + 1, &set, NULL, NULL, &timeout);
  if(rv == -1)
    perror("select"); /* an error accured */
  else if(rv == 0)
    return 0;
  else
    return read( fd, buf, len ); /* there was data to read */
}

int main(int argc, void *args)
{
    bool running=true;
    char line[80];
    hFD = open(klipperPath, O_RDWR ); 
    if (hFD==0)
    {
        printf("Failed to open %s, klipper likely not running\r\n",klipperPath);
        return 1;
    }

    // send status to check that it's operating
    write(hFD,"status\n",7);
    int ret = readMaxTime(hFD,line,30,10000);
    if (ret>0)
    {
        printf("Status: %.*s",ret,line);
    }
    else
    {
        printf("Failed to respond to status\r\n");
        return 1;
    }
   //int hstdin = open(stdin,O_RDONLY);
char input[80];
int input_pos=0;
    while (running)
    {
      // read from stdin
      input_pos = readMaxTime(0,input,80,1000);
      if (input[input_pos-1]=='\n')
      {
        write(hFD,input,input_pos);
        input_pos=0;
      }
      ret = readMaxTime(hFD,line,80,1000);
      if (ret>0)
      {
          printf("%.*s",ret,line);
      }

    }
    return 0;
}