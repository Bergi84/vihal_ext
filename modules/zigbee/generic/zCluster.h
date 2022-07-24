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

class TzeBase_pre;
class TzdBase_pre;

class TzcBase_pre
{
protected:
  TzcBase_pre() {
    endpoint = 0;
    next = 0;
    prev = 0;
    clId = 0;
  };
  virtual ~TzcBase_pre() = 0;

public:
  typedef struct
  {
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

  typedef enum
  {
    ZCL_STATUS_SUCCESS = 0x00,
    ZCL_STATUS_FAILURE = 0x01,
    ZCL_STATUS_NOT_AUTHORIZED = 0x7e,
    ZCL_STATUS_RESERVED_FIELD_NOT_ZERO = 0x7f,
    ZCL_STATUS_MALFORMED_COMMAND = 0x80,
    ZCL_STATUS_UNSUP_CLUSTER_COMMAND = 0x81,
    ZCL_STATUS_UNSUP_GENERAL_COMMAND = 0x82,
    ZCL_STATUS_UNSUP_MANUF_CLUSTER_COMMAND = 0x83,
    ZCL_STATUS_UNSUP_MANUF_GENERAL_COMMAND = 0x84,
    ZCL_STATUS_INVALID_FIELD = 0x85,
    ZCL_STATUS_UNSUPPORTED_ATTRIBUTE = 0x86,
    ZCL_STATUS_INVALID_VALUE = 0x87,
    ZCL_STATUS_READ_ONLY = 0x88,
    ZCL_STATUS_INSUFFICIENT_SPACE = 0x89,
    ZCL_STATUS_DUPLICATE_EXISTS = 0x8a,
    ZCL_STATUS_NOT_FOUND = 0x8b,
    ZCL_STATUS_UNREPORTABLE_ATTRIBUTE = 0x8c,
    ZCL_STATUS_INVALID_DATA_TYPE = 0x8d,
    ZCL_STATUS_INVALID_SELECTOR = 0x8e,
    ZCL_STATUS_WRITE_ONLY = 0x8f,
    ZCL_STATUS_INCONSISTENT_STARTUP_STATE = 0x90,
    ZCL_STATUS_DEFINED_OUT_OF_BAND = 0x91,
    ZCL_STATUS_INCONSISTENT = 0x92,
    ZCL_STATUS_ACTION_DENIED = 0x93,
    ZCL_STATUS_TIMEOUT = 0x94,
    ZCL_STATUS_ABORT = 0x95,
    ZCL_STATUS_INVALID_IMAGE = 0x96,
    ZCL_STATUS_WAIT_FOR_DATA = 0x97,
    ZCL_STATUS_NO_IMAGE_AVAILABLE = 0x98,
    ZCL_STATUS_REQUIRE_MORE_IMAGE = 0x99,
    ZCL_STATUS_NOTIFICATION_PENDING = 0x9A,
    ZCL_STATUS_HARDWARE_FAILURE = 0xc0,
    ZCL_STATUS_SOFTWARE_FAILURE = 0xc1,
    ZCL_STATUS_CALIBRATION_ERROR = 0xc2,
    ZCL_STATUS_UNSUPP_CLUSTER = 0xc3,
  } zclStatusCode_t;

  typedef struct
  {
    TCbClass* pObj;
    union {
      zclStatusCode_t (TCbClass::*pMFunc)(zAdr_t* aAdr);
      zclStatusCode_t (*pFunc)(zAdr_t* aAdr);
    };
  } cmdInCbRec_t;

  typedef struct
  {
    TCbClass* pObj;
    union {
      void (TCbClass::*pMFunc)(zAdr_t* aAdr, uint8_t status);
      void (*pFunc)(zAdr_t* aAdr, uint8_t status);
    };
  } cmdOutCbRec_t;

  inline uint16_t getClusterId() {return clId;};
  TzcBase_pre* getNextCluster() {return next;};

  bool isServer() {return server;};

  TzdBase_pre* getDevice();

protected:
  friend class TzeBase_pre;

  TzeBase_pre* endpoint;
  TzcBase_pre* next;
  TzcBase_pre* prev;

  uint16_t clId;
  bool server;
};

#endif /* ZCLUSTER_PRE_H_ */

#ifndef ZCLUSTER_PRE_ONLY

#ifndef ZCLUSTER_H_
#define ZCLUSTER_H_

#include "z_impl.h"

class TzcBase : public TZCBASE_IMPL
{
protected:
  TzcBase() {};
  virtual ~TzcBase() = 0;
};

#endif /* ZCLUSTER_H_ */

#else
  #undef ZCLUSTER_PRE_ONLY
#endif
