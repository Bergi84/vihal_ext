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
#include "zcl/general/zcl.onoff.h"

class TzcOnOffServer_stm32wb : public TzcOnOffServer_pre
{
public:
  TzcOnOffServer_stm32wb() {};
  virtual ~TzcOnOffServer_stm32wb() = 0;

private:
  virtual bool init() override;

  static enum ZclStatusCodeT cbOn(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg) {return (ZclStatusCodeT) cmdInCbHandler(CLI_ON, (TzcOnOffServer_stm32wb*)arg, srcInfo);};
  static enum ZclStatusCodeT cbOff(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg) {return (ZclStatusCodeT) cmdInCbHandler(CLI_OFF, (TzcOnOffServer_stm32wb*)arg, srcInfo);};
  static enum ZclStatusCodeT cbTog(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg) {return (ZclStatusCodeT) cmdInCbHandler(CLI_TOG, (TzcOnOffServer_stm32wb*)arg, srcInfo);};

  static constexpr struct ZbZclOnOffServerCallbacksT onOffServerCallbacks = {
      .off = cbOff,
      .on = cbOn,
      .toggle = cbTog
  };

public:
  inline void getAttrOnOff(bool &aVal) {uint8_t attrVal; ZbZclAttrRead(clusterHandler, ZCL_ONOFF_ATTR_ONOFF, NULL, &attrVal, sizeof(attrVal), false); aVal = attrVal;};
  inline void setAttrOnOff(bool aVal) { if(aVal > 1) aVal = 1; ZbZclAttrIntegerWrite(clusterHandler, ZCL_ONOFF_ATTR_ONOFF, aVal); };
};

class TzcOnOffClient_stm32wb : public TzcOnOffClient_pre
{

public:
  TzcOnOffClient_stm32wb() {};
  virtual ~TzcOnOffClient_stm32wb() = 0;

  inline void sendCmdOff(zAdr_t* adr = 0) { ZbApsAddrT adrSt; adrConv2st(adr, adrSt); ZbZclOnOffClientOffReq(clusterHandler, &adrSt, cbOff, (void*) this);};
  inline void sendCmdOn(zAdr_t* adr = 0) { ZbApsAddrT adrSt; adrConv2st(adr, adrSt); ZbZclOnOffClientOnReq(clusterHandler, &adrSt, cbOff, (void*) this);};
  inline void sendCmdToggle(zAdr_t* adr = 0) { ZbApsAddrT adrSt; adrConv2st(adr, adrSt); ZbZclOnOffClientToggleReq(clusterHandler, &adrSt, cbOff, (void*) this);};

private:
  virtual bool init() override;

  static void cbOn(struct ZbZclCommandRspT *rsp, void *arg) {return cmdOutCbHandler(CLI_ON, (TzcOnOffServer_stm32wb*)arg, rsp);};
  static void cbOff(struct ZbZclCommandRspT *rsp, void *arg) {return cmdOutCbHandler(CLI_OFF, (TzcOnOffServer_stm32wb*)arg, rsp);};
  static void cbTog(struct ZbZclCommandRspT *rsp, void *arg) {return cmdOutCbHandler(CLI_TOG, (TzcOnOffServer_stm32wb*)arg, rsp);};
};

#define TZCONOFFSERVER_IMPL TzcOnOffServer_stm32wb
#define TZCONOFFCLIENT_IMPL TzcOnOffClient_stm32wb


#endif /* ZLIBRARY_STM32WB_H_ */

#endif /* ZLIBRARY_PRE_DONE */
