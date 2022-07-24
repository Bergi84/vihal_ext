/*
 * zigbee_stm32wb.h
 *
 *  Created on: 09.04.2022
 *      Author: Bergi
 */

#ifndef ZDEVICE_STM32WB_H_
#define ZDEVICE_STM32WB_H_

#define ZDEVICE_PRE_ONLY
#include "zDevice.h"

#include "hwpwr.h"
#include "hwipcc.h"
#include "hwsema.h"
#include "hwintflash.h"
#include "sequencer_armm.h"
#include "traces.h"
#include "timeServer.h"

#include "shci_tl.h"
#include "zigbee_interface.h"

class Tzd_stm32 : public TzdBase_pre, public TCbClass
{
public:
  inline struct ZigBeeT* getZHandler() {return zb;};

  THwIpcc ipcc;
  THwSema sema;
  TSequencer* seq;
  THwPwr* pwr;
  TTimerServer* ts;
  THwIntFlash* flash;

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

  uint8_t tsIdJoin;
  uint8_t tsIdTimeOut;

  bool evtAckFromM0;
  bool evtSyncBypassNotIdle;
  bool evtSyncBypassReqIdle;
  bool evtShciCmdResp;
  bool evtStartupDone;

  // buffer in flash for persistent zigbee data
  static constexpr uint32_t nvmSize = 4096;
  static constexpr uint8_t nvm[nvmSize] __attribute__ ((aligned (4096))) = {0};

  volatile uint32_t CptReceiveRequestFromM0;
  volatile uint32_t CptReceiveNotifyFromM0;
  void (*FreeBufCb)( void );

  struct ZigBeeT *zb;

  // this flag signals that the stack is read for configuration
  bool flagStackInitDone;

  // this flag signals that the stack configuration is done and the zigbee is
  // read for network forming
  bool flagStackConfigDone;

  // this flag is set if device was connected to a network
  bool flagWasJoined;

  // this flag signals a join/rejoin timeout occurred
  bool flagJoinTimeout;

  // this flag is used for search after new networks
  // if set the network data is reset
  bool flagDoReset;

  // after calling join the timeout start to run, is the device a coordinator
  // is this the time a new device can join the network, is the device not a coordinator
  // the join procedure try a to establish a connection to a network every connectRetryTime
  // times are in ms
  uint32_t joinTimeout;
  uint32_t joinRetryInterval;

  inline void confJoining(uint32_t aTimeOut, uint32_t aRetryInterval)
  {joinTimeout = aTimeOut; joinRetryInterval = aRetryInterval;};

  // after a device was connected to a network and lost the connection
  // the device try for rejoinTimeout to rejoin to the network in interval
  // of rejoinRetryTime, if rejoinRetryTime = -1 the device try for infinite time to rejoining
  // time is in ms
  uint32_t rejoinTimeout;
  uint32_t rejoinRetryInterval;

  inline void confRejoining(uint32_t aTimeOut, uint32_t aRetryInterval)
  {rejoinTimeout = aTimeOut; rejoinRetryInterval = aRetryInterval;};

  Tzd_stm32();

  // before calling this function the rf clock and the rf wakeup clock
  // must be configured, configures the rf hardware for zigbee operation
  // this function returns immediately
  // Initialization is done if the flag flagStackInitDone becomes true
  bool init(TSequencer* aSeq, THwPwr* aPwr, TTimerServer* aTs, THwIntFlash* aFlash);

  // after adding end points and clusters this function must called
  // the function configures the stack with configured end points and clusters
  // configuration is done if flagStackConfigDone is true
  // after calling this function further modification of endpoints or clusters
  // are not allowed and lead to an unpredicted behavior
  bool config();

  // startup the zigbee communication. Device type and channel configuration must
  // be done before calling this function, this function returns immediately
  // if the stack have joined a Network the flagJoined flag becomes true
  // if the stack can't connect a network the flagJoinTimeout becomes true
  // if valid persistent data found it trys to start with this
  bool startup();

  // this function clears the network layer and starts a new search for an Network
  // can called instead of startup or afterwards
  bool joinNewNwk();

  // Handler functions for stack interface
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
  void storePersistentData(void);
  void clearPersistentData(void);

  bool setTxPwr(int8_t aPwr);

private:
  bool rfdStackFound;
  bool ffdStackFound;

  // tasks
  void taskProcessNotifyM0(void);
  void taskProcessRequestM0(void);
  void taskFormNetwork(void);

  void initStack();
  void checkWirelessFwInfo();
  void retryJoin(THwRtc::time_t time) {seq->queueTask(seqIdNetworkForm);};
  void signalTimeout(THwRtc::time_t time) {flagJoinTimeout = true;};
  bool checkPersistentData(uint8_t* buf, uint32_t aLen, uint32_t &dataLen);
  static enum zb_msg_filter_rc nlmeMsgCb(struct ZigBeeT *zb, uint32_t id, void *msg, void *cbarg);


/*
 * According AN5500 we need to call ZbTimerWork and WpanMacWork
 * on regular basis but st don't use this functions in any of there
 * examples and WpanMacWork don't exist and until now it works also without
 *
  uint8_t seqIdStackWork;
  uint8_t seqIdMacWork;

  uint8_t tsIdStackWork;
  uint8_t tsIdMacWork;

  void serviceStack(void);
  void serviceMac(void);


  void queueStackWork(THwRtc::time_t time) {seq->queueTask(seqIdStackWork);};
  void queueMacWork(THwRtc::time_t time) {seq->queueTask(seqIdMacWork);};
  */
};

#define TZDBASE_IMPL Tzd_stm32

#endif /* ZIGBEE_STM32WB_H_ */
