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

#include "zcl/zcl.h"

class TzcOnOffServer_stm32wb : public TzcOnOffServer_pre
{
public:
  TzcOnOffServer_stm32wb() {};
  virtual ~TzcOnOffServer_stm32wb() = 0;

private:
  static enum ZclStatusCodeT cmdHandler(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdrPtr, struct ZbApsdeDataIndT *dataIndPtr);

  static constexpr struct ZbZclAttrT attrList[] = {
      //  attrId,           ZclDataTypeT,         ZclAttrFlagT,                                       customValSz,  callback,     range {min, max}, reportingInterval{min, max}
      {   ZCL_ATTR_ONOFF,   ZCL_DATATYPE_BOOLEAN, ZCL_ATTR_FLAG_REPORTABLE,                           0,            0,            {0, 0},           {0, 3600}}
  };

  virtual bool init() override;

public:
  inline void getAttrOnOff(bool &aVal) {
    uint8_t attrVal;
    ZbZclAttrRead(clusterHandler, ZCL_ATTR_ONOFF, NULL, &attrVal, sizeof(attrVal), false);
    aVal = attrVal > 0 ? true : false;
  };

  inline void setAttrOnOff(bool aVal) {
    uint8_t attrVal = aVal ? 1 : 0;
    ZbZclAttrWrite(clusterHandler, 0, ZCL_ATTR_ONOFF, &attrVal, sizeof(attrVal), ZCL_ATTR_WRITE_FLAG_FORCE);
  };
};

class TzcOnOffClient_stm32wb : public TzcOnOffClient_pre
{

public:
  TzcOnOffClient_stm32wb() {};
  virtual ~TzcOnOffClient_stm32wb() = 0;

  bool sendCmdOff(zAdr_t* adr = 0);
  bool sendCmdOn(zAdr_t* adr = 0);
  bool sendCmdToggle(zAdr_t* adr = 0);

private:
  virtual bool init() override;

  static void cmdRspCbHandler(struct ZbZclCommandRspT *rsp, void *arg);
};

#define TZCONOFFSERVER_IMPL TzcOnOffServer_stm32wb
#define TZCONOFFCLIENT_IMPL TzcOnOffClient_stm32wb


#endif /* ZLIBRARY_STM32WB_H_ */

#endif /* ZLIBRARY_PRE_DONE */
