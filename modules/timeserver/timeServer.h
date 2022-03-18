/*
 * ts.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef TIMESERVER_H_
#define TIMESERVER_H_

#include "platform.h"
#include "hwrtc.h"

template <uint8_t maxTimers>
class TTimerServer
{
  THwRtc* rtc;

  uint32_t minCnt;
  uint16_t lastMsecVal;

  uint16_t maxMsecSleep;

  typedef enum {
    TS_Free,
    TS_Created,
    TS_Running
  } timerStatus_t;

  struct {
    void (*cb)(void*, THwRtc::time_t);
    uint32_t msecTimerInit;
    uint32_t msecTimerLeft;
    uint8_t nextTimerID;
    uint8_t prevTimerID;
  } timerRec[maxTimers];

  uint8_t aktivTimer;

  bool init();
  bool create(uint8_t &aTimerID, void (*aCb)(void*, THwRtc::time_t), bool repeat);
  bool remove(uint8_t &aTimerID);
  bool start(uint8_t &aTimerID, uint32_t aMsecTimeout);
  bool stop(uint8_t &aTimerID);
};



#endif /* TIMESERVER_H_ */
