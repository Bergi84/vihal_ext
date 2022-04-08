/*
 * hwpwr_stm32.cpp
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWPWR_STM32_CPP_
#define HWPWR_STM32_CPP_

#include "hwpwr.h"

#if defined(MCUSF_WB)

bool THwPwr_stm32::startCPU2()
{
  RCC->C2AHB3ENR |= RCC_C2AHB3ENR_IPCCEN;

  EXTI->C2EMR2 |= EXTI_C2IMR2_IM41;
  EXTI->RTSR2 |= EXTI_RTSR2_RT41;

  __SEV( );   // send event to all cpu's
  __WFE( );   // clear event on this cpu

  regs->CR4 |= PWR_CR4_C2BOOT;

  cpu2Started = true;

  return true;
}

void THwPwr_stm32::enableSMPS(uint32_t voltage_mV, uint32_t startCurrent_mA)
{
  uint8_t* baseVal = (uint8_t*)0x1FFF7559;

  uint32_t tmp = regs->CR5;
  tmp &= ~(PWR_CR5_SMPSVOS | PWR_CR5_SMPSSC);
  tmp |= (((voltage_mV - 1500U + 25U)/50U + (*baseVal & 0x0F)) << PWR_CR5_SMPSVOS_Pos) & PWR_CR5_SMPSVOS;
  tmp |= (((startCurrent_mA - 80U)/20U) << PWR_CR5_SMPSSC_Pos) & PWR_CR5_SMPSSC;
  regs->CR5 = tmp;

  tmp |= PWR_CR5_SMPSEN;
  regs->CR5 = tmp;
}

void THwPwr_stm32::disableSMPS()
{
  regs->CR5 &= ~PWR_CR5_SMPSEN;
}

void THwPwr_stm32::setVoltageRange(bool aRange2)
{
  uint32_t tmp = regs->CR1;
  tmp &= ~PWR_CR1_VOS;

  if(aRange2)
  {
    tmp |= PWR_CR1_VOS_1;
  }
  else
  {
    tmp |= PWR_CR1_VOS_0;
  }

  regs->CR1 = tmp;
}

#endif // defined(MCUSF_WB)

#endif /* HWPWR_STM32_CPP_ */
