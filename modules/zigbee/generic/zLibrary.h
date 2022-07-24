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
protected:
  struct
  {
    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr);
        };
      } on;

      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr);
        };
      } off;

      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr);
        };
      } tog;
    } cmd;
  } cb;

public:
  typedef enum {
    ZCL_ATTR_ONOFF = 0x0000
  } attrId_t;

  typedef enum {
    ZCL_CMD_OFF = 0x0000,
    ZCL_CMD_ON = 0x0001,
    ZCL_CMD_TOG = 0x0002
  } cmdId_t;

  TzcOnOffServer_pre()
  {
    clId = 0x0006;
    server = true;

    cb.cmd.on.pObj = 0;
    cb.cmd.on.pMFunc = 0;
    cb.cmd.off.pObj = 0;
    cb.cmd.off.pMFunc = 0;
    cb.cmd.tog.pObj = 0;
    cb.cmd.tog.pMFunc = 0;
  };
  virtual ~TzcOnOffServer_pre() = 0;

  // set functions for cmd call backs
  inline void setCmdOnCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cb.cmd.on.pObj = aPObj; cb.cmd.on.pMFunc = aPMFunc;};
  inline void setCmdOnCb(zclStatusCode_t (*aPFunc)(zAdr_t*))
    { cb.cmd.on.pObj = 0; cb.cmd.on.pFunc = aPFunc;};

  inline void setCmdOffCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cb.cmd.off.pObj = aPObj; cb.cmd.off.pMFunc = aPMFunc;};
  inline void setCmdOffCb(zclStatusCode_t (*aPFunc)(zAdr_t*))
    { cb.cmd.off.pObj = 0; cb.cmd.off.pFunc = aPFunc;};

  inline void setCmdTogCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cb.cmd.tog.pObj = aPObj; cb.cmd.tog.pMFunc = aPMFunc;};
  inline void setCmdTogCb(zclStatusCode_t (*aPFunc)(zAdr_t*))
    { cb.cmd.tog.pObj = 0; cb.cmd.tog.pFunc = aPFunc;};

  void getAttrOnOff(bool &aVal);
  void setAttrOnOff(bool aVal);
};

class TzcOnOffClient_pre : public TzcBase
{
protected:
  struct
  {
    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } on;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } off;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } tog;
    } cmd;

    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(bool aVal, zclStatusCode_t aStatus);
          void (*pFunc)(bool aVal, zclStatusCode_t aStatus);
        };
      } onOff;
    } attr;
  } cb;

public:
  TzcOnOffClient_pre()
  {
    clId = 0x0006;
    server = false;
  };
  virtual ~TzcOnOffClient_pre() = 0;

  inline void setCmdOnCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.on.pObj = aPObj; cb.cmd.on.pMFunc = aPMFunc;};
  inline void setCmdOnCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.on.pObj = 0; cb.cmd.on.pFunc = aPFunc;};

  inline void setCmdOffCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.off.pObj = aPObj; cb.cmd.off.pMFunc = aPMFunc;};
  inline void setCmdOffCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.off.pObj = 0; cb.cmd.off.pFunc = aPFunc;};

  inline void setCmdTogCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.tog.pObj = aPObj; cb.cmd.tog.pMFunc = aPMFunc;};
  inline void setCmdTogCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.tog.pObj = 0; cb.cmd.tog.pFunc = aPFunc;};

  inline void setReadOnOffCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(bool aVal, zclStatusCode_t aStatus))
    { cb.attr.onOff.pObj = aPObj; cb.attr.onOff.pMFunc = aPMFunc;};
  inline void setReadOnOffCb(void (*aPFunc)(bool aVal, zclStatusCode_t aStatus))
    { cb.attr.onOff.pObj = 0; cb.attr.onOff.pFunc = aPFunc;};

  bool sendCmdOff(zAdr_t* adr = 0);
  bool sendCmdOn(zAdr_t* adr = 0);
  bool sendCmdToggle(zAdr_t* adr = 0);

  void readAttrOnOff(zAdr_t* aAdr = 0);
};

class TzcLevelServer_pre : public TzcBase
{
protected:
  struct
  {
    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr, uint8_t aLevel, uint16_t aTime);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr, uint8_t aLevel, uint16_t aTime);
        };
      } moveTo;

      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aRate);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aRate);
        };
      } move;

      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aStepSize, uint16_t aTime);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aStepSize, uint16_t aTime);
        };
      } step;

      struct
      {
        TCbClass* pObj;
        union {
          zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr);
          zclStatusCode_t (*pFunc)(zAdr_t* aAdr);
        };
      } stop;
    } cmd;
  } cb;

public:
  TzcLevelServer_pre()
  {
    clId = 0x0008;
    server = true;
  };
  virtual ~TzcLevelServer_pre() = 0;

  // set functions for cmd call backs
  inline void setCmdMoveToCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t* aAdr, uint8_t aLevel, uint16_t aTime))
    { cb.cmd.moveTo.pObj = aPObj; cb.cmd.moveTo.pMFunc = aPMFunc;};
  inline void setCmdMoveToCb(zclStatusCode_t (*aPFunc)(zAdr_t* aAdr, uint8_t aLevel, uint16_t aTime))
    { cb.cmd.moveTo.pObj = 0; cb.cmd.moveTo.pFunc = aPFunc;};

  inline void setCmdMoveCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aRate))
    { cb.cmd.move.pObj = aPObj; cb.cmd.move.pMFunc = aPMFunc;};
  inline void setCmdMoveCb(zclStatusCode_t (*aPFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aRate))
    { cb.cmd.move.pObj = 0; cb.cmd.move.pFunc = aPFunc;};

  inline void setCmdStepCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aStepSize, uint16_t aTime))
    { cb.cmd.step.pObj = aPObj; cb.cmd.step.pMFunc = aPMFunc;};
  inline void setCmdStepCb(zclStatusCode_t (*aPFunc)(zAdr_t* aAdr, bool aDirDown, uint8_t aStepSize, uint16_t aTime))
    { cb.cmd.step.pObj = 0; cb.cmd.step.pFunc = aPFunc;};

  inline void setCmdStopCb(TCbClass* aPObj, zclStatusCode_t (TCbClass::*aPMFunc)(zAdr_t*))
    { cb.cmd.stop.pObj = aPObj; cb.cmd.stop.pMFunc = aPMFunc;};
  inline void setCmdStopCb(zclStatusCode_t (*aPFunc)(zAdr_t*))
    { cb.cmd.stop.pObj = 0; cb.cmd.stop.pFunc = aPFunc;};

  inline void getAttrCurrentLevel(uint8_t &aLevel);
  inline void setAttrCurrentLevel(uint8_t aLevel);
};

class TzcLevelClient_pre : public TzcBase
{
protected:
  struct
  {
    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } moveTo;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } move;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } step;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(zclStatusCode_t aStatus);
          void (*pFunc)(zclStatusCode_t aStatus);
        };
      } stop;
    } cmd;

    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(uint8_t aLevel, zclStatusCode_t aStatus);
          void (*pFunc)(uint8_t aLevel, zclStatusCode_t aStatus);
        };
      } currentLevel;
    } attr;
  } cb;

public:
  TzcLevelClient_pre()
  {
    clId = 0x0008;
    server = false;
  };
  virtual ~TzcLevelClient_pre() = 0;

  // set functions for cmd call backs
  inline void setCmdMoveToCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.moveTo.pObj = aPObj; cb.cmd.moveTo.pMFunc = aPMFunc;};
  inline void setCmdMoveToCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.moveTo.pObj = 0; cb.cmd.moveTo.pFunc = aPFunc;};

  inline void setCmdMoveCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.move.pObj = aPObj; cb.cmd.move.pMFunc = aPMFunc;};
  inline void setCmdMoveCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.move.pObj = 0; cb.cmd.move.pFunc = aPFunc;};

  inline void setCmdStepCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.step.pObj = aPObj; cb.cmd.step.pMFunc = aPMFunc;};
  inline void setCmdStepCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.step.pObj = 0; cb.cmd.step.pFunc = aPFunc;};

  inline void setCmdStopCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(zclStatusCode_t aStatus))
    { cb.cmd.stop.pObj = aPObj; cb.cmd.stop.pMFunc = aPMFunc;};
  inline void setCmdStopCb(void (*aPFunc)(zclStatusCode_t aStatus))
    { cb.cmd.stop.pObj = 0; cb.cmd.stop.pFunc = aPFunc;};

  inline void setReadCurrentLevelCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(uint8_t aLevel, zclStatusCode_t aStatus))
    { cb.attr.currentLevel.pObj = aPObj; cb.attr.currentLevel.pMFunc = aPMFunc;};
  inline void setReadCurrentLevelCb(void (*aPFunc)(uint8_t aLevel, zclStatusCode_t aStatus))
    { cb.attr.currentLevel.pObj = 0; cb.attr.currentLevel.pFunc = aPFunc;};

  inline bool sendCmdMoveToLevel(uint8_t aLevel, uint16_t aTime, zAdr_t* aAdr = 0);
  inline bool sendCmdMove(bool aDirDown, uint8_t aRate, zAdr_t* aAdr = 0);
  inline bool sendCmdStep(uint8_t aStepSize, uint16_t aTime, zAdr_t* aAdr = 0);
  inline bool sendCmdStop(zAdr_t* aAdr = 0);

  inline void readAttrCurrentLevel(zAdr_t* aAdr = 0);
};


class TzcAnalogInServer_pre : public TzcBase
{
protected:

public:
  TzcAnalogInServer_pre()
  {
    clId = 0x000C;
    server = true;
  };
  virtual ~TzcAnalogInServer_pre() = 0;

  inline void getAttrOutOfService(bool &aOOS);
  inline void setAttrOutOfService(bool aOOS);
  inline void getAttrPresentValue(float &aVal);
  inline void setAttrPresentValue(float aVal);
  inline void getAttrStatusFlags(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService);
  inline void setAttrStatusFlags(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService);

};

class TzcAnalogInClient_pre : public TzcBase
{
protected:
  struct
  {
    struct
    {
      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(bool &aOOS, zclStatusCode_t aStatus);
          void (*pFunc)(bool &aOOS, zclStatusCode_t aStatus);
        };
      } outOfService;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(float &aVal, zclStatusCode_t aStatus);
          void (*pFunc)(float &aVal, zclStatusCode_t aStatus);
        };
      } presentValue;

      struct
      {
        TCbClass* pObj;
        union {
          void (TCbClass::*pMFunc)(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService, zclStatusCode_t aStatus);
          void (*pFunc)(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService, zclStatusCode_t aStatus);
        };
      } statusFlags;
    } attr;
  } cb;
public:
  TzcAnalogInClient_pre()
  {
    clId = 0x000C;
    server = false;
  };

  virtual ~TzcAnalogInClient_pre() = 0;

  inline void setReadOutOfServiceCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(bool &aOOS, zclStatusCode_t aStatus))
    { cb.attr.outOfService.pObj = aPObj; cb.attr.outOfService.pMFunc = aPMFunc;};
  inline void setReadOutOfServiceCb(void (*aPFunc)(bool &aOOS, zclStatusCode_t aStatus))
    { cb.attr.outOfService.pObj = 0; cb.attr.outOfService.pFunc = aPFunc;};

  inline void setReadPresentValueCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(float &aVal, zclStatusCode_t aStatus))
    { cb.attr.presentValue.pObj = aPObj; cb.attr.presentValue.pMFunc = aPMFunc;};
  inline void setReadPresentValue(void (*aPFunc)(float &aVal, zclStatusCode_t aStatus))
    { cb.attr.presentValue.pObj = 0; cb.attr.presentValue.pFunc = aPFunc;};

  inline void setReadStatusFlagsCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService, zclStatusCode_t aStatus))
    { cb.attr.statusFlags.pObj = aPObj; cb.attr.statusFlags.pMFunc = aPMFunc;};
  inline void setReadStatusFlagsCb(void (*aPFunc)(bool aAlarm, bool aFault, bool aOverriden, bool aOutOfService, zclStatusCode_t aStatus))
    { cb.attr.statusFlags.pObj = 0; cb.attr.statusFlags.pFunc = aPFunc;};

  inline void readAttrOutOfService(zAdr_t* aAdr = 0);
  inline void readAttrPresentValue(zAdr_t* aAdr = 0);
  inline void readAttrStatusFlags(zAdr_t* aAdr = 0);
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




