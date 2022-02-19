
#include "klipper.h"
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
#include <semaphore.h>

#include <queue.h>


// create the semaphore with 0 initial value, fill command
// send command
// wait on semaphore
// process response
// most commands only respond to OK
typedef struct 
{
  char command[80];
  char response[80];
	sem_t mutex;
  uint32_t maxWait_uSec;
} cmd_t;



static const char klipperPath[] = "/tmp/printer";
int hFD; 
void *buffer[3];
queue_t queue = QUEUE_INITIALIZER(buffer);
pthread_t klipperThread;


char *axisToString(axis_t axis)
{
    switch (axis)
    {
      case AXIS_TRAY:
        return "tray";
      case AXIS_LIFT:
        return "lift";
      case AXIS_CLEAN:
        return "clean";
      case AXIS_SOLV1:
        return "SOLVENT1";
      case AXIS_SOLV2:
        return "SOLVENT2";
      case AXIS_SOLV3:
        return "SOLVENT3";
      default:
        return "noaxis";
    }
}
/*
*   @description Calls the move command
*   @params   axis, enum for which axis
*             steps, the number of microsteps 
*             speed, the speed to use, 0 for default
*             accel, the acceleration to use, 0 for default
*/
void move(axis_t axis, int steps, int speed, int accel)
{
  cmd_t cmd;
   sem_init(&cmd.mutex, 0,0);
  int count = sprintf(cmd.command,"MANUAL_STEPPER STEPPER=%s MOVE=%d ",axisToString(axis),steps);
  if (speed!=0)
  {
      count += sprintf(&cmd.command[count],"SPEED=%d ",speed);
  }
  if (accel!=0)
  {
      count += sprintf(&cmd.command[count],"ACCEL=%d ",accel);
  }
  sprintf(&cmd.command[count]," SYNC=0\r\n");
  cmd.maxWait_uSec = 100*1000; // 100 milli seconds
   queue_enqueue(&queue,&cmd);
   sem_wait(&cmd.mutex);
   
   sem_destroy(&cmd.mutex);
   printf("End of move\r\n");
}

void waitForMove()
{
  cmd_t cmd;
   sem_init(&cmd.mutex, 0,0);
  sprintf(cmd.command,"m400\r\n");
  cmd.maxWait_uSec = 30*1000*1000; // 30 seconds
   queue_enqueue(&queue,&cmd);
   sem_wait(&cmd.mutex);
   
   sem_destroy(&cmd.mutex);
   printf("End of Wait\r\n");
}

extern int readMaxTime(int fd, char *buf, int len, int timeout_uSec);

void *klipper_thread(void *param)
{
  char line[80];
  int count=0;
  while (1)
  {
    cmd_t *cmd =  queue_dequeue(&queue);
    int length = strlen(cmd->command);
    length = (length>80)?80:length;
    printf("%s",cmd->command);
    write(hFD,cmd->command,length);
      count = readMaxTime(hFD,&cmd->response,80,cmd->maxWait_uSec);
    int bufferCount=1;
     while (bufferCount!=0)
    {
      // wait another 10 mS
      bufferCount = readMaxTime(hFD,&cmd->response[count],80,100000);
      // check for more data
      count += bufferCount;
    }
    printf("%s",cmd->response);
    // done with transaction, free semaphore
    sem_post(&cmd->mutex);
  }
}


int initKlipper()
{
    bool running=true;
    char line[80];
    hFD = open(klipperPath, O_RDWR ); 
    if (hFD==0)
    {
        printf("Failed to open %s, klipper likely not running\r\n",klipperPath);
        return 1;
    }
    int ret=1;
    // flush input
    do {
        
    ret = readMaxTime(hFD,line,30,10000);
    } while(ret!=0);

    // send status to check that it's operating
    write(hFD,"status\n",7);
     ret = readMaxTime(hFD,line,30,10000);
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

	pthread_create(&klipperThread, NULL, klipper_thread, NULL);
    return 0;
}