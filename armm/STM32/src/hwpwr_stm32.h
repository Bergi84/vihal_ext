/*
 * hwpwr_stm32.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWPWR_STM32_H_
#define HWPWR_STM32_H_

#define HWPWR_PRE_ONLY
#include "hwpwr.h"

class THwPwr_stm32 : public THwPwr_pre
{
public:
  typedef enum
  {
    LPM_SLEEP = -1,
    LPM_STOP0 = 0,
    LPM_STOP1 = 1,
    LPM_STOP2 = 2,
    LPM_STANDBY = 3,
    LPM_SHUTDOWN = 4
  } lpm_t;

  // only allowed to call if system frequency is lower or equal to 2Mhz
  // if system is in low power run mode it can not enter stop2 mode
  // instead stop mode1 must used
  void enterLowPowerRunMode();
  void leaveLowPowerRunMode();


  void enterLowPowerMode(lpm_t aLpm)
  {
    if(aLpm >= LPM_STOP0)
    {
      SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
      uint32_t tmp = PWR->CR1;
      uint32_t conf = ((uint32_t)aLpm) << PWR_CR1_LPMS_Pos;
      if((tmp & PWR_CR1_LPMS) != conf)
      {
        tmp &= ~PWR_CR1_LPMS;
        tmp |= ((uint32_t)aLpm) << PWR_CR1_LPMS_Pos;
        PWR->CR1 = tmp;
      }
    }
    else
    {
      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
    }
    __WFI();
  }

  // the voltage which should be selected depends on the
  // the desired maximum RF transmit power
  // for max power (tx code 31) 1.7V plus load depended voltage derating is needed
  // the maximum possible load derating is 50mV + 10mV tolerance
  // for more information see AN5246
  // the limitation of start up current limits the current peaks during
  // wake up from sleep but also increases wakeup time
  // 80mA => ~100us wake up time
  // 200mA => ~50us wake up time
  // voltage range 1.1V up to 1.85V (minimum voltage is 1.4V + derating + tolerance)
  // current range 80mA up to 220mA
  void enableSMPS(float voltage, float startCurrent);

  // with active RF system only range 1 is allowed
  // range 2 is only with a system clock below 16Mhz allowed
  void setVoltageRange();

};

#define HWPWR_IMPL THwPwr_stm32

#endif /* HWRTC_STM32_H_ */
