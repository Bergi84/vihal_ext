/*
 * zEndpoint.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZENDPOINT_STM32WB_H_
#define ZENDPOINT_STM32WB_H_

#include <stdint.h>
#include "zEndpoint.h"

class Tze_stm32wb : public TzeBase
{
private:
  friend class Tzd_stm32;

  bool init();
};

#endif /* ZENDPOINT_STM32WB_H_ */
