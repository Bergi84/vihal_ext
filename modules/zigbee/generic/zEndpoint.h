/*
 * zEndpoint.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZENDPOINT_PRE_H_
#define ZENDPOINT_PRE_H_

#include <stdint.h>

class TzdBase_pre;
class TzcBase_pre;

class TzeBase_pre
{
public:
  TzeBase_pre() {
    epId = invalidId;
    device = 0;
    next = 0;
    prev = 0;
    clusters = 0;
  };

  bool addCluster(TzcBase_pre* aCluster);
  bool setEpId(uint8_t aEpId);
  uint8_t getEpId();
  inline TzeBase_pre* getNext() {return next;};

private:
  friend class TzdBase_pre;

  static constexpr uint8_t invalidId = 0;

  uint8_t epId;

  TzdBase_pre* device;
  TzeBase_pre* next;
  TzeBase_pre* prev;
  TzcBase_pre* clusters;
};

#endif /* ZENDPOINT_PRE_H_ */

#ifndef ZENDPOINT_PRE_ONLY

#ifndef ZENDPOINT_H_
#define ZENDPOINT_H_

#include "z_impl.h"

class TzeBase : public TZEBASE_IMPL
{

};

#endif /* ZENDPOINT_H_ */

#else
  #undef ZENDPOINT_PRE_ONLY
#endif
