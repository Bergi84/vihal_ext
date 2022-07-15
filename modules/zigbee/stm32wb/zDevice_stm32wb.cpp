/*
 * zigbee_stm32wb.cpp
 *
 *  Created on: 09.04.2022
 *      Author: Bergi
 */

#include <string.h>

#include "zDevice_stm32wb.h"

#include "hw.h"

#include "tl_zigbee_hci.h"
#include "zigbee_interface.h"

#include "shci.h"

#include "zDevice.h"
#include "zCluster.h"
#include "zEndpoint.h"

// this pointer is used for the c interface functionms to the stack
// it is initialized at class construction
static Tzd_stm32* pZigbeeInt = 0;

// static member variables definitions
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t Tzd_stm32::EvtPool[POOL_SIZE];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) TL_CmdPacket_t Tzd_stm32::SystemCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t Tzd_stm32::SystemSpareEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) TL_CmdPacket_t Tzd_stm32::OtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t Tzd_stm32::NotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t Tzd_stm32::NotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];



extern "C" void IRQ_Handler_44() {pZigbeeInt->ipcc.rxIrqHandler();}
extern "C" void IRQ_Handler_45() {pZigbeeInt->ipcc.txIrqHandler();}

/**********************************************/
/*          hw.h interface functions          */
/**********************************************/

extern "C" {
  void HW_IPCC_Init()
  {
    TRACECPU1("HWIPCC init\r\n");

    pZigbeeInt->ipcc.init();
    pZigbeeInt->ipcc.setRxCb(HW_IPCC_TRACES_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::tracesEvtHandler);
  }

  void HW_IPCC_Enable( void )
  {
    TRACECPU1("HWIPCC enable\r\n");

    pZigbeeInt->pwr->startCPU2();
  }

  void HW_IPCC_SYS_Init()
  {
    TRACECPU1("HWIPCC sys init\r\n");

    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_SYSTEM_EVENT_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::sysEvtHandler);
  }

  void HW_IPCC_SYS_SendCmd()
  {
    TRACECPU1("HWIPCC sys send cmd\r\n");

    pZigbeeInt->ipcc.setTxFlag(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
    pZigbeeInt->ipcc.enableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::sysCmdEvtHandler);
  }

  void HW_IPCC_ZIGBEE_Init()
  {
    TRACECPU1("HWIPCC zigbee init\r\n");

    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackNotifEvtHandler);
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackM0RequestHandler);
  }

  void HW_IPCC_ZIGBEE_SendM4RequestToM0( void )
  {
    TRACECPU1("HWIPCC request to M0\r\n");

    pZigbeeInt->ipcc.setTxFlag(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL);
    pZigbeeInt->ipcc.enableTxChannel(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::cmdEvtHandler);

    return;
  }

  void HW_IPCC_ZIGBEE_SendM4AckToM0Notify( void )
  {
    TRACECPU1("HWIPCC notify ack to M0\r\n");

    pZigbeeInt->ipcc.clearRxFlag(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);
    // todo: normally only enable is necessary because function already set in HW_IPCC_ZIGBEE_Init
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackNotifEvtHandler);
  }

  void HW_IPCC_ZIGBEE_SendM4AckToM0Request( void )
  {
    TRACECPU1("HWIPCC request ack to M0\r\n");

    pZigbeeInt->ipcc.clearRxFlag(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);
    // todo: normally only enable is necessary because function already set in HW_IPCC_ZIGBEE_Init
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackM0RequestHandler);
  }

  void HW_IPCC_MM_SendFreeBuf( void (*cb)( void ) )
  {
    TRACECPU1("HWIPCC send free buf\r\n");

    if(pZigbeeInt->ipcc.getTxActivFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL))
    {
      pZigbeeInt->FreeBufCb = cb;
      pZigbeeInt->ipcc.enableTxChannel(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::mmFreeBufHandler);
    }
    else
    {
      cb();
      pZigbeeInt->ipcc.setTxFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
    }
  }

  void HW_IPCC_TRACES_Init()
  {
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_TRACES_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::traceEvtHandler);

  }

  void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
  {
    pZigbeeInt->reqFromM0 = Reqbuffer;

    pZigbeeInt->CptReceiveRequestFromM0++;
    pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdReqFromM0);
  }

  void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
  {
    pZigbeeInt->evtAckFromM0 = true;
  }

  void TL_ZIGBEE_NotReceived(TL_EvtPacket_t *Notbuffer)
  {
    pZigbeeInt->notFromM0 = Notbuffer;

    pZigbeeInt->CptReceiveNotifyFromM0++;
    pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdNotFromM0);
  }

  void TL_TRACES_EvtReceived( TL_EvtPacket_t * hcievt )
  {
    gTrace.printBuf(gTrace.TA_CPU2, (const char *) ((TL_AsynchEvt_t *)(hcievt->evtserial.evt.payload))->payload, (uint32_t)hcievt->evtserial.evt.plen - 2U);
    TL_MM_EvtDone( hcievt );
  }
}

/**********************************************/
/*     wrapper functions for callback         */
/**********************************************/
void cbSysStatusNot( SHCI_TL_CmdStatus_t status )
{
  pZigbeeInt->sysStatusNot(status);
}

void cbSysUserEvtRx( void * pPayload )
{
  pZigbeeInt->sysUserEvtRx(pPayload);
}

void shci_notify_asynch_evt(void* pdata)
{
  pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdShciAsync);
}

void shci_cmd_resp_release(uint32_t flag)
{
  pZigbeeInt->evtShciCmdResp = true;
  return;
}

void shci_cmd_resp_wait(uint32_t timeout)
{
  pZigbeeInt->seq->waitForEvent(&pZigbeeInt->evtShciCmdResp);
  pZigbeeInt->evtShciCmdResp = false;
  return;
}

void cbStoreData(struct ZigBeeT *zb, void *cbarg)
{
  pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdStoreData);
}

void cbStartupDone(enum ZbStatusCodeT status, void *arg)
{
  if(status == ZB_STATUS_SUCCESS)
  {
    // connection was successful
    pZigbeeInt->joined = true;
    pZigbeeInt->wasJoined = true;
    pZigbeeInt->ts->stop(pZigbeeInt->tsTimeOut);
  }
  else
  {
    if(pZigbeeInt->joinTimeOut)
    {
      // no further join attempt
      return;
    }
    else
    {
      // queue a further join attempt
      if(pZigbeeInt->wasJoined)
      {
        pZigbeeInt->ts->start(pZigbeeInt->tsIdJoin, pZigbeeInt->rejoinRetryInterval);
      }
      else
      {
        pZigbeeInt->ts->start(pZigbeeInt->tsIdJoin, pZigbeeInt->joinRetryInterval);
      }
    }
  }
}

/**********************************************/
/*   interface functions for zigbee core      */
/**********************************************/
extern "C" {
  void Pre_ZigbeeCmdProcessing(void)
  {
    // check if there are no pending requests or
    // notifications before send a new command

    pZigbeeInt->evtSyncBypassIdle = false;
    pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdNotFromM0);
    pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdReqFromM0);
    pZigbeeInt->seq->waitForEvent(&pZigbeeInt->evtSyncBypassIdle);
  }

  void ZIGBEE_CmdTransfer(void)
  {
    pZigbeeInt->cmdTransfer();
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_OTCmdPayloadBuffer(void)
  {
    return (Zigbee_Cmd_Request_t *)pZigbeeInt->OtCmdBuffer.cmdserial.cmd.payload;
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
  {
    TL_EvtPacket_t * pkt = (TL_EvtPacket_t *) &pZigbeeInt->OtCmdBuffer;
    return (Zigbee_Cmd_Request_t *)pkt->evtserial.evt.payload;
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_NotificationPayloadBuffer(void)
  {
    return (Zigbee_Cmd_Request_t *)(pZigbeeInt->notFromM0)->evtserial.evt.payload;
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_M0RequestPayloadBuffer(void)
  {
    return (Zigbee_Cmd_Request_t *)(pZigbeeInt->reqFromM0)->evtserial.evt.payload;
  }
}

/**********************************************/
/*          zigbee class definition           */
/**********************************************/
Tzd_stm32::Tzd_stm32()
{
  rfdStackFound = false;
  ffdStackFound = false;

  stackInitDone = false;
  stackConfigDone = false;
  joined = false;
  wasJoined = false;

  joinTimeout = 60000;
  joinRetryInterval = 1000;

  rejoinTimeout = -1;
  rejoinRetryInterval = 60000;

  persistentDataCb.pObjRead = 0;
  persistentDataCb.pFuncRead = 0;
  persistentDataCb.pObjWrite = 0;
  persistentDataCb.pFuncWrite = 0;

  // check if this is the only instance of this class
  if(pZigbeeInt != 0)
    while(1);

  pZigbeeInt = this;
}

bool Tzd_stm32::init(TSequencer* aSeq, THwPwr* aPwr, TTimerServer* aTs)
{
  CptReceiveRequestFromM0 = 0;
  CptReceiveNotifyFromM0 = 0;

  evtAckFromM0 = false;
  evtShciCmdResp = false;

  seq = aSeq;
  pwr = aPwr;
  ts = aTs;
  sema.init();

  TL_Init();

  ts->create(tsIdJoin, this, (void (TCbClass::*)(THwRtc::time_t time)) &Tzd_stm32::retryJoin, false);
  ts->create(tsTimeOut, this, (void (TCbClass::*)(THwRtc::time_t time)) &Tzd_stm32::signalTimeout, false);

  seq->addTask(seqIdShciAsync, shci_user_evt_proc);
  seq->addTask(seqIdNotFromM0, this, (void (TCbClass::*)(void)) &Tzd_stm32::processNotifyM0);
  seq->addTask(seqIdReqFromM0, this, (void (TCbClass::*)(void)) &Tzd_stm32::processRequestM0);
  seq->addTask(seqIdNetworkForm, this, (void (TCbClass::*)(void)) &Tzd_stm32::formNetwork);
  seq->addTask(seqIdStoreData, this, (void (TCbClass::*)(void)) &Tzd_stm32::storePersistentData);

  SHCI_TL_HciInitConf_t SHci_Tl_Init_Conf;
  SHci_Tl_Init_Conf.p_cmdbuffer = (uint8_t*)&SystemCmdBuffer;
  SHci_Tl_Init_Conf.StatusNotCallBack = cbSysStatusNot;
  shci_init(cbSysUserEvtRx, (void*) &SHci_Tl_Init_Conf);

  TL_MM_Config_t tl_mm_config = {0};
  tl_mm_config.p_BleSpareEvtBuffer = 0;
  tl_mm_config.p_SystemSpareEvtBuffer = SystemSpareEvtBuffer;
  tl_mm_config.p_AsynchEvtPool = EvtPool;
  tl_mm_config.AsynchEvtPoolSize = POOL_SIZE;
  TL_MM_Init( &tl_mm_config );

  TL_Enable();

  return true;
}

void Tzd_stm32::sysStatusNot( SHCI_TL_CmdStatus_t status )
{
  // in ST Example this function is empty
}

void Tzd_stm32::sysUserEvtRx(void *pPayload)
{
  TL_AsynchEvt_t *p_sys_event;
  p_sys_event = (TL_AsynchEvt_t*)(((tSHCI_UserEvtRxParam*)pPayload)->pckt->evtserial.evt.payload);

  switch(p_sys_event->subevtcode)
  {
  case SHCI_SUB_EVT_CODE_READY:
    TL_TRACES_Init( );
    initStack();
    break;
  case SHCI_SUB_EVT_ERROR_NOTIF:
  {
    SCHI_SystemErrCode_t errorCode;
    errorCode = (SCHI_SystemErrCode_t) (p_sys_event->payload[0]);
    switch(errorCode)
    {
    case ERR_ZIGBEE_UNKNOWN_CMD:
      TRACECPU2("** ERR_ZIGBEE : UNKNOWN_CMD \r\n");
      break;
    default:
      TRACECPU2("** ERR_ZIGBEE : ErroCode=%d \r\n",errorCode);
      break;
    }
  }
    break;
  default:
    break;
  }
}

void Tzd_stm32::initStack()
{
  checkWirelessFwInfo();

  // in the ST implementation the ZigbeeConfigBuffer is a global static variable
  // placed in section MB_MEM1, but inside TL_ZIGBEE_Init only the pointer of
  // each buffer are copied but not the config pointer it self and the config is never
  // used again in the ST example
  TL_ZIGBEE_Config_t ZigbeeConfigBuffer;
  ZigbeeConfigBuffer.p_ZigbeeOtCmdRspBuffer = (uint8_t *)&OtCmdBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotAckBuffer = (uint8_t *)NotifRspEvtBuffer;
  ZigbeeConfigBuffer.p_ZigbeeNotifRequestBuffer = (uint8_t *)NotifRequestBuffer;
  TL_ZIGBEE_Init(&ZigbeeConfigBuffer);

  SHCI_C2_ZIGBEE_Init();

  zb = ZbInit(0U, NULL, NULL);

  stackInitDone = true;
}

void Tzd_stm32::checkWirelessFwInfo()
{
  WirelessFwInfo_t wirelessInfo;

  SHCI_CmdStatus_t status;
  status = SHCI_GetWirelessFwInfo(&wirelessInfo);

  if(status == SHCI_Success)
  {
    switch (wirelessInfo.StackType) {
    case INFO_STACK_TYPE_ZIGBEE_FFD:
      TRACECPU1("Wireless FW Type : FFD Zigbee stack\r\n");
      ffdStackFound = true;
      break;
    case INFO_STACK_TYPE_ZIGBEE_RFD:
      TRACECPU1("Wireless FW Type : RFD Zigbee stack\r\n");
      rfdStackFound = true;
      break;
    default:
      /* No Zigbee device supported ! */
      TRACECPU1("** ERR_ZIGBEE : No Zigbee stack found,  installed stackType: %hhu\r\n", wirelessInfo.StackType);
      return;
    }
    TRACECPU1("Wireless FW Version : %hhu.%hhu.%hhu\r\n", wirelessInfo.VersionMajor, wirelessInfo.VersionMinor, wirelessInfo.VersionSub);
  }
  else
  {
    TRACECPU1("** ERR_ZIGBEE : ErroCode= ERROR Unknown \r\n");
  }
}

void Tzd_stm32::processNotifyM0(void)
{
  if (CptReceiveNotifyFromM0 != 0) {
    /* If CptReceiveNotifyFromM0 is > 1. it means that we did not serve all the events from the radio */
    if (CptReceiveNotifyFromM0 > 1U) {
        TRACECPU1("** ERR_ZIGBEE : Multiple massages from M0 \r\n");
    }
    else {
        Zigbee_CallBackProcessing();
    }
    /* Reset counter */
    CptReceiveNotifyFromM0 = 0;
  }

  if(CptReceiveRequestFromM0 == 0)
    evtSyncBypassIdle = true;
}

void Tzd_stm32::processRequestM0(void)
{
  if (CptReceiveRequestFromM0 != 0) {
      Zigbee_M0RequestProcessing();
      CptReceiveRequestFromM0 = 0;
  }

  if(CptReceiveNotifyFromM0 == 0)
    evtSyncBypassIdle = true;
}

void Tzd_stm32::formNetwork(void)
{
  if(!stackConfigDone)
  {
    TRACECPU1("** ERR_ZIGBEE : stack not configured \r\n");
    return;
  }

  if(joined)
  {
    return;
  }

  if(wasJoined)
  {
    // try a rejoin
    // todo: how we get the information that we lost the connection?

  }
  else
  {
    // join or form network

    // look for persistent startup data
    uint32_t len = 0;
    uint8_t *buf = 0;

    if(persistentDataCb.pObjRead != 0 && persistentDataCb.pMFuncRead != 0)
    {
       (persistentDataCb.pObjRead->*persistentDataCb.pMFuncRead)(len, buf);
    }

    if(persistentDataCb.pObjRead == 0 && persistentDataCb.pFuncRead != 0)
    {
       persistentDataCb.pFuncRead(len, buf);
    }

    if(len > 0)
    {
      // Persistent startup data available
      enum ZbStatusCodeT status = ZbStartupPersist(zb, (const void*) buf, len, 0, cbStartupDone, 0);

      if(status != ZB_STATUS_SUCCESS)
      {
        cbStartupDone(status, 0);
      }
    }
    else
    {
      // startup without persistent startup data

      struct ZbStartupT config;

      // get default configuration for zigbee 3.0 (zigbee pro)
      ZbStartupConfigGetProDefaults(&config);

      // configure channel list
      config.channelList.list[0].page = 0;
      config.channelList.list[0].channelMask = priChMsk;

      if(priChMsk != 0)
      {
        config.channelList.list[1].page = 0;
        config.channelList.list[1].channelMask = secChMsk;
        config.channelList.count = 2;
      }
      else
      {
        config.channelList.count = 1;
      }

      switch(deviceType)
      {
      case DT_Coordinator:
        if(ffdStackFound)
        {
          config.startupControl = ZbStartTypeForm;
        }
        else
        {
          TRACECPU1("** ERR_ZIGBEE : Wrong Stack installed \r\n");
          cbStartupDone(ZB_NWK_STATUS_STARTUP_FAILURE, 0);
          return;
        }
        break;

      case DT_Router:
        if(ffdStackFound)
        {
          config.startupControl = ZbStartTypeJoin;
        }
        else
        {
          TRACECPU1("** ERR_ZIGBEE : Wrong Stack installed \r\n");
          cbStartupDone(ZB_NWK_STATUS_STARTUP_FAILURE, 0);
          return;
        }
        break;

      case DT_Enddevice:
        if(rfdStackFound)
        {
          config.startupControl = ZbStartTypeJoin;
          config.capability &= ~(MCP_ASSOC_CAP_RXONIDLE | MCP_ASSOC_CAP_DEV_TYPE | MCP_ASSOC_CAP_ALT_COORD);
          config.endDeviceTimeout = endDeviceTimeout;
        }
        else
        {
          TRACECPU1("** ERR_ZIGBEE : Wrong Stack installed \r\n");
          cbStartupDone(ZB_NWK_STATUS_STARTUP_FAILURE, 0);
          return;
        }
        break;
      }

      enum ZbStatusCodeT status = ZbStartup(zb, &config, cbStartupDone, 0);
      if(status != ZB_STATUS_SUCCESS)
      {
        cbStartupDone(status, 0);
      }
    }
  }
}

bool Tzd_stm32::config()
{
  if(!stackInitDone and stackConfigDone)
  {
    return false;
  }

  if(endpoints == 0)
  {
    return false;
  }

  // register callback to store persistent data
  if( (persistentDataCb.pObjWrite != 0 && persistentDataCb.pMFuncWrite != 0) ||
      (persistentDataCb.pObjWrite == 0 && persistentDataCb.pFuncWrite != 0))
  {
    ZbPersistNotifyRegister(zb, cbStoreData, 0);
  }

  // setup basic cluster attributes
  struct ZbZclBasicServerDefaults basicCluster;
  basicCluster.app_version = attrAppVersion;
  basicCluster.hw_version = attrHWVersion;
  basicCluster.stack_version = attrStackVersion;
  basicCluster.power_source = attrPowerSource;

  if(attrManufacturaName != 0)
  {
    uint8_t len = strlen((const char*)attrManufacturaName);
    if(len > 32)
      len = 32;
    basicCluster.mfr_name[0] = len;
    memcpy(&basicCluster.mfr_name[1], attrManufacturaName, len);
  }
  else
  {
    basicCluster.mfr_name[0] = 0;
  }

  if(attrModelIdentifier != 0)
  {
    uint8_t len = strlen((const char*)attrModelIdentifier);
    if(len > 32)
      len = 32;
    basicCluster.model_name[0] = len;
    memcpy(&basicCluster.model_name[1], attrModelIdentifier, len);
  }
  else
  {
    basicCluster.mfr_name[0] = 0;
  }

  if(attrSWBuildId != 0)
  {
    uint8_t len = strlen((const char*)attrSWBuildId);
    if(len > 16)
      len = 16;
    basicCluster.sw_build_id[0] = len;
    memcpy(&basicCluster.sw_build_id[1], attrSWBuildId, len);
  }
  else
  {
    basicCluster.mfr_name[0] = 0;
  }

  if(attrDateCode != 0)
  {
    uint8_t len = strlen((const char*)attrDateCode);
    if(len > 16)
      len = 16;
    basicCluster.date_code[0] = len;
    memcpy(&basicCluster.date_code[1], attrDateCode, len);
  }
  else
  {
    basicCluster.mfr_name[0] = 0;
  }

  ZbZclBasicServerConfigDefaults(zb, &basicCluster);


  // call setup routines for endpoints and cluster
  Tze_stm32wb* aktEp = (Tze_stm32wb*) endpoints;
  while(aktEp != 0)
  {
    aktEp->init();
    aktEp = (Tze_stm32wb*) aktEp->getNext();
  }

  stackConfigDone = true;

  return true;
}

bool Tzd_stm32::join()
{
  joinTimeOut = false;
  if(wasJoined)
  {
    if(rejoinTimeout != -1)
    {
      ts->start(tsTimeOut, rejoinTimeout);
    }
  }
  else
  {
    ts->start(tsTimeOut, joinTimeout);
  }

  seq->queueTask(seqIdNetworkForm);

  return true;
};

void Tzd_stm32::sysEvtHandler()
{
  TRACECPU1("HWIPCC sys event recieved\r\n");

  HW_IPCC_SYS_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_SYSTEM_EVENT_CHANNEL);
}

void Tzd_stm32::sysCmdEvtHandler()
{
  TRACECPU1("HWIPCC sys cmd event recieved\r\n");

  ipcc.disableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
  HW_IPCC_SYS_CmdEvtNot();
}

void Tzd_stm32::stackNotifEvtHandler()
{
  TRACECPU1("HWIPCC stack notify recieved\r\n");

  ipcc.disableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);
  HW_IPCC_ZIGBEE_RecvM0NotifyToM4();
}

void Tzd_stm32::stackM0RequestHandler()
{
  TRACECPU1("HWIPCC stack request recieved\r\n");

  ipcc.disableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);
  HW_IPCC_ZIGBEE_RecvM0RequestToM4();
}

void Tzd_stm32::traceEvtHandler()
{
  HW_IPCC_TRACES_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_TRACES_CHANNEL);
}

void Tzd_stm32::mmFreeBufHandler()
{
  TRACECPU1("HWIPCC free buf recieved\r\n");

  ipcc.disableTxChannel(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
  FreeBufCb();
  ipcc.setTxFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
}

void Tzd_stm32::cmdEvtHandler()
{
  TRACECPU1("HWIPCC cmd recieved\r\n");

  ipcc.disableTxChannel(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL);
  HW_IPCC_ZIGBEE_RecvAppliAckFromM0();
}

void Tzd_stm32::tracesEvtHandler()
{
  HW_IPCC_TRACES_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_TRACES_CHANNEL);
}

void Tzd_stm32::cmdTransfer()
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)OtCmdBuffer.cmdserial.cmd.payload;

  OtCmdBuffer.cmdserial.cmd.cmdcode = 0x280U;
  OtCmdBuffer.cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  seq->waitForEvent(&evtAckFromM0);
  evtAckFromM0 = false;
}

void Tzd_stm32::storePersistentData()
{
  // get length
  uint32_t len;

  len = ZbPersistGet(zb, 0, 0);

  uint8_t* buf = (uint8_t*) malloc(len);

  if(buf == 0)
  {
    TRACECPU1("** ERR_ZIGBEE : insufficient memory for persistent data backup \r\n");
    return;
  }

  ZbPersistGet(zb, buf, len);

  if(persistentDataCb.pObjWrite != 0 && persistentDataCb.pMFuncWrite != 0)
  {
     (persistentDataCb.pObjWrite->*persistentDataCb.pMFuncWrite)(len, buf);
  }

  if(persistentDataCb.pObjWrite == 0 && persistentDataCb.pFuncWrite != 0)
  {
     persistentDataCb.pFuncWrite(len, buf);
  }

  free(buf);
}
