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







class TzcLevelServer_stm32wb : public TzcLevelServer_pre
{
public:
  TzcLevelServer_stm32wb() {};
  virtual ~TzcLevelServer_stm32wb() = 0;

private:
  static enum ZclStatusCodeT cmdHandler(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdrPtr, struct ZbApsdeDataIndT *dataIndPtr);

  static constexpr struct ZbZclAttrT attrList[] = {
      //  attrId,                 ZclDataTypeT,                 ZclAttrFlagT,                                       customValSz,  callback,     range {min, max}, reportingInterval{min, max}
      {   ZCL_ATTR_CurrentLevel,   ZCL_DATATYPE_UNSIGNED_8BIT,  ZCL_ATTR_FLAG_REPORTABLE,                           0,            0,            {0x0, 0xfe},           {0, 3600}}
  };

  virtual bool init() override;

public:
  inline void getAttrCurrentLevel(uint8_t &aLevel) {
    ZbZclAttrRead(clusterHandler, ZCL_ATTR_CurrentLevel, NULL, &aLevel, sizeof(aLevel), false);
  };

  inline void setAttrCurrentLevel(uint8_t aLevel) {
    ZbZclAttrWrite(clusterHandler, 0, ZCL_ATTR_CurrentLevel, &aLevel, sizeof(aLevel), ZCL_ATTR_WRITE_FLAG_FORCE);
  };
};

class TzcLevelClient_stm32wb : public TzcLevelClient_pre
{
public:
  TzcLevelClient_stm32wb(void) {};
  virtual ~TzcLevelClient_stm32wb() = 0;

  bool sendCmdMoveToLevel(uint8_t aLevel, uint16_t aTime, zAdr_t* aAdr = 0);
  bool sendCmdMove(bool aDirDown, uint8_t aRate, zAdr_t* aAdr = 0);
  bool sendCmdStep(bool aDirDown, uint8_t aStepSize, uint16_t aTime, zAdr_t* aAdr = 0);
  bool sendCmdStop(zAdr_t* aAdr = 0);

private:
  virtual bool init() override;

  static void cmdRspCbHandler(struct ZbZclCommandRspT *rsp, void *arg);
};

#define TZCLEVELSERVER_IMPL TzcLevelServer_stm32wb
#define TZCLEVELCLIENT_IMPL TzcLevelClient_stm32wb





class TzcAnalogInServer_stm32wb : public TzcAnalogInServer_pre
{
public:
  TzcAnalogInServer_stm32wb() {};
  virtual ~TzcAnalogInServer_stm32wb() = 0;

private:
  virtual bool init() override;

  static constexpr struct ZbZclAttrT attrList[] = {
      //  attrId,                 ZclDataTypeT,                 ZclAttrFlagT,                                       customValSz,  callback,     range {min, max}, reportingInterval{min, max}
      {   ZCL_ATTR_OutOfService,  ZCL_DATATYPE_BOOLEAN,         ZCL_ATTR_FLAG_REPORTABLE,                           0,            0,            {0x0, 0x01},      {0, 3600}},
      {   ZCL_ATTR_PresentValue,  ZCL_DATATYPE_FLOATING_SINGLE, ZCL_ATTR_FLAG_REPORTABLE,                           0,            0,            {0x0, 0x0},       {0, 3600}},
      {   ZCL_ATTR_StatusFlags,   ZCL_DATATYPE_BITMAP_8BIT,     ZCL_ATTR_FLAG_REPORTABLE,                           0,            0,            {0x0, 0x0f},       {0, 3600}}
  };

public:
  inline void getAttrOutOfService(bool &aOOS) {
    uint8_t val;
    ZbZclAttrRead(clusterHandler, ZCL_ATTR_OutOfService, NULL, &val, sizeof(val), false);
    aOOS = val > 0 ? true : false;
  };
  inline void setAttrOutOfService(bool aOOS) {
    uint8_t val = aOOS ? 1 : 0;
    ZbZclAttrWrite(clusterHandler, 0, ZCL_ATTR_OutOfService, &val, sizeof(val), ZCL_ATTR_WRITE_FLAG_FORCE);
  };
  inline void getAttrPresentValue(float &aVal) {
    ZbZclAttrRead(clusterHandler, ZCL_ATTR_PresentValue, NULL, &aVal, sizeof(aVal), false);
  };
  inline void setAttrPresentValue(float aVal) {
    ZbZclAttrWrite(clusterHandler, 0, ZCL_ATTR_PresentValue, (uint8_t*)&aVal, sizeof(aVal), ZCL_ATTR_WRITE_FLAG_FORCE);
  };
  inline void getAttrStatusFlags(bool &aAlarm, bool &aFault, bool &aOverriden, bool &aOutOfService) {
    uint8_t val;
    ZbZclAttrRead(clusterHandler, ZCL_ATTR_PresentValue, NULL, &val, sizeof(val), false);
    aAlarm = val & 0x01 ? true : false;
    aFault = val & 0x02 ? true : false;
    aOverriden = val & 0x04 ? true : false;
    aOutOfService = val & 0x08 ? true : false;
  };
  inline void setAttrStatusFlags(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService) {
    uint8_t val = (aAlarm ? 0x01 : 0x00) | (aFault ? 0x02 : 0x00) | (aOverriden ? 0x04 : 0x00) | (aOutOfService ? 0x08 : 0x00);
    ZbZclAttrWrite(clusterHandler, 0, ZCL_ATTR_StatusFlags, &val, sizeof(val), ZCL_ATTR_WRITE_FLAG_FORCE);
  }
};

class TzcAnalogInClient_stm32wb : public TzcAnalogInClient_pre
{
public:
  TzcAnalogInClient_stm32wb() {};
  virtual ~TzcAnalogInClient_stm32wb() = 0;

private:
  virtual bool init() override;
};

#define TZCANALOGINSERVER_IMPL TzcAnalogInServer_stm32wb
#define TZCANALOGINCLIENT_IMPL TzcAnalogInClient_stm32wb


#endif /* ZLIBRARY_STM32WB_H_ */

#endif /* ZLIBRARY_PRE_DONE */
