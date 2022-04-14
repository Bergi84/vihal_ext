/*
 * timeServer.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef TIMESERVER_H_
#define TIMESERVER_H_

#include "platform.h"
#include "hwrtc.h"

#ifndef TS_MAXTIMERS
  #define TS_MAXTIMERS 8
#endif

class TTimerServer
{
public:
  THwRtc* rtcRegs;

  uint16_t lastMsecVal;
  THwRtc::time_t lastTime;

  uint16_t maxMsecSleep;

  typedef enum {
    TS_Free,
    TS_Created,
    TS_Running
  } timerStatus_t;

  static constexpr uint8_t invalidTimerId = -1;

  struct {
    timerStatus_t state;
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)(THwRtc::time_t time);
      void (*pFunc)(THwRtc::time_t time);
    };
    bool repeat;
    int32_t msecTimerInit;
    int32_t msecTimerLeft;
    uint8_t nextTimerID;
    uint8_t prevTimerID;
  } timerRec[TS_MAXTIMERS];

  uint8_t aktivTimer;

  bool init(THwRtc* aRtc);

  // create a new  timer, if timer is expired the time server calls the aCb function
  // with aObjP as first argument, aCb should as short as possible because it is called
  // in interrupt context inside critical section
  bool create(uint8_t &aTimerID, TCbClass* aPObj, void (TCbClass::*aPMFunc)(THwRtc::time_t time), bool aRepeat);
  bool create(uint8_t &aTimerID, void (*aPFunc)(THwRtc::time_t time), bool aRepeat);
  bool remove(uint8_t aTimerID);
  bool start(uint8_t aTimerID, uint32_t aMsecTimeout);
  bool stop(uint8_t aTimerID);

  // additional functions which are not implemented but maybe necessary in future:
  // bool setTime(THwRtc::time_t aTime);    // time should set trough timer server to compensate occurring time shifts
  // bool getTime(THwRtc::time_t &aTime);

private:
  // helper functions for internal use only

  void queue(uint8_t aTimerID, bool updateTimer = true);
  void unqueue(uint8_t aTimerID, bool updateTimer = true);

  // should only called inside critical sections
  void updateWakupTimer();

  // get time and update all left counter
  // should only called inside critical sections
  void serviceLeftTime();

public:
  // must called inside timer wakeup irq
  inline void irqHandler()
  {
    TCriticalSection cSec(true);

    serviceLeftTime();

    uint8_t tmpId = aktivTimer;
    while(tmpId != invalidTimerId && timerRec[tmpId].msecTimerLeft <= 0)
    {
      while(tmpId != invalidTimerId && timerRec[tmpId].msecTimerLeft <= 0)
      {
        unqueue(tmpId, false);
        if(timerRec[tmpId].pObj != 0)
        {
          if(timerRec[tmpId].pMFunc != 0)
          {
            ((timerRec[tmpId].pObj)->*(timerRec[tmpId].pMFunc))(lastTime);
          }
        }
        else
        {
          if(timerRec[tmpId].pFunc != 0)
          {
            timerRec[tmpId].pFunc(lastTime);
          }
        }

        if(timerRec[tmpId].repeat)
        {
          timerRec[tmpId].msecTimerLeft += timerRec[tmpId].msecTimerInit;
          queue(tmpId, false);
        }

        tmpId = aktivTimer;
      }
      serviceLeftTime();
    }
    if(aktivTimer == invalidTimerId)
    {
      rtcRegs->stopWakeupTimer();
    }
    else
    {
      updateWakupTimer();
    }

    rtcRegs->clearWakeupIRQ();

    cSec.leave();
  }
};



#endif /* TIMESERVER_H_ */
