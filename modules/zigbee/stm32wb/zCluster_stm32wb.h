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
#include "zEndpoint.h"

#include "zcl/zcl.h"

class Tzc_stm32wb : public TzcBase_pre
{
protected:
  Tzc_stm32wb() {clusterHandler = 0;};
  virtual ~Tzc_stm32wb() = 0;

  struct ZbZclClusterT* clusterHandler;

  inline ZigBeeT* getZHandler() {return ((TzeBase*)endpoint)->getZHandler();};

  static statusCode_t cmdInCbHandler(uint16_t cmd, Tzc_stm32wb* pCluster, struct ZbZclAddrInfoT *srcInfo);
  static void cmdOutCbHandler(uint16_t cmd, Tzc_stm32wb* pCluster, struct ZbZclCommandRspT *rsp);

  void adrConv2st(zAdr_t* intAdr, ZbApsAddrT &adr_st);
  void adrConv2int(ZbApsAddrT* stAdr, zAdr_t &adr_int);

private:
  friend class Tze_stm32wb;
  virtual bool init() = 0;
};

#define TZCBASE_IMPL Tzc_stm32wb

#endif /* ZCLUSTER_STM32WB_H_ */
