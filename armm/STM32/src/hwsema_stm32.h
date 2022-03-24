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

  HSEM_TypeDef* regs;

  bool init();
  bool lock(uint8_t semaID, uint8_t process);
  bool release(uint8_t semaID, uint8_t process);
  bool status(uint8_t semaID);
  bool getCoreID(uint8_t semaID);
  bool getProcessID(uint8_t semaID);
  bool enableIrq(uint8_t semaID);
  bool disableIrq(uint8_t semaID);
};

#define HWSEMA_IMPL THwSema_stm32

#endif /* HWRTC_STM32_H_ */
