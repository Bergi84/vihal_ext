/*
 * zCluster_stm32wb.cpp
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */
#include "zLibrary.h"
#include "zDevice.h"
#include "zCluster.h"
#include "zEndpoint.h"

struct ZbZclOnOffServerCallbacksT onOffServerCallbacks;

bool TzcOnOffServer_stm32wb::init()
{
  clusterHandler = ZbZclOnOffServerAlloc(getZHandler(), endpoint->getEpId(), (ZbZclOnOffServerCallbacksT*) &onOffServerCallbacks, (void*) this);
  ZbZclClusterEndpointRegister(clusterHandler);
  return true;
}


bool TzcOnOffClient_stm32wb::init()
{
  clusterHandler = ZbZclOnOffClientAlloc(getZHandler(), endpoint->getEpId());
  ZbZclClusterEndpointRegister(clusterHandler);
  return true;
}
