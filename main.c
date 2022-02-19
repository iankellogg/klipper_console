

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

#include <queue.h>



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

void klipper_thread(void *param)
{
  char line[80];
  while (1)
  {
    //
      readMaxTime(hFD,line,80,10000);
  }
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
      static const char statusReady[] = "// Klipper state: Ready";
      ret = strncmp(statusReady,line,strlen(statusReady));
        if (ret==0)
        {
          printf("Klipper  Ready\r\nStatus: %.*s",ret,line);
        } else {
          printf("Klipper Not Ready\r\nStatus: %.*s",ret,line);
        //  return 1;
        }
    }
    else
    {
        printf("Failed to respond to status\r\n");
        return 1;
    }
   //int hstdin = open(stdin,O_RDONLY);
    char input[80];
    int input_pos=0;
    int curPos = 0;
    while (running)
    {
      // read from stdin
      input_pos = readMaxTime(0,input,80,1000);
      if (input[input_pos-1]=='\n')
      {
        //printf("debug: %s\r\n",input);
        if (strncmp(input,"!zero",5)==0)
        {
          char z[] = "MANUAL_STEPPER STEPPER=tray ENABLE=1 SET_POSITION=7200\r\n";
          write(hFD,z,strlen(z));
        } else
        if (input[0]=='!')
        {
          write(hFD,&input[1],input_pos-1);
          input_pos=0;
        }
        else
        {
          float pos = atof(input);
          if (pos<1 || pos>20)
          continue;
          char buffer[80];
          printf("Going to %f pos\r\n",pos);
          // if (((7200*2)-curPos)>10800)
          // {

          // }
          int p = (pos-1.0)*(7200.0/20.0)+7200.0;
          curPos = p;
          int c = snprintf(buffer,80,"MANUAL_STEPPER STEPPER=tray ENABLE=1 MOVE=%d\n",p);
          printf("debug: %s\r\n",buffer);
          write(hFD,buffer,c);
        }
      }
      ret = readMaxTime(hFD,line,80,1000);
      if (ret>0)
      {
          printf("%.*s",ret,line);
      }

    }
    return 0;
}