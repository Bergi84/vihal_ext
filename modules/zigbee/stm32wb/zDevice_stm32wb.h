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
#include "sequencer_armm.h"
#include "traces.h"

#include "shci_tl.h"

class Tzd_stm32 : public TzdBase_pre, public TCbClass
{
public:
  inline struct ZigBeeT* getZHandler() {return zb;};

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

  bool evtAckFromM0;
  bool evtSyncBypassIdle;
  bool evtShciCmdResp;

  volatile uint32_t CptReceiveRequestFromM0;
  volatile uint32_t CptReceiveNotifyFromM0;
  void (*FreeBufCb)( void );

  struct ZigBeeT *zb;

  // this flag signals that the stack is read for configuration
  bool stackInitDone;

  // this flag signals that the stack configuration is done and the zigbee is
  // read for network forming
  bool stackConfigDone;

  Tzd_stm32();

  // configures the rf hardware for zigbee operation
  // this function returns immediately
  // Initialization is done if stackInitDone becomes true
  bool init(TSequencer* aSeq, THwPwr* aPwr);

  // after adding end points and clusters this function must called
  // the function configures the stack with configured end points and clusters
  // configuration is done if stackConfigDone is true
  // after calling this function further modification of endpoints or clusters
  // are not allowed and lead to an unpredicted behavior
  bool config(void);

  // set get functions for zigbee basic cluster
  // get functions must called before the config function
  inline void getAttrZCLVersion(uint8_t &aVal) {aVal = attrZCLVersion;};
  inline void setAttrZCLVersion(uint8_t aVal) {attrZCLVersion = aVal;};
  inline void getAttrAppVersion(uint8_t &aVal) {aVal = attrAppVersion;};
  inline void setAttrAppVersion(uint8_t aVal) {attrAppVersion = aVal;};
  inline void getAttrStackVersion(uint8_t &aVal) {aVal = attrStackVersion;};
  inline void setAttrStackVersion(uint8_t aVal) {attrStackVersion = aVal;};
  inline void getAttrHWVersion(uint8_t &aVal) {aVal = attrStackVersion;};
  inline void setAttrHWVersion(uint8_t aVal) {attrStackVersion = aVal;};
  inline void getAttrPowerSource(uint8_t &aVal) {aVal = attrPowerSource;};
  inline void setAttrPowerSource(uint8_t aVal) {attrPowerSource = aVal;};
  inline void getAttrPhysicalEnv(uint8_t &aVal) {aVal = attrPhysicalEnv;};
  inline void setAttrPhysicalEnv(uint8_t aVal) {attrPhysicalEnv = aVal;};
  inline void getAttrSWBuildId(uint8_t* &aVal) {aVal = attrSWBuildId;};
  inline void setAttrSWBuildId(uint8_t* aVal) {attrSWBuildId = aVal;};
  inline void getAttrManufacturaName(uint8_t* &aVal) {aVal = attrManufacturaName;};
  inline void setAttrManufacturaName(uint8_t* aVal) {attrManufacturaName = aVal;};
  inline void getAttrModelIdentifier(uint8_t* &aVal) {aVal = attrModelIdentifier;};
  inline void setAttrModelIdentifier(uint8_t* aVal) {attrModelIdentifier = aVal;};
  inline void getAttrDateCode(uint8_t* &aVal) {aVal = attrDateCode;};
  inline void setAttrDateCode(uint8_t* aVal) {attrDateCode = aVal;};
  inline void getAttrLocationDesc(uint8_t* &aVal) {aVal = attrLocationDesc;};
  inline void setAttrLocationDesc(uint8_t* aVal) {attrLocationDesc = aVal;};

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

  // tasks
  void processNotifyM0(void);
  void processRequestM0(void);
  bool formNetwork(void);


private:
  void initStack();
  void checkWirelessFwInfo();
};

#define TZDBASE_IMPL Tzd_stm32

#endif /* ZIGBEE_STM32WB_H_ */
