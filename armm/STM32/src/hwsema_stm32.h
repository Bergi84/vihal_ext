/*
 * hwrtc_stm32.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWSEMA_STM32_H_
#define HWSEMA_STM32_H_

#define HWSEMA_PRE_ONLY
#include "hwsema.h"

class THwSema_stm32 : public THwSema_pre
{
public:
  constexpr uint8_t coreID = HSEM_CR_COREID_CURRENT;
  constexpr uint8_t invalidProcID = -1;

  HSEM_TypeDef* regs;

  bool init();

  // returns true if lock is successful
  bool lock(uint8_t semaID, uint8_t procID = invalidProcID)
  {
    if(procID == invalidProcID)
    {
      // one step lock without proc id
      return regs->RLR[semaID] != (HSEM_RLR_LOCK | coreID);
    }
    else
    {
      // two step lock with proc id
      regs->R[semaID] = (HSEM_R_LOCK | coreID | procID);
      return regs->R[semaID] != (HSEM_R_LOCK | coreID | procID);
    }
  }

  bool release(uint8_t semaID, uint8_t procID = invalidProcID)
  {
    if(procID == invalidProcID)
    {
      regs->R[semaID] = coreID;
    }
    else
    {
      regs->R[semaID] = coreID | procID;
    }
    return regs->R[semaID] == 0;
  }

  inline bool status(uint8_t semaID, bool &lock)
  {
    lock = regs->R[semaID] & HSEM_R_LOCK;
    return true;
  }

  inline bool getCoreID(uint8_t semaID, uint8_t &coreID)
  {
    coreID = (regs->R[semaID] & HSEM_R_COREID) >> HSEM_R_COREID_Pos;
    return true;
  }

  inline bool getProcessID(uint8_t semaID, uint8_t &procID)
  {
    coreID = (regs->R[semaID] & HSEM_R_PROCID) >> HSEM_R_PROCID_Pos;
    return true;
  }
};

#define HWSEMA_IMPL THwSema_stm32

#endif /* HWRTC_STM32_H_ */
