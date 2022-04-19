/*
 * zigbee_stm32wb.h
 *
 *  Created on: 09.04.2022
 *      Author: Bergi
 */

#ifndef ZIGBEE_STM32WB_H_
#define ZIGBEE_STM32WB_H_

#include "hwpwr.h"
#include "hwipcc.h"
#include "hwsema.h"
#include "sequencer_armm.h"
#include "traces.h"

#include "shci_tl.h"


class TZigbee_stm32wb : public TCbClass
{
public:
  THwIpcc ipcc;
  THwSema sema;
  TSequencer* seq;
  THwPwr* pwr;

  static constexpr uint32_t CFG_TL_EVT_QUEUE_LENGTH = 5;
  static constexpr uint32_t POOL_SIZE  = (CFG_TL_EVT_QUEUE_LENGTH * 4U * DIVC(( sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255), 4U));

  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t EvtPool[POOL_SIZE];
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t SystemCmdBuffer;
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t SystemSpareEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static TL_CmdPacket_t OtCmdBuffer;
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t NotifRspEvtBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];
  PLACE_IN_SECTION("MB_MEM2") ALIGN(4) static uint8_t NotifRequestBuffer[sizeof(TL_PacketHeader_t) + TL_EVT_HDR_SIZE + 255U];

  TL_EvtPacket_t *notFromM0;
  TL_EvtPacket_t *reqFromM0;

  uint8_t seqIdShciAsync;
  uint8_t seqIdReqFromM0;
  uint8_t seqIdNotFromM0;
  uint8_t seqIdNetworkForm;
  uint8_t seqIdZigbeeAppStart;

  bool evtAckFromM0;
  bool evtSyncBypassIdle;
  bool evtShciCmdResp;

  TL_EvtPacket_t *p_ZIGBEE_request_M0_to_M4;
  volatile uint32_t CptReceiveRequestFromM0;
  volatile uint32_t CptReceiveNotifyFromM0;
  void (*FreeBufCb)( void );

  struct ZigBeeT *zb;

  // this flag signals that the stack is read for the endpoint configuration
  bool stackInitDone;

  // this flag signals that the stack configuration is done and the zigbee is
  // read for network forming
  bool stackConfigDone;

  TZigbee_stm32wb();

  bool init(TSequencer* aSeq, THwPwr* aPwr);

  void sysEvtHandler();
  void sysCmdEvtHandler();
  void stackNotifEvtHandler();
  void stackM0RequestHandler();
  void traceEvtHandler();
  void mmFreeBufHandler();
  void cmdEvtHandler();
  void tracesEvtHandler();
  void cmdTransfer();

  void sysStatusNot( SHCI_TL_CmdStatus_t status );
  void sysUserEvtRx(void *pPayload);

  // tasks
  void processNotifyM0(void);
  void processRequestM0(void);
  void formNetwork(void);
  void appStart(void);

private:
  void initStack();
  void checkWirelessFwInfo();
};

extern TZigbee_stm32wb gZigbee;

#endif /* ZIGBEE_STM32WB_H_ */
