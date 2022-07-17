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
  serviceLeftTime();
  rtcRegs->enableWakeupIRQ();

  return true;
}

bool TTimerServer::create(uint8_t &aTimerID, TCbClass* aPObj, void (TCbClass::*aPMFunc)(THwRtc::time_t time), bool aRepeat)
{
  for(int i = 0; i < TS_MAXTIMERS; i++)
  {
    if(timerRec[i].state == TS_Free)
    {
      timerRec[i].state = TS_Created;
      timerRec[i].repeat = aRepeat;
      timerRec[i].pObj = aPObj;
      timerRec[i].pMFunc = aPMFunc;
      aTimerID = i;
      return true;
    }
  }

  aTimerID = invalidTimerId;
  return false;
}

bool TTimerServer::create(uint8_t &aTimerID, void (*aPFunc)(THwRtc::time_t time), bool aRepeat)
{
  for(int i = 0; i < TS_MAXTIMERS; i++)
  {
    if(timerRec[i].state == TS_Free)
    {
      timerRec[i].state = TS_Created;
      timerRec[i].repeat = aRepeat;
      timerRec[i].pObj = 0;
      timerRec[i].pFunc = aPFunc;
      aTimerID = i;
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

  TCriticalSection cSec(true);
  serviceLeftTime();
  cSec.leave();

  timerRec[aTimerID].msecTimerLeft = aMsecTimeout;
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


void TTimerServer::queue(uint8_t aTimerID, bool updateTimer)
{
  TCriticalSection cSec(true);

  if(aktivTimer == invalidTimerId)
  {
    // no timer Aktiv
    aktivTimer = aTimerID;
    timerRec[aTimerID].nextTimerID = invalidTimerId;
    timerRec[aTimerID].prevTimerID = invalidTimerId;
    if(updateTimer)
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
      if(updateTimer)
        updateWakupTimer();
    }
    else
    {
      // insert timer between prevID and nextID
      uint8_t nextID = timerRec[aktivTimer].nextTimerID;
      uint8_t prevID = aktivTimer;
      while(nextID != invalidTimerId && timerRec[nextID].msecTimerLeft < timerRec[aTimerID].msecTimerLeft)
      {
        prevID = nextID;
        nextID = timerRec[nextID].nextTimerID;
      }
      timerRec[aTimerID].nextTimerID = nextID;
      timerRec[aTimerID].prevTimerID = prevID;
      if(nextID != invalidTimerId)
      {
        timerRec[nextID].prevTimerID = aTimerID;
      }
      timerRec[prevID].nextTimerID = aTimerID;
    }
  }
  timerRec[aTimerID].state = TS_Running;

  cSec.leave();
}


void TTimerServer::unqueue(uint8_t aTimerID, bool updateTimer)
{
  TCriticalSection cSec(true);

  if(timerRec[aTimerID].state == TS_Running)
  {
    if(aktivTimer == aTimerID)
    {
      aktivTimer = timerRec[aTimerID].nextTimerID;
      timerRec[aktivTimer].prevTimerID = invalidTimerId;
      if(updateTimer)
      {
        if(aktivTimer == invalidTimerId)
        {
          rtcRegs->stopWakeupTimer();
        }
        else
        {
          updateWakupTimer();
        }
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
    timerRec[aTimerID].state = TS_Created;
  }

  cSec.leave();
}


void TTimerServer::updateWakupTimer()
{
  int32_t nextWakeUp = timerRec[aktivTimer].msecTimerLeft;
  if(nextWakeUp > maxMsecSleep)
  {
    rtcRegs->setWakeupTimer(maxMsecSleep);
  }
  else
  {
    if(nextWakeUp < 1)
      rtcRegs->setWakeupIrq();
    else
      rtcRegs->setWakeupTimer(nextWakeUp);
  }
}


void TTimerServer::serviceLeftTime()
{
  rtcRegs->getTime(lastTime);
  uint16_t timeMsec = lastTime.sec*1000 + lastTime.msec;

  uint16_t leftMsec;
  if(lastMsecVal <= timeMsec)
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

