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
  static constexpr uint32_t coreID = HSEM_CR_COREID_CURRENT;
  static constexpr uint8_t invalidProcID = -1;

  HSEM_TypeDef* regs;

  bool init(HSEM_TypeDef* pHsem)
  {
    regs = pHsem;
    RCC->AHB3ENR |= RCC_AHB3ENR_HSEMEN;
    EXTI->IMR2 |= EXTI_IMR2_IM38;
    return true;
  }

  // returns true if lock is successful
  inline bool lock(uint8_t semaID, uint8_t procID = invalidProcID)
  {
    if(procID == invalidProcID)
    {
      // one step lock without proc id
      return regs->RLR[semaID] != (HSEM_RLR_LOCK | coreID);
    }
    else
    {
      // two step lock with proc id
      regs->R[semaID] = (HSEM_R_LOCK | coreID | (uint32_t)procID);
      return regs->R[semaID] != (HSEM_R_LOCK | coreID | (uint32_t)procID);
    }
  }

  inline bool release(uint8_t semaID, uint8_t procID = invalidProcID)
  {
    if(procID == invalidProcID)
    {
      regs->R[semaID] = coreID;
    }
    else
    {
      regs->R[semaID] = coreID | (uint32_t)procID;
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
    procID = (regs->R[semaID] & HSEM_R_PROCID) >> HSEM_R_PROCID_Pos;
    return true;
  }
};

#define HWSEMA_IMPL THwSema_stm32

#endif /* HWRTC_STM32_H_ */
