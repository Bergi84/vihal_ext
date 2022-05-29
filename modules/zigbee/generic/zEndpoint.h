/*
 * zEndpoint.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZENDPOINT_H_
#define ZENDPOINT_H_

#include <stdint.h>

class TzdBase;
class TzcBase;

class TzeBase
{
public:
  TzeBase() {
    epId = invalidId;
    device = 0;
    next = 0;
    prev = 0;
    clusters = 0;
  };

  bool addCluster(TzcBase* aCluster);
  bool setEpId(uint8_t aEpId);
  uint8_t getEpId();

private:
  friend class TzdBase;

  static constexpr uint8_t invalidId = 0;

  uint8_t epId;

  TzdBase* device;
  TzeBase* next;
  TzeBase* prev;
  TzcBase* clusters;
};

#endif /* ZENDPOINT_H_ */
