/*
 * zEndpoint.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZENDPOINT_STM32WB_H_
#define ZENDPOINT_STM32WB_H_

#define ZENDPOINT_PRE_ONLY
#include "zEndpoint.h"

class Tze_stm32wb : public TzeBase_pre
{
private:
  friend class Tzd_stm32;

  bool init();
};

#define TZEBASE_IMPL Tze_stm32wb

#endif /* ZENDPOINT_STM32WB_H_ */
