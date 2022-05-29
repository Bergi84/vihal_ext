/*
 * zCluster.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZCLUSTER_PRE_H_
#define ZCLUSTER_PRE_H_

#include <stdint.h>
#include "generic_defs.h"

class TzeBase;

class TzcBase
{
protected:
  uint16_t clId;

  TzcBase() {
    endpoint = 0;
    next = 0;
    prev = 0;
    clId = 0;
  };
  virtual ~TzcBase();

  virtual bool init();

public:
  typedef struct
  {
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)();
      void (*pFunc)();
    };
  } cmdCbRec_t;

  typedef struct
  {
    uint8_t id;
    uint8_t payLen;
    bool mandatory;
  } cmdRec_t;

  typedef struct
  {
    // todo: add variables for different address methods
    enum {
      MODE_INVALID = 0,
      MODE_GROUP,
      MODE_SHORT,
      MODE_EXTENDED,
      MODE_INTERPAN
    } mode;

    union {
      uint16_t nwkAdr;  // network address
      uint64_t extAdr;  // extended address
    };

    union {
      uint16_t epId;    // end point Id if not an inter pan package
      uint16_t panID;   // used in mode INTERPAN, epId = ZB_ENDPOINT_INTERPAN
    };

  } zAdr_t;

  uint16_t getClusterId();
  bool isServer();

private:
  friend class TzeBase;

  TzeBase* endpoint;
  TzcBase* next;
  TzcBase* prev;

};

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


#endif /* ZCLUSTER_PRE_H_ */

#ifndef ZCLUSTER_H_
#define ZCLUSTER_H_

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

#endif /* ZCLUSTER_H_ */
