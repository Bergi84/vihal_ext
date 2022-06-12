#include "zDevice.h"
#include "zCluster.h"
#include "zEndpoint.h"

#include "zigbee_interface.h"
#include "zcl/zcl.h"

bool Tze_stm32wb::init()
{
  // todo: register endpoint

  struct ZbApsmeAddEndpointReqT req;
  struct ZbApsmeAddEndpointConfT conf;

  memset(&req, 0, sizeof(req));

  req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
  req.deviceId = ZCL_DEVICE_ONOFF_SWITCH;
  req.endpoint = epId;

  ZbZclAddEndpoint(((TzdBase*)device)->getZHandler(), &req, &conf);
  if(conf.status != ZB_STATUS_SUCCESS)
  {
    return false;
  }

  TzcBase* aktCluster = (TzcBase*) clusters;
  while(aktCluster != 0)
  {
    aktCluster->init();
    aktCluster = (TzcBase*) aktCluster->getNextCluster();
  }
  return true;
}
