/*
 * zLibrary.h
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */

#ifndef ZLIBRARY_PRE_H_
#define ZLIBRARY_PRE_H_

#include <stdint.h>
#include "zCluster.h"

class TzcOnOff : public TzcBase
{
protected:
  TzcOnOff() {clId = clusterId;};

public:
  static constexpr uint16_t clusterId = 0x0006;
  static constexpr uint8_t cmdListLen = 3;

  enum cmdListInd
  {
      CLI_OFF = 0,
      CLI_ON = 1,
      CLI_TOG = 2,
  };

  cmdCbRec_t cmdCbList[cmdListLen];

  inline uint16_t getClusterId() {return clusterId;};
  virtual cmdCbRec_t* getCbList(uint8_t* len = 0) {if(len) *len = cmdListLen; return cmdCbList;};

  inline void setOffCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[CLI_OFF].pObj = aPObj; cmdCbList[CLI_OFF].pMFunc = aPMFunc;};
  inline void setOffCmdCb(void (*aPFunc)())
    { cmdCbList[CLI_OFF].pObj = 0; cmdCbList[CLI_OFF].pFunc = aPFunc;};

  inline void setOnCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[CLI_ON].pObj = aPObj; cmdCbList[CLI_ON].pMFunc = aPMFunc;};
  inline void setOnCmdCb(void (*aPFunc)())
    { cmdCbList[CLI_ON].pObj = 0; cmdCbList[CLI_ON].pFunc = aPFunc;};

  inline void setToggleCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[CLI_TOG].pObj = aPObj; cmdCbList[CLI_TOG].pMFunc = aPMFunc;};
  inline void setToggleCmdCb(void (*aPFunc)())
    { cmdCbList[CLI_TOG].pObj = 0; cmdCbList[CLI_TOG].pFunc = aPFunc;};
};

class TzcOnOffServer_pre : public TzcOnOff
{
public:
  TzcOnOffServer_pre() {};

  inline bool isServer() {return true;};

  inline void getAttrOnOff(bool &aVal);
  inline void setAttrOnOff(bool aVal);
};

class TzcOnOffClient_pre : public TzcOnOff
{
public:
  TzcOnOffClient_pre() {};

  inline bool isServer() {return false;};

  inline void sendCmdOff(zAdr_t* adr = 0);
  inline void sendCmdOn(zAdr_t* adr = 0);
  inline void sendCmdToggle(zAdr_t* adr = 0);
};

#define ZLIBRARY_PRE_DONE

#endif /* ZLIBRARY_PRE_H_ */





#ifndef ZLIBRARY_PRE_ONLY

#ifndef ZLIBRARY_H_
#define ZLIBRARY_H_

#include "z_impl.h"

class TzcOnOffServer : public TZCONOFFSERVER_IMPL
{
public:
  TzcOnOffServer() {};
};

class TzcOnOffClient : public TZCONOFFCLIENT_IMPL
{
public:
  TzcOnOffClient() {};
};

#endif /* ZLIBRARY_H_ */

#else
  #undef ZLIBRARY_PRE_ONLY
#endif




