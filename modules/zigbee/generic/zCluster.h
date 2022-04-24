/*
 * zCluster.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZCLUSTER_H_
#define VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZCLUSTER_H_

#include <stdint.h>
#include "generic_defs.h"
#include "zEndpoint.h"

class TzcBase
{
private:
  TzcBase() {};
  virtual ~TzcBase() {};

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

  typedef enum
  {
    AT_nodata = 0x00,
    AT_data8 = 0x08,
    AT_data16 = 0x09,
    AT_data24 = 0x0a,
    AT_data32 = 0x0b,
    AT_data40 = 0x0c,
    AT_data48 = 0x0d,
    AT_data56 = 0x0e,
    AT_data64 = 0x0f,
    AT_bool = 0x10,
    AT_map8 = 0x18,
    AT_map16 = 0x19,
    AT_map24 = 0x1a,
    AT_map32 = 0x1b,
    AT_map40 = 0x1c,
    AT_map48 = 0x1d,
    AT_map56 = 0x1e,
    AT_map64 = 0x1f,
    AT_uint8 = 0x20,
    AT_uint16 = 0x21,
    AT_uint24 = 0x22,
    AT_uint32 = 0x23,
    AT_uint40 = 0x24,
    AT_uint48 = 0x25,
    AT_uint56 = 0x26,
    AT_uint64 = 0x27,
    AT_int8 = 0x28,
    AT_int16 = 0x29,
    AT_int24 = 0x2a,
    AT_int32 = 0x2b,
    AT_int40 = 0x2c,
    AT_int48 = 0x2d,
    AT_int56 = 0x2e,
    AT_int64 = 0x2f,
    AT_enum8 = 0x30,
    AT_enum16 = 0x31,
    AT_semi = 0x38,
    AT_single = 0x39,
    AT_double = 0x3a,
    AT_octstr = 0x41,
    AT_string = 0x42,
    AT_octstr16 = 0x43,
    AT_string16 = 0x44,
    AT_array = 0x48,
    AT_struct = 0x4c,
    AT_set = 0x50,
    AT_bag = 0x51,
    AT_ToD = 0xe0,
    AT_date = 0xe1,
    AT_UTC = 0xe2,
    AT_clusterId = 0xe8,
    AT_attribId = 0xe9,
    AT_bacOID = 0xea,
    AT_EUI64 = 0xf0,
    AT_key128 = 0xf1,
    AT_unkowen = 0xff
  } attrType_t;

  typedef struct
  {
    uint16_t id;
    attrType_t type;
    bool mandatory;
  } attrRec_t;

  typedef struct
  {
    uint16_t zeId;
    uint16_t zcId;
  } zcuid_t;

  zcuid_t zcuid;

  virtual uint16_t getClusterId();

  virtual void getCmdList(cmdRec_t* &aCmdList, uint8_t &aCmdListLen);

  virtual void getCmdCbList(cmdCbRec_t* &aCmdCbList, uint8_t &aCmdListLen);

  virtual void getAttrList(attrRec_t* &aAttrList, uint8_t &aAttrListLen);

  virtual bool isServer();

private:
  friend class TzeBase;

  TzeBase* parent;
  TzcBase* next;
  TzcBase* prev;

};

class TzcOnOff : public TzcBase
{
private:
  TzcOnOff() {};
  virtual  ~TzcOnOff() {};

public:
  static constexpr uint16_t clusterId = 0x0006;
  static constexpr uint8_t cmdListLen = 6;
  static constexpr uint8_t attrListLen = 4;

  static constexpr cmdRec_t cmdList[cmdListLen] = {
      {0x00, 0, true},    // off
      {0x01, 0, true},    // on
      {0x02, 0, true},    // toggle
      {0x40, 2, false},   // off with effect
      {0x41, 0, false},   // on with recall global scene
      {0x42, 5, false},   // on with timed off
  };

  cmdCbRec_t cmdCbList[cmdListLen];

  static constexpr attrRec_t attrList[attrListLen] = {
      {0x0000, AT_bool, true},    // OnOff
      {0x4000, AT_bool, false},   // GlobalSceneConmtrol
      {0x4001, AT_uint16, false}, // OnTime
      {0x4002, AT_uint16, false}, // OffWaitTime
  };

  virtual void getCmdList(cmdRec_t* &aCmdList, uint8_t &aCmdListLen) { aCmdList = cmdList; aCmdListLen = cmdListLen; };
  virtual void getAttrList(attrRec_t* &aAttrList, uint8_t &aAttrListLen) { aAttrList = attrList; aAttrListLen = attrListLen; };
  virtual void getCmdCbList(cmdCbRec_t* &aCmdCbList, uint8_t &aCmdListLen) { aCmdCbList = cmdCbList; aCmdListLen = cmdListLen; };

  inline void setOnCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[1].pObj = aPObj; cmdCbList[1].pMFunc = aPMFunc;};
  inline void setOnCmdCb(void (*aPFunc)(void *arg))
    { cmdCbList[1].pObj = 0; cmdCbList[1].pFunc = aPFunc;};

  inline void setOffCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[0].pObj = aPObj; cmdCbList[0].pMFunc = aPMFunc;};
  inline void setOffCmdCb(void (*aPFunc)(void *arg))
    { cmdCbList[0].pObj = 0; cmdCbList[0].pMFunc = aPFunc;};

  inline void setToggleCmdCb(TCbClass* aPObj, void (TCbClass::*aPMFunc)())
    { cmdCbList[2].pObj = aPObj; cmdCbList[2].pMFunc = aPMFunc;};
  inline void setToggleCmdCb(void (*aPFunc)(void *arg))
    { cmdCbList[2].pObj = 0; cmdCbList[2].pMFunc = aPFunc;};
};

class TzcOnOffServer : public TzcOnOff
{
public:
  TzcOnOffServer() {};
  virtual ~TzcOnOffServer() {};

  virtual bool isServer() {return true;};

  inline void getAttrOnOff(bool &aVal);
  inline void setAttrOnOff(bool aVal);
};

class TzcOnOffClient : public TzcOnOff
{
public:
  TzcOnOffClient() {};
  virtual ~TzcOnOffClient() {};

  virtual bool isServer() {return false;};

  inline void sendCmdOn(zAdr_t* adr);
  inline void sendCmdOff(zAdr_t* adr);
  inline void sendCmdToggle(zAdr_t* adr);
};




#endif /* VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZCLUSTER_H_ */
