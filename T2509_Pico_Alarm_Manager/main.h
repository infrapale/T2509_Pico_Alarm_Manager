#ifndef __MAIN_H__
#define __MAIN_H__

#include "Arduino.h"
#define APP_NAME "T2509_Pico_Alarm_Manager"
#define RFM_SEND_INTERVAL 2000
#define DEBUG_PRINT 

#define MY_MODULE_TAG       ('A')
#define MY_MODULE_ADDR      ('1')

typedef enum
{
    STATUS_UNDEFINED = 0,
    STATUS_OK_FOR_ME,
    STATUS_NOT_FOR_ME,
    STATUS_UNKNOWN_COMMAND,
    STATUS_CORRECT_FRAME,
    STATUS_INCORRECT_FRAME,
} msg_status_et;

typedef enum
{
    HOME_AWAY = 0,
    HOME_ALL_IN,
    HOME_IN_LILLA,
    HOME_SLEEPING,
    HOME_ALL_OFF,
    HOME_NBR_OF
} home_status_et;

typedef enum
{
    BELL_OFF  = 0,
    BELL_1,
    BELL_2,
    BELL_3,
    BELL_4,
    BELL_NBR_OF
} bell_level_et;

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} time_st;


typedef struct
{
  uint16_t state;
  home_status_et   home_status;
  time_st time;
} main_ctrl_st;

#endif