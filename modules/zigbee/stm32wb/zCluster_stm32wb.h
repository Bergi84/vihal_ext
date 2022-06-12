/*
 * zCluster_stm32wb.h
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */

#ifndef ZCLUSTER_STM32WB_H_
#define ZCLUSTER_STM32WB_H_

#define ZCLUSTER_PRE_ONLY
#include "zCluster.h"
#include "zEndpoint.h"

#include "zcl/zcl.h"

class Tzc_stm32wb : public TzcBase_pre
{
protected:
  Tzc_stm32wb() {};
  virtual ~Tzc_stm32wb() {};

  struct ZbZclClusterT* clusterHandler;

  inline ZigBeeT* getZHandler() {return ((TzeBase*)endpoint)->getZHandler();};

  static enum ZclStatusCodeT cbHandler(uint16_t cmd, TzcBase_pre* pCluster);

  ZbApsAddrT adrConv(zAdr_t* zAdr)
  {
    ZbApsAddrT adr_st;

    if(zAdr == 0)
    {
      adr_st.mode = ZB_APSDE_ADDRMODE_SHORT;
      adr_st.nwkAddr = 0;   // coordinator
      adr_st.endpoint = endpoint->getEpId();
    }
    else
    {
      switch(zAdr->mode)
      {
      case zAdr_t::MODE_INVALID:
        adr_st.mode = ZB_APSDE_ADDRMODE_NOTPRESENT;
        break;
      case zAdr_t::MODE_GROUP:
        adr_st.mode = ZB_APSDE_ADDRMODE_GROUP;
        adr_st.nwkAddr = zAdr->nwkAdr;
        adr_st.endpoint = zAdr->epId;
        break;
      case zAdr_t::MODE_SHORT:
        adr_st.mode = ZB_APSDE_ADDRMODE_SHORT;
        adr_st.nwkAddr = zAdr->nwkAdr;
        adr_st.endpoint = zAdr->epId;
        break;
      case zAdr_t::MODE_EXTENDED:
        adr_st.mode = ZB_APSDE_ADDRMODE_EXT;
        adr_st.extAddr = zAdr->extAdr;
        adr_st.endpoint = zAdr->epId;
        break;
      case zAdr_t::MODE_INTERPAN:
        adr_st.mode = ZB_APSDE_ADDRMODE_IPGROUP;
        adr_st.endpoint = ZB_ENDPOINT_INTERPAN;
        adr_st.nwkAddr = zAdr->nwkAdr;
        adr_st.panId = zAdr->panID;
        break;
      }
    }
    return adr_st;
  }

private:
  friend class Tze_stm32wb;
  virtual bool init();
};

#define TZCBASE_IMPL Tzc_stm32wb

#endif /* ZCLUSTER_STM32WB_H_ */
