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

  cmdCbRec_t cmdCbList[cmdListLen];

  inline uint16_t getClusterId() {return clusterId;};

  inline void setOffCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[0].pObj = aPObj; cmdCbList[0].pMFunc = aPMFunc;};
  inline void setOffCmdCb(void (*aPFunc)())
    { cmdCbList[0].pObj = 0; cmdCbList[0].pFunc = aPFunc;};

  inline void setOnCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[1].pObj = aPObj; cmdCbList[1].pMFunc = aPMFunc;};
  inline void setOnCmdCb(void (*aPFunc)())
    { cmdCbList[1].pObj = 0; cmdCbList[1].pFunc = aPFunc;};

  inline void setToggleCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[2].pObj = aPObj; cmdCbList[2].pMFunc = aPMFunc;};
  inline void setToggleCmdCb(void (*aPFunc)())
    { cmdCbList[2].pObj = 0; cmdCbList[2].pFunc = aPFunc;};
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




