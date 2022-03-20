/*
 * ts.cpp
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */
#include "timeServer.h"

bool TTimerServer::init(THwRtc* aRtc)
{
  rtcRegs = aRtc;

  aktivTimer = invalidTimerId;
  for(int i; i < TS_MAXTIMERS; i++)
  {
    timerRec[i].state = TS_Free;
  }

  // set minimum wake up interval, TS is designed to use millisecond and second counter from RTC
  // minutes are counted internal with up to 2^32 => maximum sleep interval is 1 minute minus safety margin
  if(rtcRegs->wakeUpMaxMsec > 55000U)
  {
    maxMsecSleep = 55000U;
  }
  else
  {
    maxMsecSleep = rtcRegs->wakeUpMaxMsec;
  }
  rtcRegs->enableWakeupIRQ();

  return true;
}

bool TTimerServer::create(uint8_t &aTimerID, void (*aCb)(void*, THwRtc::time_t), void* aObjP, bool aRepeat)
{
  for(int i; i < TS_MAXTIMERS; i++)
  {
    if(timerRec[i].state == TS_Free)
    {
      timerRec[i].state = TS_Created;
      timerRec[i].repeat = aRepeat;
      timerRec[i].cb = aCb;
      timerRec[i].objP = aObjP;
      aTimerID = 0;
      return true;
    }
  }

  aTimerID = invalidTimerId;
  return false;
}


bool TTimerServer::remove(uint8_t aTimerID)
{
  if(aTimerID >= TS_MAXTIMERS)
  {
    return false;
  }

  unqueue(aTimerID);

  timerRec[aTimerID].state = TS_Free;

  return true;
}


bool TTimerServer::start(uint8_t aTimerID, uint32_t aMsecTimeout)
{
  if(aTimerID >= TS_MAXTIMERS)
  {
    return false;
  }

  unqueue(aTimerID);

  timerRec[aTimerID].msecTimerInit = aMsecTimeout;

  if(timerRec[aTimerID].state == TS_Created)
  {
    queue(aTimerID);
  }

  return true;
}


bool TTimerServer::stop(uint8_t aTimerID)
{
  if(aTimerID >= TS_MAXTIMERS)
  {
    return false;
  }

  unqueue(aTimerID);

  return true;
}


void TTimerServer::queue(uint8_t aTimerID)
{
  // todo: enter critical section

  serviceLeftTime();
  timerRec[aTimerID].msecTimerLeft = timerRec[aTimerID].msecTimerInit;

  if(aktivTimer == invalidTimerId)
  {
    // no timer Aktiv
    aktivTimer = aTimerID;
    timerRec[aTimerID].nextTimerID = invalidTimerId;
    timerRec[aTimerID].prevTimerID = invalidTimerId;
    updateWakupTimer();
  }
  else
  {
    if(timerRec[aktivTimer].msecTimerLeft > timerRec[aTimerID].msecTimerLeft)
    {
      // insert timer as aktiv timer
      timerRec[aTimerID].nextTimerID = aktivTimer;
      timerRec[aTimerID].prevTimerID = invalidTimerId;
      timerRec[aktivTimer].prevTimerID = aTimerID;
      aktivTimer = aTimerID;
      updateWakupTimer();
    }
    else
    {
      // insert timer between prevID and nextID
      uint8_t prevID = aktivTimer;
      while(timerRec[prevID].nextTimerID != invalidTimerId && timerRec[prevID].msecTimerLeft < timerRec[aTimerID].msecTimerLeft)
      {
        prevID = timerRec[prevID].nextTimerID;
      }
      uint8_t nextID = timerRec[prevID].nextTimerID;

      timerRec[aTimerID].nextTimerID = nextID;
      timerRec[aTimerID].prevTimerID = prevID;
      if(nextID != invalidTimerId)
      {
        timerRec[nextID].prevTimerID = aTimerID;
      }
      timerRec[prevID].nextTimerID = aTimerID;
    }
  }
  timerRec[aTimerID].state == TS_Running;

  // todo: leave critical section
}


void TTimerServer::unqueue(uint8_t aTimerID)
{
  // todo: enter critical section

  if(timerRec[aTimerID].state == TS_Running)
  {
    if(aktivTimer == aTimerID)
    {
      aktivTimer = timerRec[aTimerID].nextTimerID;
      timerRec[aktivTimer].prevTimerID = invalidTimerId;
      if(aktivTimer == invalidTimerId)
      {
        rtcRegs->stopWakeupTimer();
      }
    }
    else
    {
      uint8_t prevID = timerRec[aTimerID].prevTimerID;
      timerRec[prevID].nextTimerID = timerRec[aTimerID].nextTimerID;

      if(timerRec[aTimerID].nextTimerID != invalidTimerId)
      {
        uint8_t nextID = timerRec[aTimerID].nextTimerID;
        timerRec[nextID].prevTimerID = timerRec[aTimerID].prevTimerID;
      }
    }
    timerRec[aTimerID].state == TS_Created;
  }

  // todo: leave critical section
}


void TTimerServer::updateWakupTimer()
{
  uint16_t nextWakeUp = timerRec[aktivTimer].msecTimerLeft;
  if(nextWakeUp > maxMsecSleep)
  {
    rtcRegs->setWakeupTimer(maxMsecSleep);
  }
  else
  {
    rtcRegs->setWakeupTimer(nextWakeUp);
  }
}


void TTimerServer::serviceLeftTime()
{
  rtcRegs->getTime(lastTime);
  uint16_t timeMsec = lastTime.sec*1000 + lastTime.msec;

  uint16_t leftMsec;
  if(lastMsecVal < timeMsec)
  {
    leftMsec = timeMsec - lastMsecVal;
  }
  else
  {
    leftMsec = timeMsec + 60000 - lastMsecVal;
  }
  lastMsecVal = timeMsec;

  uint8_t tmpId = aktivTimer;

  while(tmpId != invalidTimerId)
  {
    timerRec[tmpId].msecTimerLeft -= leftMsec;

    tmpId = timerRec[tmpId].nextTimerID;
  }
}


void TTimerServer::serviceQueue()
{
  // todo: enter critical section

  serviceLeftTime();

  uint8_t tmpId = aktivTimer;
  while(tmpId != invalidTimerId && timerRec[tmpId].msecTimerLeft <= 0)
  {
    unqueue(tmpId);
    timerRec[tmpId].cb(timerRec[tmpId].objP, lastTime);
    if(timerRec[tmpId].repeat)
    {
      timerRec[tmpId].msecTimerLeft += timerRec[tmpId].msecTimerInit;
      queue(tmpId);
    }

    tmpId = timerRec[tmpId].nextTimerID;
  }
  // todo: leave critical section
}
