/*
 * zDevice.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZDEVICE_PRE_H_
#define ZDEVICE_PRE_H_

#include <stdint.h>

class TzeBase_pre;

class TzdBase_pre
{
public:
  typedef enum
  {
    DT_Coordinator,
    DT_Router,
    DT_Enddevice
  } deviceType_e;

  // this flag is set if device has connected to a network
  bool flagJoined;

  TzdBase_pre() {
    endpoints = 0;

    attrZCLVersion = 2;
    attrAppVersion = 0;
    attrStackVersion = 0;
    attrHWVersion = 0;
    attrPowerSource = 0;
    attrPhysicalEnv = 0;
    attrSWBuildId = 0;
    attrManufacturaName = 0;
    attrModelIdentifier = 0;
    attrDateCode = 0;
    attrLocationDesc = 0;
    priChMsk = bdb_priChMsk;
    secChMsk = bdb_secChMsk;

    deviceType = DT_Router;

    flagJoined = false;
  };

  bool addEndpoint(TzeBase_pre* aEndpoint);

  // set functions for zigbee basic cluster attributes an network configuration
  // functions must called before configuration of stack
  inline void setAttrZCLVersion(uint8_t aVal) {attrZCLVersion = aVal;};
  inline void setAttrAppVersion(uint8_t aVal) {attrAppVersion = aVal;};
  inline void setAttrStackVersion(uint8_t aVal) {attrStackVersion = aVal;};
  inline void setAttrHWVersion(uint8_t aVal) {attrStackVersion = aVal;};
  inline void setAttrPowerSource(uint8_t aVal) {attrPowerSource = aVal;};
  inline void setAttrPhysicalEnv(uint8_t aVal) {attrPhysicalEnv = aVal;};
  inline void setAttrSWBuildId(uint8_t* aVal) {attrSWBuildId = aVal;};
  inline void setAttrManufacturaName(uint8_t* aVal) {attrManufacturaName = aVal;};
  inline void setAttrModelIdentifier(uint8_t* aVal) {attrModelIdentifier = aVal;};
  inline void setAttrDateCode(uint8_t* aVal) {attrDateCode = aVal;};
  inline void setAttrLocationDesc(uint8_t* aVal) {attrLocationDesc = aVal;};

  inline void setDeviceType(deviceType_e aDT) {deviceType = aDT;};

  // set parent age out timeout for end devices
  // Timeout = (60 * 2^n) seconds for n > 0. If n = 0, timeout = 10 seconds.
  // 255 (0xff) disables the age out
  inline void setEndDeviceTimeout(uint8_t aTimeout) {endDeviceTimeout = aTimeout;};

  // channel mask selection, each bit stands for one channel, mask = 1 << ch1 | 1 << ch2 ....
  // available channel range is from 11 to 26, but many devices do not support channel 26
  // if no channels are selected primary channel mask is set to ch 11,15,20,25
  // like recommended in the bdb specification. The secondary channel mask is set to the rest
  // excepting channel 26 to avoid a network form on this channel
  inline void setPriChMsk(uint32_t aMsk) {priChMsk = aMsk;};
  inline void setSecChMsk(uint32_t aMsk) {secChMsk = aMsk;};


  // set transmission power, aPwr is in dBm
  bool setTxPwr(int8_t aPwr);

protected:
  TzeBase_pre* endpoints;

  // basic Cluster attributes
  // this attributes normally should set before startup of zigbee
  uint8_t attrZCLVersion;
  uint8_t attrAppVersion;
  uint8_t attrStackVersion;
  uint8_t attrHWVersion;
  uint8_t attrPowerSource;
  uint8_t attrPhysicalEnv;
  uint8_t* attrSWBuildId;
  uint8_t* attrManufacturaName;
  uint8_t* attrModelIdentifier;
  uint8_t* attrDateCode;
  uint8_t* attrLocationDesc;

  deviceType_e deviceType;

  uint8_t endDeviceTimeout;

  static constexpr uint32_t bdb_priChMsk = 0x02108800;
  static constexpr uint32_t bdb_secChMsk = 0x03fff800 ^ bdb_priChMsk;

  uint32_t priChMsk;
  uint32_t secChMsk;
};

#endif /* ZDEVICE_PRE_H_ */


#ifndef ZDEVICE_PRE_ONLY

#ifndef ZDEVICE_H_
#define ZDEVICE_H_

#include "z_impl.h"

class TzdBase : public TZDBASE_IMPL
{

};

#endif /* ZDEVICE_H_ */

#else
  #undef ZDEVICE_PRE_ONLY
#endif
