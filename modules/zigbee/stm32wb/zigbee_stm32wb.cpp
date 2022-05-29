/*
 * zigbee_stm32wb.cpp
 *
 *  Created on: 09.04.2022
 *      Author: Bergi
 */

#include "zigbee_stm32wb.h"

#include "hw.h"

#include "tl.h"
#include "tl_zigbee_hci.h"
#include "zigbee_interface.h"

#include "shci.h"

TZigbee_stm32wb gZigbee;

// static member variables definitions
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t TZigbee_stm32wb::EvtPool[POOL_SIZE];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) TL_CmdPacket_t TZigbee_stm32wb::SystemCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t TZigbee_stm32wb::SystemSpareEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) TL_CmdPacket_t TZigbee_stm32wb::OtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t TZigbee_stm32wb::NotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t TZigbee_stm32wb::NotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];



extern "C" void IRQ_Handler_44() {gZigbee.ipcc.rxIrqHandler();}
extern "C" void IRQ_Handler_45() {gZigbee.ipcc.txIrqHandler();}

/**********************************************/
/*          hw.h interface functions          */
/**********************************************/

// todo: find all gZigbee.ipcc.enableTxChannel an put the callback configuration into init function
extern "C" {
  void HW_IPCC_Init()
  {
    gZigbee.ipcc.init();
    gZigbee.ipcc.setRxCb(HW_IPCC_TRACES_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::tracesEvtHandler);
  }

  void HW_IPCC_Enable( void )
  {
    gZigbee.pwr->startCPU2();
  }

  void HW_IPCC_SYS_Init()
  {
    gZigbee.ipcc.enableRxChannel(HW_IPCC_SYSTEM_EVENT_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::sysEvtHandler);
  }

  void HW_IPCC_SYS_SendCmd()
  {
    gZigbee.ipcc.setTxFlag(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
    gZigbee.ipcc.enableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::sysCmdEvtHandler);
  }

  void HW_IPCC_ZIGBEE_Init()
  {
    gZigbee.ipcc.enableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::stackNotifEvtHandler);
    gZigbee.ipcc.enableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::stackM0RequestHandler);
  }

  void HW_IPCC_ZIGBEE_SendM4RequestToM0( void )
  {
    gZigbee.ipcc.setTxFlag(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL);
    gZigbee.ipcc.enableTxChannel(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::cmdEvtHandler);

    return;
  }

  void HW_IPCC_ZIGBEE_SendM4AckToM0Notify( void )
  {
    gZigbee.ipcc.clearRxFlag(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);
    // todo: normally only enable is necessary because function already set in HW_IPCC_ZIGBEE_Init
    gZigbee.ipcc.enableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::stackNotifEvtHandler);
  }

  void HW_IPCC_ZIGBEE_SendM4AckToM0Request( void )
  {
    gZigbee.ipcc.clearRxFlag(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);
    // todo: normally only enable is necessary because function already set in HW_IPCC_ZIGBEE_Init
    gZigbee.ipcc.enableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::stackM0RequestHandler);
  }

  void HW_IPCC_MM_SendFreeBuf( void (*cb)( void ) )
  {
    if(gZigbee.ipcc.getTxActivFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL))
    {
      gZigbee.FreeBufCb = cb;
      gZigbee.ipcc.enableTxChannel(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::mmFreeBufHandler);
    }
    else
    {
      cb();
      gZigbee.ipcc.setTxFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
    }
  }

  void HW_IPCC_TRACES_Init()
  {
    gZigbee.ipcc.enableRxChannel(HW_IPCC_TRACES_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::traceEvtHandler);

  }

  void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
  {
    gZigbee.p_ZIGBEE_request_M0_to_M4 = Reqbuffer;
    gZigbee.CptReceiveRequestFromM0++;
    gZigbee.seq->queueTask(gZigbee.seqIdReqFromM0);
  }

  void TL_ZIGBEE_CmdEvtReceived(TL_EvtPacket_t *Otbuffer)
  {
    gZigbee.evtAckFromM0 = true;
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
  gZigbee.sysStatusNot(status);
}

void cbSysUserEvtRx( void * pPayload )
{
  gZigbee.sysUserEvtRx(pPayload);
}

void shci_notify_asynch_evt(void* pdata)
{
  gZigbee.seq->queueTask(gZigbee.seqIdShciAsync);
}

void shci_cmd_resp_release(uint32_t flag)
{
  gZigbee.evtShciCmdResp = true;
  return;
}

void shci_cmd_resp_wait(uint32_t timeout)
{
  gZigbee.seq->waitForEvent(&gZigbee.evtShciCmdResp);
  return;
}


/**********************************************/
/*   interface functions for zigbee core      */
/**********************************************/
extern "C" {
  void Pre_ZigbeeCmdProcessing(void)
  {
    // check if there are no pending requests or
    // notifications before send a new command

    gZigbee.seq->queueTask(gZigbee.seqIdNotFromM0);
    gZigbee.seq->queueTask(gZigbee.seqIdReqFromM0);
    gZigbee.evtSyncBypassIdle = false;
    gZigbee.seq->waitForEvent(&gZigbee.evtSyncBypassIdle);
  }

  void ZIGBEE_CmdTransfer(void)
  {
    gZigbee.cmdTransfer();
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_OTCmdPayloadBuffer(void)
  {
    return (Zigbee_Cmd_Request_t *)gZigbee.OtCmdBuffer.cmdserial.cmd.payload;
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_OTCmdRspPayloadBuffer(void)
  {
    TL_EvtPacket_t * pkt = (TL_EvtPacket_t *) &gZigbee.OtCmdBuffer;
    return (Zigbee_Cmd_Request_t *)pkt->evtserial.evt.payload;
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_NotificationPayloadBuffer(void)
  {
    return (Zigbee_Cmd_Request_t *)(gZigbee.notFromM0)->evtserial.evt.payload;
  }

  Zigbee_Cmd_Request_t* ZIGBEE_Get_M0RequestPayloadBuffer(void)
  {
    return (Zigbee_Cmd_Request_t *)(gZigbee.reqFromM0)->evtserial.evt.payload;
  }
}

/**********************************************/
/*          zigbee class definition           */
/**********************************************/
TZigbee_stm32wb::TZigbee_stm32wb()
{
  stackInitDone = false;
  stackConfigDone = false;
}

bool TZigbee_stm32wb::init(TSequencer* aSeq, THwPwr* aPwr)
{
  CptReceiveRequestFromM0 = 0;
  CptReceiveNotifyFromM0 = 0;

  evtAckFromM0 = false;
  evtShciCmdResp = false;

  seq = aSeq;
  pwr = aPwr;
  sema.init();

  TL_Init();


  seq->addTask(seqIdShciAsync, shci_user_evt_proc);
  seq->addTask(seqIdNotFromM0, this, (void (TCbClass::*)(void)) &TZigbee_stm32wb::processNotifyM0);
  seq->addTask(seqIdReqFromM0, this, (void (TCbClass::*)(void)) &TZigbee_stm32wb::processRequestM0);
  seq->addTask(seqIdNetworkForm, this, (void (TCbClass::*)(void)) &TZigbee_stm32wb::formNetwork);
  seq->addTask(seqIdZigbeeAppStart, this, (void (TCbClass::*)(void)) &TZigbee_stm32wb::appStart);

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

void TZigbee_stm32wb::sysStatusNot( SHCI_TL_CmdStatus_t status )
{
  // in ST Example this function is empty
}

void TZigbee_stm32wb::sysUserEvtRx(void *pPayload)
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
      TRACECPU2("** ERR_ZIGBEE : UNKNOWN_CMD \n");
      break;
    default:
      TRACECPU2("** ERR_ZIGBEE : ErroCode=%d \n",errorCode);
      break;
    }
  }
    break;
  default:
    break;
  }
}

void TZigbee_stm32wb::initStack()
{
  TRACECPU1("init wireless stack\r\n")

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

  TRACECPU1("init of wireless stack done\r\n")

  stackInitDone = true;
}

void TZigbee_stm32wb::checkWirelessFwInfo()
{
  WirelessFwInfo_t wirelessInfo;

  SHCI_CmdStatus_t status;
  status = SHCI_GetWirelessFwInfo(&wirelessInfo);

  if(status == SHCI_Success)
  {
    switch (wirelessInfo.StackType) {
    case INFO_STACK_TYPE_ZIGBEE_FFD:
      TRACECPU1("Wireless FW Type : FFD Zigbee stack\r\n");
      break;
    case INFO_STACK_TYPE_ZIGBEE_RFD:
      TRACECPU1("Wireless FW Type : RFD Zigbee stack\r\n");
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

void TZigbee_stm32wb::processNotifyM0(void)
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

void TZigbee_stm32wb::processRequestM0(void)
{
  if (CptReceiveRequestFromM0 != 0) {
      Zigbee_M0RequestProcessing();
      CptReceiveRequestFromM0 = 0;
  }

  if(CptReceiveNotifyFromM0 == 0)
    evtSyncBypassIdle = true;
}

bool TZigbee_stm32wb::formNetwork(void)
{
  if(!stackConfigDone)
  {
    return false;
  }
  // todo: handle here network forming and joining see: APP_ZIGBEE_NwkForm()
}

bool TZigbee_stm32wb::configStack(void)
{
  if(!stackInitDone)
  {
    return false;
  }
  // todo: init cluster, endpoints etc. see: APP_ZIGBEE_App_Init()
}

void TZigbee_stm32wb::sysEvtHandler()
{
  HW_IPCC_SYS_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_SYSTEM_EVENT_CHANNEL);
}

void TZigbee_stm32wb::sysCmdEvtHandler()
{
  ipcc.disableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
  HW_IPCC_SYS_CmdEvtNot();
}

void TZigbee_stm32wb::stackNotifEvtHandler()
{
  ipcc.disableRxChannel(HW_IPCC_ZIGBEE_APPLI_NOTIF_ACK_CHANNEL);
  HW_IPCC_ZIGBEE_RecvM0NotifyToM4();
}

void TZigbee_stm32wb::stackM0RequestHandler()
{
  ipcc.disableRxChannel(HW_IPCC_ZIGBEE_M0_REQUEST_CHANNEL);
  HW_IPCC_ZIGBEE_RecvM0RequestToM4();
}

void TZigbee_stm32wb::traceEvtHandler()
{
  HW_IPCC_TRACES_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_TRACES_CHANNEL);
}

void TZigbee_stm32wb::mmFreeBufHandler()
{
  ipcc.disableTxChannel(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
  FreeBufCb();
  ipcc.setTxFlag(HW_IPCC_MM_RELEASE_BUFFER_CHANNEL);
}

void TZigbee_stm32wb::cmdEvtHandler()
{
  ipcc.disableTxChannel(HW_IPCC_ZIGBEE_CMD_APPLI_CHANNEL);
  HW_IPCC_ZIGBEE_RecvAppliAckFromM0();
}

void TZigbee_stm32wb::tracesEvtHandler()
{
  HW_IPCC_TRACES_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_TRACES_CHANNEL);
}

void TZigbee_stm32wb::cmdTransfer()
{
  Zigbee_Cmd_Request_t *cmd_req = (Zigbee_Cmd_Request_t *)OtCmdBuffer.cmdserial.cmd.payload;

  OtCmdBuffer.cmdserial.cmd.cmdcode = 0x280U;
  OtCmdBuffer.cmdserial.cmd.plen = 8U + (cmd_req->Size * 4U);

  TL_ZIGBEE_SendM4RequestToM0();

  seq->waitForEvent(&evtAckFromM0);
}
