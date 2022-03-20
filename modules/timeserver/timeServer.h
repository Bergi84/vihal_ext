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
    void (*cb)(void*, THwRtc::time_t);
    void* objP;
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
  bool create(uint8_t &aTimerID, void (*aCb)(void*, THwRtc::time_t), void* aObjP, bool aRepeat);
  bool remove(uint8_t aTimerID);
  bool start(uint8_t aTimerID, uint32_t aMsecTimeout);
  bool stop(uint8_t aTimerID);

  // additional functions which are not implemented but maybe necessary in future:
  // bool setTime(THwRtc::time_t aTime);    // time should set trough timer server to compensate occurring time shifts
  // bool getTime(THwRtc::time_t &aTime);

private:
  // helper functions for internal use only

  void queue(uint8_t aTimerID);
  void unqueue(uint8_t aTimerID);

  // should only called inside critical sections
  void updateWakupTimer();

  // get time and update all left counter
  // should only called inside critical sections
  void serviceLeftTime();

public:
  // must called inside timer wakeup irq
  void serviceQueue();
};



#endif /* TIMESERVER_H_ */
