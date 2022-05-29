/*
 * zLibrary_stm32wb.h
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */

#ifdef ZLIBRARY_PRE_DONE

#ifndef ZLIBRARY_STM32WB_H_
#define ZLIBRARY_STM32WB_H_

#define ZLIBRARY_PRE_ONLY
#include "zLibrary.h"

class TzcOnOffServer_stm32wb : public TzcOnOffServer_pre
{
public:
  TzcOnOffServer_stm32wb() {};

private:
  virtual bool init() override;
};

class TzcOnOffClient_stm32wb : public TzcOnOffClient_pre
{
public:
  TzcOnOffClient_stm32wb() {};

private:
  virtual bool init() override;
};

#define TZCONOFFSERVER_IMPL TzcOnOffServer_stm32wb
#define TZCONOFFCLIENT_IMPL TzcOnOffClient_stm32wb


#endif /* ZLIBRARY_STM32WB_H_ */

#endif /* ZLIBRARY_PRE_DONE */
