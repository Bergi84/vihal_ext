/*
 * zDevice.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZDEVICE_STM32WB_H_
#define ZDEVICE_STM32WB_H_

#include <stdint.h>
#include "zDevice.h"
#include "zigbee_stm32wb.h"


class Tzd_stm32 : public TzdBase
{
private:
  friend class TZigbee_stm32wb;

  struct ZigBeeT *zb;

  bool init(struct ZigBeeT *aZb);

};





#endif /* ZDEVICE_STM32WB_H_ */
