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

constexpr uint8_t Tzd_stm32::nvm[4096] __attribute__ ((aligned (4096)));

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
#ifdef debug
    TRACECPU1("HWIPCC init\r\n");
#endif
    pZigbeeInt->ipcc.init();
    pZigbeeInt->ipcc.setRxCb(HW_IPCC_TRACES_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::tracesEvtHandler);
    pZigbeeInt->ipcc.setRxCb(HW_IPCC_SYSTEM_EVENT_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::sysEvtHandler);
    pZigbeeInt->ipcc.setRxCb(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackNotifEvtHandler);
    pZigbeeInt->ipcc.setRxCb(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackM0RequestHandler);
    pZigbeeInt->ipcc.setRxCb(HW_IPCC_TRACES_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::traceEvtHandler);

    pZigbeeInt->ipcc.setTxCb(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::sysCmdEvtHandler);
    pZigbeeInt->ipcc.setTxCb(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::cmdEvtHandler);
    pZigbeeInt->ipcc.setTxCb(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::mmFreeBufHandler);

  }

  void HW_IPCC_Enable( void )
  {
#ifdef debug
    TRACECPU1("HWIPCC enable\r\n");
#endif
    pZigbeeInt->pwr->startCPU2();
  }

  void HW_IPCC_SYS_Init()
  {
#ifdef debug
    TRACECPU1("HWIPCC sys init\r\n");
#endif
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_SYSTEM_EVENT_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::sysEvtHandler);
  }

  void HW_IPCC_SYS_SendCmd()
  {
#ifdef debug
    TRACECPU1("HWIPCC sys send cmd\r\n");
#endif
    pZigbeeInt->ipcc.setTxFlag(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
    pZigbeeInt->ipcc.enableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::sysCmdEvtHandler);
  }

  void HW_IPCC_ZIGBEE_Init()
  {
#ifdef debug
    TRACECPU1("HWIPCC zigbee init\r\n");
#endif
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackNotifEvtHandler);
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackM0RequestHandler);
  }

  void HW_IPCC_ZIGBEE_SendM4RequestToM0( void )
  {
#ifdef debug
    TRACECPU1("HWIPCC request to M0\r\n");
#endif
    pZigbeeInt->ipcc.setTxFlag(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL);
    pZigbeeInt->ipcc.enableTxChannel(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::cmdEvtHandler);

    return;
  }

  void HW_IPCC_ZIGBEE_SendM4AckToM0Notify( void )
  {
#ifdef debug
    TRACECPU1("HWIPCC notify ack to M0\r\n");
#endif
    pZigbeeInt->ipcc.clearRxFlag(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);
    // todo: normally only enable is necessary because function already set in HW_IPCC_ZIGBEE_Init
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackNotifEvtHandler);
  }

  void HW_IPCC_ZIGBEE_SendM4AckToM0Request( void )
  {
#ifdef debug
    TRACECPU1("HWIPCC request ack to M0\r\n");
#endif
    pZigbeeInt->ipcc.clearRxFlag(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);
    // todo: normally only enable is necessary because function already set in HW_IPCC_ZIGBEE_Init
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::stackM0RequestHandler);
  }

  void HW_IPCC_MM_SendFreeBuf( void (*cb)( void ) )
  {
#ifdef debug
    TRACECPU1("HWIPCC send free buf\r\n");
#endif
    if(pZigbeeInt->ipcc.getTxActivFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL))
    {
      pZigbeeInt->FreeBufCb = cb;
      pZigbeeInt->ipcc.enableTxChannel(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::mmFreeBufHandler);
    }
    else
    {
      cb();
      pZigbeeInt->ipcc.setTxFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
    }
  }

  void HW_IPCC_TRACES_Init()
  {
    pZigbeeInt->ipcc.enableRxChannel(HW_IPCC_TRACES_CHANNEL);//, pZigbeeInt, (void (TCbClass::*)()) &Tzd_stm32::traceEvtHandler);

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
    gTrace.printBuf(gTrace.TA_CPU2, (const char *) ((TL_AsynchEvt_t *)(hcievt->evtserial.evt.payload))->payload, (uint32_t)hcievt->evtserial.evt.plen - 2U, false);
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
  pZigbeeInt->storePersistentData();
}

void cbStartupDone(enum ZbStatusCodeT status, void *arg)
{
  if(status == ZB_STATUS_SUCCESS)
  {
    TRACECPU1("joined a network\r\n");

    // connection was successful
    pZigbeeInt->flagJoined = true;
    pZigbeeInt->flagWasJoined = true;
    pZigbeeInt->ts->stop(pZigbeeInt->tsIdTimeOut);
  }
  else
  {
    if(pZigbeeInt->flagJoinTimeout)
    {
      // no further join attempt
      TRACECPU1("joining timeout\r\n");
      return;
    }
    else
    {
      // queue a further join attempt
      if(pZigbeeInt->flagWasJoined)
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

void cbRejoin(struct ZbNlmeJoinConfT *conf, void *arg)
{
  cbStartupDone(conf->status, 0);
}

void stackLog(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argptr)
{
  gTrace.vprintf_rn(TTrace::TA_CPU1, fmt, argptr);
}

/**********************************************/
/*   interface functions for zigbee core      */
/**********************************************/
extern "C" {
  void Pre_ZigbeeCmdProcessing(void)
  {
    // check if there are no pending requests or
    // notifications before send a new command

    // check if the aktive task handles a Notification
    if(pZigbeeInt->seq->getAktivTask() != pZigbeeInt->seqIdNotFromM0)
    {
      pZigbeeInt->evtSyncBypassNotIdle = false;
      pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdNotFromM0);
      pZigbeeInt->seq->waitForEvent(&pZigbeeInt->evtSyncBypassNotIdle);
    }

    if(pZigbeeInt->seq->getAktivTask() != pZigbeeInt->seqIdReqFromM0)
    {
      pZigbeeInt->evtSyncBypassReqIdle = false;
      pZigbeeInt->seq->queueTask(pZigbeeInt->seqIdReqFromM0);
      pZigbeeInt->seq->waitForEvent(&pZigbeeInt->evtSyncBypassReqIdle);
    }
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

  flagStackInitDone = false;
  flagStackConfigDone = false;
  flagWasJoined = false;
  flagAvoidPersistentStartup = false;

  joinTimeout = 60000;
  joinRetryInterval = 100;

  rejoinTimeout = -1;
  rejoinRetryInterval = 60000;

  // check if this is the only instance of this class
  if(pZigbeeInt != 0)
    while(1);

  pZigbeeInt = this;
}

bool Tzd_stm32::init(TSequencer* aSeq, THwPwr* aPwr, TTimerServer* aTs, THwIntFlash* aFlash)
{
  CptReceiveRequestFromM0 = 0;
  CptReceiveNotifyFromM0 = 0;

  evtAckFromM0 = false;
  evtSyncBypassNotIdle = false;
  evtSyncBypassReqIdle = false;
  evtShciCmdResp = false;
  evtStartupDone = false;

  seq = aSeq;
  pwr = aPwr;
  flash = aFlash;
  ts = aTs;
  sema.init();

  TL_Init();

  ts->create(tsIdJoin, this, (void (TCbClass::*)(THwRtc::time_t time)) &Tzd_stm32::retryJoin, false);
  ts->create(tsIdTimeOut, this, (void (TCbClass::*)(THwRtc::time_t time)) &Tzd_stm32::signalTimeout, false);


  seq->addTask(seqIdShciAsync, shci_user_evt_proc);
  seq->addTask(seqIdNotFromM0, this, (void (TCbClass::*)(void)) &Tzd_stm32::taskProcessNotifyM0);
  seq->addTask(seqIdReqFromM0, this, (void (TCbClass::*)(void)) &Tzd_stm32::taskProcessRequestM0);
  seq->addTask(seqIdNetworkForm, this, (void (TCbClass::*)(void)) &Tzd_stm32::taskFormNetwork);

/*
  * According AN5500 we need to call ZbTimerWork and WpanMacWork
  * on regular basis but st don't use this functions in any of there
  * examples and WpanMacWork don't exist and until now it works also without
  *

  ts->create(tsIdStackWork, this, (void (TCbClass::*)(THwRtc::time_t time)) &Tzd_stm32::queueStackWork, false);
  ts->create(tsIdMacWork, this, (void (TCbClass::*)(THwRtc::time_t time)) &Tzd_stm32::queueMacWork, false);

  seq->addTask(seqIdStackWork, this, (void (TCbClass::*)(void)) &Tzd_stm32::serviceStack);
  seq->addTask(seqIdMacWork, this, (void (TCbClass::*)(void)) &Tzd_stm32::serviceMac);
*/

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

  flagStackInitDone = true;
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

void Tzd_stm32::taskProcessNotifyM0(void)
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

  evtSyncBypassNotIdle = true;
}

void Tzd_stm32::taskProcessRequestM0(void)
{
  if (CptReceiveRequestFromM0 != 0) {
    if(CptReceiveRequestFromM0 > 1)
    {
      TRACECPU1("** ERR_ZIGBEE : Multiple massages from M0 \r\n");
    }

    Zigbee_M0RequestProcessing();
    CptReceiveRequestFromM0 = 0;
  }

  evtSyncBypassReqIdle = true;
}

void Tzd_stm32::taskFormNetwork(void)
{
  if(!flagStackConfigDone)
  {
    TRACECPU1("** ERR_ZIGBEE : stack not configured \r\n");
    return;
  }

  if(flagJoined)
  {
    return;
  }

  if(flagWasJoined)
  {
    // try a rejoin
    enum ZbStatusCodeT status = ZbStartupRejoin(zb, cbRejoin, 0);

    if(status != ZB_STATUS_SUCCESS)
    {
      while(1);
    }
  }
  else
  {
    // join or form network
    if(flagDoReset)
    {
      TRACECPU1("reset connection data\r\n");
      ZbReset(zb);
      flagDoReset = false;
    }

    // look for persistent startup data
    uint32_t len;

    if(checkPersistentData((uint8_t*)nvm, nvmSize, len) && !flagAvoidPersistentStartup)
    {
      TRACECPU1("startup with persistent data\r\n");

      /* Make sure the EPID is cleared, before we are allowed to restore persistence */
      uint64_t epid = 0U;
      ZbNwkSet(zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));

      // Persistent startup data available
      enum ZbStatusCodeT status = ZbStartupPersist(zb, (const void*) &nvm[4], len, 0, cbStartupDone, 0);

      if(status != ZB_STATUS_SUCCESS)
      {
        cbStartupDone(status, 0);
      }
    }
    else
    {
      TRACECPU1("startup without persistent data\r\n");

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
  if(!flagStackInitDone and flagStackConfigDone)
  {
    return false;
  }

  if(endpoints == 0)
  {
    return false;
  }

  // register callback to store persistent data
  ZbPersistNotifyRegister(zb, cbStoreData, 0);

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

  ZbSetLogging(zb, ZB_LOG_MASK_LEVEL_5, stackLog);

  flagStackConfigDone = true;

  return true;
}

bool Tzd_stm32::startup()
{
  flagJoinTimeout = false;
  if(flagWasJoined)
  {
    if(rejoinTimeout != -1)
    {
      ts->start(tsIdTimeOut, rejoinTimeout);
    }
  }
  else
  {
    ts->start(tsIdTimeOut, joinTimeout);
  }

  seq->queueTask(seqIdNetworkForm);

  return true;
};

void Tzd_stm32::sysEvtHandler()
{
#ifdef debug
  TRACECPU1("HWIPCC sys event recieved\r\n");
#endif
  HW_IPCC_SYS_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_SYSTEM_EVENT_CHANNEL);
}

void Tzd_stm32::sysCmdEvtHandler()
{
#ifdef debug
  TRACECPU1("HWIPCC sys cmd event recieved\r\n");
#endif
  ipcc.disableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
  HW_IPCC_SYS_CmdEvtNot();
}

void Tzd_stm32::stackNotifEvtHandler()
{
#ifdef debug
  TRACECPU1("HWIPCC stack notify recieved\r\n");
#endif
  ipcc.disableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);
  HW_IPCC_ZIGBEE_RecvM0NotifyToM4();
}

void Tzd_stm32::stackM0RequestHandler()
{
#ifdef debug
  TRACECPU1("HWIPCC stack request recieved\r\n");
#endif
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
#ifdef debug
  TRACECPU1("HWIPCC free buf recieved\r\n");
#endif
  ipcc.disableTxChannel(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
  FreeBufCb();
  ipcc.setTxFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
}

void Tzd_stm32::cmdEvtHandler()
{
#ifdef debug
  TRACECPU1("HWIPCC cmd recieved\r\n");
#endif
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
  // todo: the stack requests every 5 minutes store data
  // so wear leveling is needed

  // get length
  uint32_t len = ZbPersistGet(zb, 0, 0);
  uint32_t len8BAl = ((len + 7UL) / 8UL) * 8UL; // 8Byte aligned length
  uint32_t lenFlash = len8BAl + 8;

  TRACECPU1("Store %iByte persistent data\r\n", len);

  if(!flagJoined)
  {
    TRACECPU1("Not joined, skip store of persistent data\r\n", len);
    return;
  }

  if(lenFlash > nvmSize)
  {
    TRACECPU1("** ERR_ZIGBEE : insufficient nvm for persistent data backup \r\n");
    return;
  }

  // we allocate additional 4Byte at the beginning for length
  // and 4 Byte for crc at the end
  uint8_t* buf = (uint8_t*) malloc(lenFlash);

  if(buf == 0)
  {
    TRACECPU1("** ERR_ZIGBEE : insufficient memory for persistent data backup \r\n");
    return;
  }

  *((uint32_t*) &buf[0]) = len;

  // set the end buffer to defined value, because they are maybe not used
  *((uint32_t*) &buf[len8BAl-4]) = 0;
  *((uint32_t*) &buf[len8BAl]) = 0;

  uint32_t storeLen = ZbPersistGet(zb, &buf[4], len);

  if(storeLen != len)
  {
    TRACECPU1("** ERR_ZIGBEE : inconsistent size \r\n");
    free(buf);
    return;
  }

  // todo: we use here the crc unit without driver

  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
  CRC->INIT = 0xFFFFFFFF;
  CRC->POL = 0x04C11DB7;
  CRC->CR = CRC_CR_RESET;

  // calculate CRC over length and data
  for(uint32_t i = 0; i < (len8BAl >> 2) + 1; i++)
  {
    CRC->DR = ((uint32_t*)buf)[i];
  }

  *((uint32_t*) &buf[len8BAl+4]) = CRC->DR;

  RCC->AHB1ENR &= ~RCC_AHB1ENR_CRCEN;

  // according AN5289 the flash access must coordinated with CPU2
  // but there is no coordination in the ST examples and maybe it is only
  // necessary for BLE and Thread

  flash->StartEraseMem((uint32_t)&nvm, lenFlash);
  flash->WaitForComplete();
  flash->StartWriteMem((uint32_t)&nvm, (void*) buf, lenFlash);
  flash->WaitForComplete();

  free(buf);
}

bool Tzd_stm32::checkPersistentData(uint8_t* buf, uint32_t aLen, uint32_t &dataLen)
{
  if(buf == 0)
    return false;

  uint32_t len = *((uint32_t*) &buf[0]);
  uint32_t len8BAl = ((len + 7UL) / 8UL) * 8UL; // 8Byte aligned length

  if(len8BAl+8 > aLen || len == 0)
  {
    return false;
  }

  RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
  CRC->INIT = 0xFFFFFFFF;
  CRC->POL = 0x04C11DB7;
  CRC->CR = CRC_CR_RESET;

  // calculate CRC over length and data
  for(uint32_t i = 0; i < (len8BAl >> 2) + 1; i++)
  {
    CRC->DR = ((uint32_t*)buf)[i];
  }

  uint32_t crc = CRC->DR;

  RCC->AHB1ENR &= ~RCC_AHB1ENR_CRCEN;

  if(crc == *((uint32_t*) &buf[len8BAl+4]))
  {
    dataLen = len;
    return true;
  }
  else
  {
    dataLen = 0;
    return false;
  }
}

bool Tzd_stm32::joinNewNwk()
{
  TCriticalSection sec(true);

  flagWasJoined = false;
  flagJoined = false;
  flagAvoidPersistentStartup = true;
  flagDoReset = true;

  sec.leave();

  return startup();
}

/*
 * According AN5500 we need to call ZbTimerWork and WpanMacWork
 * on regular basis but st don't use this functions in any of there
 * examples and WpanMacWork don't exist and until now it works also without
 *

void Tzd_stm32::serviceStack(void)
{
  ZbTimerWork(zb);
  ts->start(tsIdStackWork, ZbCheckTime(zb));
}

void Tzd_stm32::serviceMac(void)
{
  WpanMacWork(zb);
  ts->start(tsIdMacWork, WpanMacCheck(zb));
}
*/
