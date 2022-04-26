/*
 * zDevice.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZDEVICE_H_
#define VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZDEVICE_H_

#include "zCluster.h"
#include "zEndpoint.h"

class TzdBase
{
public:
  void addEndpoint(TzeBase* aEndpoint);

private:
  friend class TzdBase;

  TzeBase* endpoints;
};





#endif /* VIHAL_EXT_MODULES_ZIGBEE_GENERIC_ZDEVICE_H_ */
