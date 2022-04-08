#include "lowPowerManager.h"

bool TLowPowerManger::init(TCbClass* pObj, void (TCbClass::*pMFunc)(lpm_t aLpm))
{
  lpRunAktiv = false;
  setPowerMode.pObj = pObj;
  setPowerMode.pMFunc = pMFunc;
  for(int i = 0; i < arrayLen; i++)
  {
    usedID[i] = 0;
  }
  return true;
}

bool TLowPowerManger::init(void (*pFunc)(lpm_t aLpm))
{
  lpRunAktiv = false;
  setPowerMode.pObj = 0;
  setPowerMode.pFunc = pFunc;
  for(int i = 0; i < arrayLen; i++)
  {
    usedID[i] = 0;
  }
  return true;
}

bool TLowPowerManger::registerApp(uint32_t &aId)
{
  for(int i = 0; i < arrayLen; i++)
  {
    for(int j = 0; j < 32; j++)
    {
      uint32_t mask = (1U << j);
      if((usedID[i] & mask) == 0)
      {
        usedID[i] |= mask;
        lowPowerDis[i] &= ~mask;
        sleepDis[i] &= ~mask;
        deepSleepDis[i] &= ~mask;
        offDis[i] &= ~mask;
        run[i] |= mask;

        aId = i*32 + j;
        return true;
      }
    }
  }
  return false;
}

bool TLowPowerManger::unregisterApp(uint32_t aId)
{
  uint32_t i = aId/32;
  uint32_t j = aId - i*32;

  usedID[i] &= ~(1U << j);

  return true;
}

bool TLowPowerManger::enableLpMode(uint32_t aId, lpm_t aLpm)
{
  uint32_t i = aId/32;
  uint32_t j = aId - i*32;

  switch(aLpm)
  {
  case LPM_Run:
    run[i] |= (1U << j);
    break;

  case LPM_lpRun:
    lowPowerDis[i] &= ~(1U << j);
    break;

  case LPM_sleep:
    sleepDis[i] &= ~(1U << j);
    break;

  case LPM_deepSleep:
    deepSleepDis[i] &= ~(1U << j);
    break;

  case LPM_off:
    offDis[i] &= ~(1U << j);
    break;
  }

  return true;
}

bool TLowPowerManger::disableLpMode(uint32_t aId, lpm_t aLpm)
{
  uint32_t i = aId/32;
  uint32_t j = aId - i*32;

  switch(aLpm)
  {
  case LPM_Run:
    run[i] &= ~(1U << j);
    break;

  case LPM_lpRun:
    lowPowerDis[i] |= (1U << j);
    if(lpRunAktiv)
    {
      if(setPowerMode.pObj == 0)
      {
        setPowerMode.pFunc(LPM_Run);
      }
      else
      {
        (setPowerMode.pObj->*(setPowerMode.pMFunc))(LPM_Run);
      }
    }
    break;

  case LPM_sleep:
    sleepDis[i] |= (1U << j);
    break;

  case LPM_deepSleep:
    deepSleepDis[i] |= (1U << j);
    break;

  case LPM_off:
    offDis[i] |= (1U << j);
    break;
  }

  return true;
}

bool TLowPowerManger::enterLowPowerMode()
{

  for(int i = 0; i < arrayLen; i++)
  {
    if(usedID[i] & run[i])
    {
      return true;
    }
  }


  bool lpmDis = false;

  // check if power off is possible
  for(int i = 0; i < arrayLen; i++)
  {
    if(usedID[i] & offDis[i])
    {
      lpmDis = true;
      break;
    }
  }

  if(!lpmDis)
  {
    if(setPowerMode.pObj == 0)
    {
      setPowerMode.pFunc(LPM_off);
    }
    else
    {
      (setPowerMode.pObj->*(setPowerMode.pMFunc))(LPM_off);
    }
    return true;
  }

  lpmDis = false;

  // check if deep sleep is possible
  for(int i = 0; i < arrayLen; i++)
  {
    if(usedID[i] & deepSleepDis[i])
    {
      lpmDis = true;
      break;
    }
  }

  if(!lpmDis)
  {
    if(setPowerMode.pObj == 0)
    {
      setPowerMode.pFunc(LPM_deepSleep);
    }
    else
    {
      (setPowerMode.pObj->*(setPowerMode.pMFunc))(LPM_deepSleep);
    }
    return true;
  }

  lpmDis = false;

  // check if sleep is possible
  for(int i = 0; i < arrayLen; i++)
  {
    if(usedID[i] & sleepDis[i])
    {
      lpmDis = true;
      break;
    }
  }

  if(!lpmDis)
  {
    if(setPowerMode.pObj == 0)
    {
      setPowerMode.pFunc(LPM_sleep);
    }
    else
    {
      (setPowerMode.pObj->*(setPowerMode.pMFunc))(LPM_sleep);
    }
    return true;
  }

  lpmDis = false;

  // check if low power run is possible
  for(int i = 0; i < arrayLen; i++)
  {
    if(usedID[i] & lowPowerDis[i])
    {
      lpmDis = true;
      break;
    }
  }

  if(!lpmDis)
  {
    if(setPowerMode.pObj == 0)
    {
      setPowerMode.pFunc(LPM_lpRun);
    }
    else
    {
      (setPowerMode.pObj->*(setPowerMode.pMFunc))(LPM_lpRun);
    }
    return true;
  }

  // no low power mode was possible
  return false;
}
