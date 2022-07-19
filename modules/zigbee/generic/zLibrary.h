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


class TzcOnOffServer_pre : public TzcBase
{
public:
  cmdInCbRec_t cmdInCbList[3];

  TzcOnOffServer_pre()
  {
    clId =0x0006;
    cmdInListLen = 3;
    cmdOutListLen = 0;
    pCmdInCbList = cmdInCbList;
    pCmdOutCbList = 0;
  ;};
  virtual ~TzcOnOffServer_pre() = 0;

  enum cmdInListInd
  {
      CLI_OFF = 0,
      CLI_ON = 1,
      CLI_TOG = 2,
  };



  inline bool isServer() {return true;};

  inline void getAttrOnOff(bool &aVal);
  inline void setAttrOnOff(bool aVal);

  inline void setOffCmdInCb(TCbClass* aPObj, statusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cmdInCbList[CLI_OFF].pObj = aPObj; cmdInCbList[CLI_OFF].pMFunc = aPMFunc;};
  inline void setOffCmdInCb(statusCode_t (*aPFunc)(zAdr_t*))
    { cmdInCbList[CLI_OFF].pObj = 0; cmdInCbList[CLI_OFF].pFunc = aPFunc;};

  inline void setOnCmdInCb(TCbClass* aPObj, statusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cmdInCbList[CLI_ON].pObj = aPObj; cmdInCbList[CLI_ON].pMFunc = aPMFunc;};
  inline void setOnCmdInCb(statusCode_t (*aPFunc)(zAdr_t*))
    { cmdInCbList[CLI_ON].pObj = 0; cmdInCbList[CLI_ON].pFunc = aPFunc;};

  inline void setToggleCmdInCb(TCbClass* aPObj, statusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cmdInCbList[CLI_TOG].pObj = aPObj; cmdInCbList[CLI_TOG].pMFunc = aPMFunc;};
  inline void setToggleCmdInCb(statusCode_t (*aPFunc)(zAdr_t*))
    { cmdInCbList[CLI_TOG].pObj = 0; cmdInCbList[CLI_TOG].pFunc = aPFunc;};
};

class TzcOnOffClient_pre : public TzcBase
{
public:
  cmdOutCbRec_t cmdOutCbList[3];

  TzcOnOffClient_pre()
  {
    clId =0x0006;
    cmdInListLen = 0;
    cmdOutListLen = 3;
    pCmdInCbList = 0;
    pCmdOutCbList = cmdOutCbList;
  ;}
;
  virtual ~TzcOnOffClient_pre() = 0;

  enum cmdOutListInd
  {
      CLI_OFF = 0,
      CLI_ON = 1,
      CLI_TOG = 2,
  };



  inline bool isServer() {return false;};

  inline bool sendCmdOff(zAdr_t* adr = 0);
  inline bool sendCmdOn(zAdr_t* adr = 0);
  inline bool sendCmdToggle(zAdr_t* adr = 0);

  inline void setOffCmdOutCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zAdr_t*, uint8_t))
    { cmdOutCbList[CLI_OFF].pObj = aPObj; cmdOutCbList[CLI_OFF].pMFunc = aPMFunc;};
  inline void setOffCmdOutCb(void (*aPFunc)(zAdr_t*, uint8_t))
    { cmdOutCbList[CLI_OFF].pObj = 0; cmdOutCbList[CLI_OFF].pFunc = aPFunc;};

  inline void setOnCmdOutCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zAdr_t*, uint8_t))
    { cmdOutCbList[CLI_ON].pObj = aPObj; cmdOutCbList[CLI_ON].pMFunc = aPMFunc;};
  inline void setOnCmdOutCb(void (*aPFunc)(zAdr_t*, uint8_t))
    { cmdOutCbList[CLI_ON].pObj = 0; cmdOutCbList[CLI_ON].pFunc = aPFunc;};

  inline void setToggleCmdOutCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zAdr_t*, uint8_t))
    { cmdOutCbList[CLI_TOG].pObj = aPObj; cmdOutCbList[CLI_TOG].pMFunc = aPMFunc;};
  inline void setToggleCmdOutCb(void (*aPFunc)(zAdr_t*, uint8_t))
    { cmdOutCbList[CLI_TOG].pObj = 0; cmdOutCbList[CLI_TOG].pFunc = aPFunc;};
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
  virtual ~TzcOnOffServer();
};

class TzcOnOffClient : public TZCONOFFCLIENT_IMPL
{
public:
  TzcOnOffClient() {};
  virtual ~TzcOnOffClient();
};

#endif /* ZLIBRARY_H_ */

#else
  #undef ZLIBRARY_PRE_ONLY
#endif




