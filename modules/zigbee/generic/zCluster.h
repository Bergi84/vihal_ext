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

class TzcBase_pre
{
protected:
  uint16_t clId;

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
  TzcBase_pre* getNextCluster() {return next;};
  virtual cmdCbRec_t* getCbList(uint8_t* len = 0) = 0;
  bool isServer();

protected:
  friend class TzeBase_pre;

  TzeBase_pre* endpoint;
  TzcBase_pre* next;
  TzcBase_pre* prev;

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
