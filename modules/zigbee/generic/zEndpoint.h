/*
 * zEndpoint.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZENDPOINT_H_
#define VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZENDPOINT_H_

#include "zDevice.h"
#include "zCluster.h"

class TzeBase
{
public:
  void addCluster(TzcBase* aCluster);

private:
  friend class TzeBase;

  TzdBase* device;
  TzeBase* next;
  TzeBase* prev;
  TzcBase* clusters;
};

#endif /* VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZENDPOINT_H_ */
