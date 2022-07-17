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

constexpr struct ZbZclOnOffServerCallbacksT TzcOnOffServer_stm32wb::onOffServerCallbacks;

TzcOnOffServer_stm32wb::~TzcOnOffServer_stm32wb()
{

}

bool TzcOnOffServer_stm32wb::init()
{
  clusterHandler = ZbZclOnOffServerAlloc(getZHandler(), endpoint->getEpId(), (ZbZclOnOffServerCallbacksT*) &onOffServerCallbacks, (void*) this);
  ZbZclClusterEndpointRegister(clusterHandler);
  return true;
}


TzcOnOffClient_stm32wb::~TzcOnOffClient_stm32wb()
{

}

bool TzcOnOffClient_stm32wb::init()
{
  clusterHandler = ZbZclOnOffClientAlloc(getZHandler(), endpoint->getEpId());
  ZbZclClusterEndpointRegister(clusterHandler);
  return true;
}
