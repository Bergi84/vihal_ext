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
  };

  bool addEndpoint(TzeBase_pre* aEndpoint);

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

  inline void getAttrZCLVersion(uint8_t &aVal);
  inline void setAttrZCLVersion(uint8_t aVal);
  inline void getAttrAppVersion(uint8_t &aVal);
  inline void setAttrAppVersion(uint8_t aVal);
  inline void getAttrStackVersion(uint8_t &aVal);
  inline void setAttrStackVersion(uint8_t aVal);
  inline void getAttrHWVersion(uint8_t &aVal);
  inline void setAttrHWVersion(uint8_t aVal);
  inline void getAttrPowerSource(uint8_t &aVal);
  inline void setAttrPowerSource(uint8_t aVal);
  inline void getAttrPhysicalEnv(uint8_t &aVal);
  inline void setAttrPhysicalEnv(uint8_t aVal);
  inline void getAttrSWBuildId(uint8_t* &aVal);
  inline void setAttrSWBuildId(uint8_t* aVal);
  inline void getAttrManufacturaName(uint8_t* &aVal);
  inline void setAttrManufacturaName(uint8_t* aVal);
  inline void getAttrModelIdentifier(uint8_t* &aVal);
  inline void setAttrModelIdentifier(uint8_t* aVal);
  inline void getAttrDateCode(uint8_t* &aVal);
  inline void setAttrDateCode(uint8_t* aVal);
  inline void getAttrLocationDesc(uint8_t* &aVal);
  inline void setAttrLocationDesc(uint8_t* aVal);

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
