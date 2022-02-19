#pragma once


typedef enum
{
  AXIS_TRAY=0,
  AXIS_LIFT=1,
  AXIS_CLEAN=2,
  AXIS_SOLV1=3,
  AXIS_SOLV2=4,
  AXIS_SOLV3=5,
} axis_t;



void move(axis_t axis, int steps, int speed, int accel);
void waitForMove();

int initKlipper();