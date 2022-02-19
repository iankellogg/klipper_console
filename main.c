
#include <klipper.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>


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
    initKlipper();

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
        // if (input[0]=='!')
        // {
        //   write(hFD,&input[1],input_pos-1);
        //   input_pos=0;
        // }
        // else
        // {
          float pos = atof(input);
          if (pos<1 || pos>20)
          continue;
          char buffer[80];
          printf("Going to %f pos\r\n",pos);
          // if (((7200*2)-curPos)>10800)
          // {

          // }
          int p = (pos-1.0)*(7200.0/20.0)+7200.0;
          move(AXIS_TRAY,p,0,0);
          waitForMove();
        // }
      }

    }
    return 0;
}