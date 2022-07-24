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

constexpr struct ZbZclAttrT TzcOnOffServer_stm32wb::attrList[];

TzcOnOffServer_stm32wb::~TzcOnOffServer_stm32wb()
{

}

bool TzcOnOffServer_stm32wb::init()
{
  clusterHandler = (ZbZclClusterT*) ZbZclClusterAlloc(getZHandler(), sizeof(ZbZclClusterT), (enum ZbZclClusterIdT) clId, endpoint->getEpId(), ZCL_DIRECTION_TO_SERVER);
  if (clusterHandler == 0) {
      return false;
  }

  clusterHandler->txOptions |= ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY;

  clusterHandler->command = cmdHandler;

  if ((zclStatusCode_t) ZbZclAttrAppendList(clusterHandler, attrList, ZCL_ATTR_LIST_LEN(attrList)) != ZCL_STATUS_SUCCESS)
  {
      ZbZclClusterFree(clusterHandler);
      clusterHandler = 0;
      return false;
  }

  ZbZclClusterSetCallbackArg(clusterHandler, this);

  ZbZclClusterAttach(clusterHandler);

  ZbZclClusterEndpointRegister(clusterHandler);
  return true;
}

enum ZclStatusCodeT TzcOnOffServer_stm32wb::cmdHandler(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdrPtr, struct ZbApsdeDataIndT *dataIndPtr)
{
  uint8_t cmd_id = zclHdrPtr->cmdId;
  zclStatusCode_t status = ZCL_STATUS_SUCCESS;

  TzcOnOffServer_stm32wb* pObj = (TzcOnOffServer_stm32wb*) cluster->app_cb_arg;

  //src_info.tx_options = dataIndPtr->securityStatus.;

  zAdr_t adrInt;
  pObj->adrConv2int(&dataIndPtr->src, adrInt);

  switch (cmd_id) {
    case ZCL_CMD_OFF:
      if(pObj->cb.cmd.off.pObj)
      {
        if(pObj->cb.cmd.off.pMFunc)
          status = (pObj->cb.cmd.off.pObj->*pObj->cb.cmd.off.pMFunc)(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      else
      {
        if(pObj->cb.cmd.off.pFunc)
          status = pObj->cb.cmd.off.pFunc(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      break;

    case ZCL_CMD_ON:
      if(pObj->cb.cmd.on.pObj)
      {
        if(pObj->cb.cmd.on.pMFunc)
          status = (pObj->cb.cmd.on.pObj->*pObj->cb.cmd.on.pMFunc)(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      else
      {
        if(pObj->cb.cmd.on.pFunc)
          status = pObj->cb.cmd.on.pFunc(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      break;

    case ZCL_CMD_TOG:
      if(pObj->cb.cmd.tog.pObj)
      {
        if(pObj->cb.cmd.tog.pMFunc)
          status = (pObj->cb.cmd.tog.pObj->*pObj->cb.cmd.tog.pMFunc)(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      else
      {
        if(pObj->cb.cmd.on.pFunc)
          status = pObj->cb.cmd.on.pFunc(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      break;
  }
  return (enum ZclStatusCodeT) status;
}

//enum ZclStatusCodeT TzcOnOffServer_stm32wb::rwCbHandler(struct ZbZclClusterT *cluster, struct ZbZclAttrCbInfoT *info)
//{
//
//}

TzcOnOffClient_stm32wb::~TzcOnOffClient_stm32wb()
{

}

bool TzcOnOffClient_stm32wb::init()
{
  clusterHandler = (ZbZclClusterT*) ZbZclClusterAlloc(getZHandler(), sizeof(ZbZclClusterT), (enum ZbZclClusterIdT) clId, endpoint->getEpId(), ZCL_DIRECTION_TO_CLIENT);
  if (clusterHandler == 0) {
      return false;
  }

  clusterHandler->txOptions |= (uint16_t)(ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY | ZB_APSDE_DATAREQ_TXOPTIONS_ACK | ZB_APSDE_DATAREQ_TXOPTIONS_FRAG);

  ZbZclClusterSetCallbackArg(clusterHandler, this);

  ZbZclClusterAttach(clusterHandler);

  ZbZclClusterEndpointRegister(clusterHandler);

  return true;
}

void TzcOnOffClient_stm32wb::cmdRspCbHandler(struct ZbZclCommandRspT *rsp, void *arg)
{
  TzcOnOffClient_stm32wb* pObj = (TzcOnOffClient_stm32wb*) arg;

  switch (rsp->hdr.cmdId) {
    case TzcOnOffServer_pre::ZCL_CMD_OFF:
      if(pObj->cb.cmd.off.pObj)
      {
        if(pObj->cb.cmd.off.pMFunc)
          (pObj->cb.cmd.off.pObj->*pObj->cb.cmd.off.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.off.pFunc)
          pObj->cb.cmd.off.pFunc((zclStatusCode_t) rsp->status);
      }
      break;

    case TzcOnOffServer_pre::ZCL_CMD_ON:
      if(pObj->cb.cmd.on.pObj)
      {
        if(pObj->cb.cmd.on.pMFunc)
          (pObj->cb.cmd.on.pObj->*pObj->cb.cmd.on.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.on.pFunc)
          pObj->cb.cmd.on.pFunc((zclStatusCode_t) rsp->status);
      }
      break;

    case TzcOnOffServer_pre::ZCL_CMD_TOG:
      if(pObj->cb.cmd.on.pObj)
      {
        if(pObj->cb.cmd.on.pMFunc)
          (pObj->cb.cmd.on.pObj->*pObj->cb.cmd.on.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.on.pFunc)
          pObj->cb.cmd.on.pFunc((zclStatusCode_t) rsp->status);
      }
      break;
  }
}

bool TzcOnOffClient_stm32wb::sendCmdOff(zAdr_t* adr)
{
  if(!getDevice()->flagJoined)
    return false;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(adr, req.dst);
  req.cmdId = TzcOnOffServer_pre::ZCL_CMD_OFF;
  req.length = 0;
  req.payload = 0;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};

bool TzcOnOffClient_stm32wb::sendCmdOn(zAdr_t* adr)
{
  if(!getDevice()->flagJoined)
    return false;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(adr, req.dst);
  req.cmdId = TzcOnOffServer_pre::ZCL_CMD_ON;
  req.length = 0;
  req.payload = 0;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};

bool TzcOnOffClient_stm32wb::sendCmdToggle(zAdr_t* adr)
{
  if(!getDevice()->flagJoined)
    return false;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(adr, req.dst);
  req.cmdId = TzcOnOffServer_pre::ZCL_CMD_TOG;
  req.length = 0;
  req.payload = 0;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};
