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


TzcLevelServer_stm32wb::~TzcLevelServer_stm32wb()
{


}

bool TzcLevelServer_stm32wb::init()
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

enum ZclStatusCodeT TzcLevelServer_stm32wb::cmdHandler(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdrPtr, struct ZbApsdeDataIndT *dataIndPtr)
{
  uint8_t cmd_id = zclHdrPtr->cmdId;
  zclStatusCode_t status = ZCL_STATUS_SUCCESS;

  TzcLevelServer_stm32wb* pObj = (TzcLevelServer_stm32wb*) cluster->app_cb_arg;

  //src_info.tx_options = dataIndPtr->securityStatus.;

  zAdr_t adrInt;
  pObj->adrConv2int(&dataIndPtr->src, adrInt);

  switch (cmd_id) {
    case ZCL_CMD_Move_OnOff:
    case ZCL_CMD_Move:
      if(dataIndPtr->asduLength != 2)
      {
        status = ZCL_STATUS_MALFORMED_COMMAND;
      }
      else
      {
        bool dirDown = dataIndPtr->asdu[0];
        uint8_t rate = dataIndPtr->asdu[1];

        if(pObj->cb.cmd.move.pObj)
        {
          if(pObj->cb.cmd.move.pMFunc)
            status = (pObj->cb.cmd.move.pObj->*pObj->cb.cmd.move.pMFunc)(&adrInt, dirDown, rate);
          else
            status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
        }
        else
        {
          if(pObj->cb.cmd.move.pFunc)
            status = pObj->cb.cmd.move.pFunc(&adrInt, dirDown, rate);
          else
            status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
        }
      }
      break;

    case ZCL_CMD_MoveTo_OnOff:
    case ZCL_CMD_MoveTo:
      if(dataIndPtr->asduLength != 3)
      {
        status = ZCL_STATUS_MALFORMED_COMMAND;
      }
      else
      {
        uint8_t level = dataIndPtr->asdu[0];
        uint16_t time = (uint16_t)dataIndPtr->asdu[1] + ((uint16_t)dataIndPtr->asdu[2] << 8);

        if(pObj->cb.cmd.moveTo.pObj)
        {
          if(pObj->cb.cmd.moveTo.pMFunc)
            status = (pObj->cb.cmd.moveTo.pObj->*pObj->cb.cmd.moveTo.pMFunc)(&adrInt, level, time);
          else
            status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
        }
        else
        {
          if(pObj->cb.cmd.moveTo.pFunc)
            status = pObj->cb.cmd.moveTo.pFunc(&adrInt, level, time);
          else
            status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
        }
      }
      break;

    case ZCL_CMD_Step_OnOff:
    case ZCL_CMD_Step:
      if(dataIndPtr->asduLength != 4)
      {
        status = ZCL_STATUS_MALFORMED_COMMAND;
      }
      else
      {
        bool dirDown = dataIndPtr->asdu[0];
        uint8_t stepSize = dataIndPtr->asdu[1];
        uint16_t time = (uint16_t) dataIndPtr->asdu[2] + ((uint16_t)dataIndPtr->asdu[3] << 8);

        if(pObj->cb.cmd.step.pObj)
        {
          if(pObj->cb.cmd.step.pMFunc)
            status = (pObj->cb.cmd.step.pObj->*pObj->cb.cmd.step.pMFunc)(&adrInt, dirDown, stepSize, time);
          else
            status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
        }
        else
        {
          if(pObj->cb.cmd.step.pFunc)
            status = pObj->cb.cmd.step.pFunc(&adrInt, dirDown, stepSize, time);
          else
            status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
        }
      }
      break;

    case ZCL_CMD_Stop_OnOff:
    case ZCL_CMD_Stop:
      if(pObj->cb.cmd.stop.pObj)
      {
        if(pObj->cb.cmd.stop.pMFunc)
          status = (pObj->cb.cmd.stop.pObj->*pObj->cb.cmd.stop.pMFunc)(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      else
      {
        if(pObj->cb.cmd.stop.pFunc)
          status = pObj->cb.cmd.stop.pFunc(&adrInt);
        else
          status = ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
      }
      break;
  }
  return (enum ZclStatusCodeT) status;
}

TzcLevelClient_stm32wb::~TzcLevelClient_stm32wb()
{


}

bool TzcLevelClient_stm32wb::init()
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

void TzcLevelClient_stm32wb::cmdRspCbHandler(struct ZbZclCommandRspT *rsp, void *arg)
{
  TzcLevelClient_stm32wb* pObj = (TzcLevelClient_stm32wb*) arg;

  switch (rsp->hdr.cmdId) {
    case TzcLevelServer_stm32wb::ZCL_CMD_Move_OnOff:
    case TzcLevelServer_stm32wb::ZCL_CMD_Move:
      if(pObj->cb.cmd.move.pObj)
      {
        if(pObj->cb.cmd.move.pMFunc)
          (pObj->cb.cmd.move.pObj->*pObj->cb.cmd.move.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.move.pFunc)
          pObj->cb.cmd.move.pFunc((zclStatusCode_t) rsp->status);
      }
      break;

    case TzcLevelServer_stm32wb::ZCL_CMD_MoveTo_OnOff:
    case TzcLevelServer_stm32wb::ZCL_CMD_MoveTo:
      if(pObj->cb.cmd.moveTo.pObj)
      {
        if(pObj->cb.cmd.moveTo.pMFunc)
          (pObj->cb.cmd.moveTo.pObj->*pObj->cb.cmd.moveTo.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.moveTo.pFunc)
          pObj->cb.cmd.moveTo.pFunc((zclStatusCode_t) rsp->status);
      }
      break;

    case TzcLevelServer_stm32wb::ZCL_CMD_Step_OnOff:
    case TzcLevelServer_stm32wb::ZCL_CMD_Step:
      if(pObj->cb.cmd.step.pObj)
      {
        if(pObj->cb.cmd.step.pMFunc)
          (pObj->cb.cmd.step.pObj->*pObj->cb.cmd.step.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.step.pFunc)
          pObj->cb.cmd.step.pFunc((zclStatusCode_t) rsp->status);
      }
      break;

    case TzcLevelServer_stm32wb::ZCL_CMD_Stop_OnOff:
    case TzcLevelServer_stm32wb::ZCL_CMD_Stop:
      if(pObj->cb.cmd.stop.pObj)
      {
        if(pObj->cb.cmd.stop.pMFunc)
          (pObj->cb.cmd.stop.pObj->*pObj->cb.cmd.stop.pMFunc)((zclStatusCode_t) rsp->status);
      }
      else
      {
        if(pObj->cb.cmd.stop.pFunc)
          pObj->cb.cmd.stop.pFunc((zclStatusCode_t) rsp->status);
      }
      break;
  }
}

bool TzcLevelClient_stm32wb::sendCmdMove(bool aDirDown, uint8_t aRate, zAdr_t* aAdr)
{
  if(!getDevice()->flagJoined)
    return false;

  uint8_t payload[2];

  payload[0] = aDirDown;
  payload[1] = aRate;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(aAdr, req.dst);
  req.cmdId = TzcLevelServer_stm32wb::ZCL_CMD_Move;
  req.length = 2;
  req.payload = payload;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};

bool TzcLevelClient_stm32wb::sendCmdMoveToLevel(uint8_t aLevel, uint16_t aTime, zAdr_t* aAdr)
{
  if(!getDevice()->flagJoined)
    return false;

  uint8_t payload[3];

  payload[0] = aLevel;
  payload[1] = aTime & 0x00FF;
  payload[2] = (aTime & 0xFF00) >> 8;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(aAdr, req.dst);
  req.cmdId = TzcLevelServer_stm32wb::ZCL_CMD_MoveTo;
  req.length = 3;
  req.payload = payload;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};

bool TzcLevelClient_stm32wb::sendCmdStep(bool aDirDown, uint8_t aStepSize, uint16_t aTime, zAdr_t* aAdr)
{
  if(!getDevice()->flagJoined)
    return false;

  uint8_t payload[4];

  payload[0] = aDirDown;
  payload[1] = aStepSize;
  payload[2] = aTime & 0x00FF;
  payload[3] = (aTime & 0xFF00) >> 8;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(aAdr, req.dst);
  req.cmdId = TzcLevelServer_stm32wb::ZCL_CMD_Step;
  req.length = 4;
  req.payload = payload;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};

bool TzcLevelClient_stm32wb::sendCmdStop(zAdr_t* aAdr)
{
  if(!getDevice()->flagJoined)
    return false;

  struct ZbZclClusterCommandReqT req;
  adrConv2st(aAdr, req.dst);
  req.cmdId = TzcLevelServer_stm32wb::ZCL_CMD_Stop;
  req.length = 0;
  req.payload = 0;
  req.noDefaultResp = ZCL_NO_DEFAULT_RESPONSE_FALSE;

  ZbZclClusterCommandReq(clusterHandler, &req, cmdRspCbHandler, (void*) this);
  return true;
};
