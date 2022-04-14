/*
 * zigbee_stm32wb.h
 *
 *  Created on: 09.04.2022
 *      Author: Bergi
 */

#ifndef ZIGBEE_STM32WB_H_
#define ZIGBEE_STM32WB_H_

#include "hwipcc.h"
#include "hwsema.h"
#include "traces.h"

#include "shci_tl.h"


class TZigbee_stm32wb : public TCbClass
{
public:
  THwIpcc ipcc;
  THwSema sema;

  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t SystemCmdBuffer;
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t OtCmdBuffer;
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t NotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t NotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

  bool evtShciAsync;
  bool evtReqFromM0;
  bool evtNotFromM0;
  bool evtNetworkForm;
  bool evtZigbeeAppStart;

  TL_EvtPacket_t *p_ZIGBEE_request_M0_to_M4;
  volatile uint32_t CptReceiveRequestFromM0;
  volatile uint32_t CptReceiveNotifyFromM0;

  struct ZigBeeT *zb;
  bool initDone;
  bool configDone;

  TZigbee_stm32wb();

  bool init();
  bool service();

  // functions with c wrapper
  void sysEvtHandler();
  void sysCmdEvtHandler();
  void stackNotifEvtHandler();
  void stackM0RequestHandler();
  void traceEvtHandler();


  void sysStatusNot( SHCI_TL_CmdStatus_t status );
  void sysUserEvtRx(void *pPayload);

private:
  void initStack();
  void checkWirelessFwInfo();
};

extern TZigbee_stm32wb gZigbee;

#endif /* ZIGBEE_STM32WB_H_ */
