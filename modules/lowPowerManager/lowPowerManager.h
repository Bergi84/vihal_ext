/*
 * lowPowerManager.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef LOWPOWERMANAGER_H_
#define LOWPOWERMANAGER_H_

#include "platform.h"

#ifndef LPM_MAXIDS
  #define LPM_MAXIDS 32
#endif

class TLowPowerManger : public TCbClass
{
public:
  static constexpr uint32_t arrayLen = ((LPM_MAXIDS - 1)/32 + 1);
  uint32_t usedID[arrayLen];
  uint32_t run[arrayLen];
  uint32_t lowPowerDis[arrayLen];
  uint32_t sleepDis[arrayLen];
  uint32_t deepSleepDis[arrayLen];
  uint32_t offDis[arrayLen];

  typedef enum
  {
    LPM_Run,
    LPM_lpRun,
    LPM_sleep,
    LPM_deepSleep,
    LPM_off
  } lpm_t;

  bool lpRunAktiv;

  struct {
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)(lpm_t aLpm);
      void (*pFunc)(lpm_t aLpm);
    };
  } setPowerMode;

  // init function must get a function pointer for setting low power mode
  // Normally this power set function is implemented in user application because
  // the used power mode configurations are highly application depended
  bool init(TCbClass* pObj,  void (TCbClass::*pMFunc)(lpm_t aLpm));
  bool init(void (*pFunc)(lpm_t aLpm));

  // after registration of a new application all low power modes
  // are enabled and application is marked as run
  bool registerApp(uint32_t &aId);
  bool unregisterApp(uint32_t aId);

  bool enableLpMode(uint32_t aId, lpm_t aLpm);
  bool disableLpMode(uint32_t aId, lpm_t aLpm);

  // enter deepest possible low power mode
  bool enterLowPowerMode();
};

#endif /* LOWPOWERMANAGER_H_ */
