/*
 * traces.cpp
 *
 *  Created on: 08.04.2022
 *      Author: Bergi
 */

#include "traces.h"

TTrace gTrace;

constexpr const char TTrace::strCpu1[];
constexpr const char TTrace::strCpu2[];

extern "C"
{
  void cTraceCpu2(const char* format, ...)
  {
    va_list argptr;
    va_start(argptr, format);

    gTrace.vprintf(TTrace::TA_CPU2, format, argptr);
    va_end(argptr);
  }

  void cTraceBufferCpu2(const void *pBuffer, uint32_t u32Length, const char *strFormat, ...)
  {
    va_list vaArgs;
    uint32_t u32Index;
    va_start(vaArgs, strFormat);
    gTrace.vprintf(TTrace::TA_CPU2, strFormat, vaArgs);
    va_end(vaArgs);
    for (u32Index = 0; u32Index < u32Length; u32Index ++)
    {
      gTrace.traceCpu2(" %02X", ((const uint8_t *) pBuffer)[u32Index]);
    }
  }
}

bool TTrace::init(THwUart* aUart, TLowPowerManger* aLpm, TSequencer* aSeq)
{
  if(aUart == 0)
    uart = &conuart;
  else
    uart = aUart;

  lpm = aLpm;
  if(lpm != 0)
  {
    lpm->registerApp(lpmId);
  }

  seq = aSeq;
  if(aSeq != 0)
  {
    aSeq->addTask(seqId, this, (void (TCbClass::*)(void)) &TTrace::service);
  }

  bufWInd = 0;
  bufRInd = 0;

  lastAktiv = TA_NONE;

  return true;
}

void TTrace::traceCpu1(const char* format, ...)
{
  va_list argptr;
  va_start(argptr, format);

  this->vprintf(TA_CPU1, format, argptr);
  va_end(argptr);
}

void TTrace::traceCpu2(const char* format, ...)
{
  va_list argptr;
  va_start(argptr, format);

  this->vprintf(TA_CPU2, format, argptr);
  va_end(argptr);
}


void TTrace::vprintf(aktiv_t aAktiv, const char* aFormat, va_list aVa)
{
  char locBuf[128];
  uint32_t pos = mp_vsnprintf(&locBuf[0], 128, aFormat, aVa);

  printBuf(aAktiv, locBuf, pos);
}

void TTrace::printBuf(aktiv_t aAktiv, const char* aBuf, uint32_t aLen)
{
  TCriticalSection cSec(true);

   if(lastAktiv != aAktiv)
   {
     lastAktiv = aAktiv;
     const char* cpuString;

     switch(aAktiv)
     {
     case TA_CPU1:
       cpuString = strCpu1;

       break;
     case TA_CPU2:
       cpuString = strCpu2;
       break;
     }

     for(int i = 0; cpuString[i] != '\0'; i++)
     {
       buf[bufWInd] = cpuString[i];
       bufWInd = (bufWInd < bufLength-1) ? (bufWInd + 1) : 0;
     }
     intend = false;
   }

   for(int i = 0; i < aLen; i++)
   {
     if(intend && (aBuf[i] != '\r'))
     {
       intend = false;

       for(int j = 0; j < strCpuIndent; j++)
       {
         buf[bufWInd] = ' ';
         bufWInd = (bufWInd < bufLength-1) ? (bufWInd + 1) : 0;
       }
     }

     buf[bufWInd] = aBuf[i];
     bufWInd = (bufWInd < bufLength-1) ? (bufWInd + 1) : 0;

     if(aBuf[i] == '\n')
     {
       intend = true;
     }
   }

   cSec.leave();

   if(seq != 0)
   {
     seq->queueTask(seqId);
   }
}

void TTrace::service()
{
  while(bufWInd != bufRInd)
  {
    if(lpm != 0)
    {
      lpm->enableLpMode(lpmId, TLowPowerManger::LPM_Run);
    }

    if(uart->TrySendChar(buf[bufRInd]))
    {
      if(bufRInd == bufLength-1)
      {
        bufRInd = 0;
      }
      else
      {
        bufRInd++;
      }
    }
    else
    {
      if(seq != 0)
      {
        seq->queueTask(seqId);
      }
      return;
    }
  }

  if(lpm != 0 && uart->SendFinished())
  {
    lpm->disableLpMode(lpmId, TLowPowerManger::LPM_Run);
  }
  else
  {
    if(seq != 0)
    {
      seq->queueTask(seqId);
    }
  }
}

void TTrace::flush()
{
  while(bufWInd != bufRInd)
  {
    if(lpm != 0)
    {
      lpm->enableLpMode(lpmId, TLowPowerManger::LPM_Run);
    }

    if(uart->TrySendChar(buf[bufRInd]))
    {
      if(bufRInd == bufLength-1)
      {
        bufRInd = 0;
      }
      else
      {
        bufRInd++;
      }
    }
  }
}
