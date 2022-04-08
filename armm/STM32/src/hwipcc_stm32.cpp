/*
 * hwrtc_stm32.cpp
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#ifndef HWRTC_STM32_CPP_
#define HWRTC_STM32_CPP_

#include "hwipcc.h"

#if defined(MCUSF_WB)

bool THwIpcc_stm32::init()
{
  regs = IPCC;

  // enable ipcc clock
  RCC->AHB3ENR |= RCC_AHB3ENR_IPCCEN;

  // enable interrupt generation
  regs->C1CR |= IPCC_C1CR_RXOIE | IPCC_C1CR_TXFIE;

  // enable interrupts
  __NVIC_EnableIRQ(IPCC_C1_RX_IRQn);
  __NVIC_EnableIRQ(IPCC_C1_TX_IRQn);

  return true;
}

bool THwIpcc_stm32::enableRxChannel(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)())
{
  enableRxChannel(aChannel);
  setRxCb(aChannel, mPObj, aPMFunc);
  return true;
}

bool THwIpcc_stm32::enableRxChannel(channel_t aChannel, void (*aPFunc)())
{
  enableRxChannel(aChannel);
  setRxCb(aChannel, aPFunc);
  return true;
}


bool THwIpcc_stm32::setRxCb(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)())
{
  uint32_t index = 31 - __CLZ((int32_t) aChannel);
  irqRxCbList[index].pObj = mPObj;
  irqRxCbList[index].pMFunc = aPMFunc;
  return true;
}

bool THwIpcc_stm32::setRxCb(channel_t aChannel, void (*aPFunc)())
{
  uint32_t index = 31 - __CLZ((int32_t) aChannel);
  irqRxCbList[index].pObj = 0;
  irqRxCbList[index].pFunc = aPFunc;
  return true;
}

bool THwIpcc_stm32::enableTxChannel(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)())
{
  enableTxChannel(aChannel);
  setTxCb(aChannel, mPObj, aPMFunc);
  return true;
}

bool THwIpcc_stm32::enableTxChannel(channel_t aChannel, void (*aPFunc)())
{
  enableTxChannel(aChannel);
  setTxCb(aChannel, aPFunc);
  return true;
}

bool THwIpcc_stm32::setTxCb(channel_t aChannel, TCbClass* mPObj, void (TCbClass::*aPMFunc)())
{
  uint32_t index = 31 - __CLZ((int32_t) aChannel);
  irqTxCbList[index].pObj = mPObj;
  irqTxCbList[index].pMFunc = aPMFunc;
  return true;
}

bool THwIpcc_stm32::setTxCb(channel_t aChannel, void (*aPFunc)())
{
  uint32_t index = 31 - __CLZ((int32_t) aChannel);
  irqTxCbList[index].pObj = 0;
  irqTxCbList[index].pFunc = aPFunc;
  return true;
}

#endif // defined(MCUSF_WB)

#endif /* HWRTC_STM32_CPP_ */
