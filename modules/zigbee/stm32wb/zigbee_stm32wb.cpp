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
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) TL_CmdPacket_t TZigbee_stm32wb::SystemCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) TL_CmdPacket_t TZigbee_stm32wb::OtCmdBuffer;
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t TZigbee_stm32wb::NotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
PLACE_IN_SECTION("MB_MEM2") ALIGN(4) uint8_t TZigbee_stm32wb::NotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];


// hw.h interface definitions
extern "C" {
  void HW_IPCC_Init()
  {gZigbee.ipcc.init();}

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

  void HW_IPCC_TRACES_Init()
  {
    gZigbee.ipcc.enableRxChannel(HW_IPCC_TRACES_CHANNEL, &gZigbee, (void (TCbClass::*)()) &TZigbee_stm32wb::traceEvtHandler);

  }

  void TL_ZIGBEE_M0RequestReceived(TL_EvtPacket_t *Reqbuffer)
  {
    gZigbee.p_ZIGBEE_request_M0_to_M4 = Reqbuffer;
    gZigbee.CptReceiveRequestFromM0++;
    gZigbee.evtReqFromM0 = true;
  }

  void cbSysStatusNot( SHCI_TL_CmdStatus_t status )
  {gZigbee.sysStatusNot(status);}

  void cbSysUserEvtRx( void * pPayload )
  {gZigbee.sysUserEvtRx(pPayload);}

  void shci_notify_asynch_evt(void* pdata)
  {
    gZigbee.evtShciAsync = true;
  }
}

bool TZigbee_stm32wb::init()
{
  CptReceiveRequestFromM0 = 0;
  CptReceiveNotifyFromM0 = 0;

  TL_Init();

  SHCI_TL_HciInitConf_t SHci_Tl_Init_Conf;
  SHci_Tl_Init_Conf.p_cmdbuffer = (uint8_t*)&SystemCmdBuffer;
  SHci_Tl_Init_Conf.StatusNotCallBack = cbSysStatusNot;
  shci_init(cbSysUserEvtRx, (void*) &SHci_Tl_Init_Conf);



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
  TRACECPU1("init wireless stack\n")

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

  TRACECPU1("init stack layers\n")

//  zb = ZbInit(0U, NULL, NULL);

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
      TRACECPU1("Wireless FW Type : FFD Zigbee stack\n");
      break;
    case INFO_STACK_TYPE_ZIGBEE_RFD:
      TRACECPU1("Wireless FW Type : RFD Zigbee stack\n");
      break;
    default:
      /* No Zigbee device supported ! */
      TRACECPU1("** ERR_ZIGBEE : No Zigbee stack found,  installed stackType: %hhu\n", wirelessInfo.StackType);
      return;
    }
    TRACECPU1("Wireless FW Version : %hhu.%hhu.%hhu", wirelessInfo.VersionMajor, wirelessInfo.VersionMinor, wirelessInfo.VersionSub);
  }
  else
  {
    TRACECPU1("** ERR_ZIGBEE : ErroCode= ERROR Unknown \n");
  }
}

bool TZigbee_stm32wb::service()
{
  if(evtShciAsync)
  {
    evtShciAsync = false;
    shci_user_evt_proc();
  }

  if(evtReqFromM0)
  {
    evtReqFromM0 = false;
    if (CptReceiveRequestFromM0 != 0) {
        Zigbee_M0RequestProcessing();
        CptReceiveRequestFromM0 = 0;
    }
  }

  if(evtNotFromM0)
  {
    evtNotFromM0 = false;
    if (CptReceiveNotifyFromM0 != 0) {
        /* If CptReceiveNotifyFromM0 is > 1. it means that we did not serve all the events from the radio */
        if (CptReceiveNotifyFromM0 > 1U) {
            TRACECPU1("** Multiple massages from M0 \n");
        }
        else {
            Zigbee_CallBackProcessing();
        }
        /* Reset counter */
        CptReceiveNotifyFromM0 = 0;
    }
  }

  if(evtNetworkForm)
  {
    evtNetworkForm = false;
    // todo: handle here network forming and joining see: APP_ZIGBEE_NwkForm()
  }

  if(evtZigbeeAppStart)
  {
    evtZigbeeAppStart = false;
    // todo: init cluster, endpoints etc. see: APP_ZIGBEE_App_Init()
  }

  return true;
}

void TZigbee_stm32wb::sysEvtHandler()
{
  HW_IPCC_SYS_EvtNot();
  ipcc.clearRxFlag(HW_IPCC_SYSTEM_EVENT_CHANNEL);
}

void TZigbee_stm32wb::sysCmdEvtHandler()
{
  ipcc.diableTxChannel(HW_IPCC_SYSTEM_CMD_RSP_CHANNEL);
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
