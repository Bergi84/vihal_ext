/*
 * zCluster_stm32wb.h
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */

#ifndef ZCLUSTER_STM32WB_H_
#define ZCLUSTER_STM32WB_H_

#define ZCLUSTER_PRE_ONLY
#include "zCluster.h"

class Tzc_stm32wb : public TzcBase_pre
{
protected:
  Tzc_stm32wb() {};
  virtual ~Tzc_stm32wb() {};

private:
  friend class Tze_stm32wb;
  virtual bool init();
};

#define TZCBASE_IMPL Tzc_stm32wb

#endif /* ZCLUSTER_STM32WB_H_ */
