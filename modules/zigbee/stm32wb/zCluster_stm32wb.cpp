/*
 * zCluster_stm32wb.cpp
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */
#include "zDevice.h"
#include "zCluster.h"
#include "zEndpoint.h"

Tzc_stm32wb::~Tzc_stm32wb()
{

}

void Tzc_stm32wb::adrConv2st(zAdr_t* intAdr, ZbApsAddrT &adr_st)
{
  if(intAdr == 0)
  {
    adr_st.mode = ZB_APSDE_ADDRMODE_SHORT;
    adr_st.nwkAddr = 0;   // coordinator
    adr_st.endpoint = endpoint->getEpId();
  }
  else
  {
    switch(intAdr->mode)
    {
    case zAdr_t::MODE_INVALID:
      adr_st.mode = ZB_APSDE_ADDRMODE_NOTPRESENT;
      break;
    case zAdr_t::MODE_GROUP:
      adr_st.mode = ZB_APSDE_ADDRMODE_GROUP;
      adr_st.nwkAddr = intAdr->nwkAdr;
      adr_st.endpoint = intAdr->epId;
      break;
    case zAdr_t::MODE_SHORT:
      adr_st.mode = ZB_APSDE_ADDRMODE_SHORT;
      adr_st.nwkAddr = intAdr->nwkAdr;
      adr_st.endpoint = intAdr->epId;
      break;
    case zAdr_t::MODE_EXTENDED:
      adr_st.mode = ZB_APSDE_ADDRMODE_EXT;
      adr_st.extAddr = intAdr->extAdr;
      adr_st.endpoint = intAdr->epId;
      break;
    case zAdr_t::MODE_INTERPAN:
      adr_st.mode = ZB_APSDE_ADDRMODE_IPGROUP;
      adr_st.endpoint = ZB_ENDPOINT_INTERPAN;
      adr_st.nwkAddr = intAdr->nwkAdr;
      adr_st.panId = intAdr->panID;
      break;
    }
  }
}

void Tzc_stm32wb::adrConv2int(ZbApsAddrT* stAdr, zAdr_t &adr_int)
{
  if(stAdr == 0)
  {
    // if no address was given use the coordinator address
    adr_int.mode = zAdr_t::MODE_SHORT;
    adr_int.nwkAdr = 0;
    adr_int.epId = endpoint->getEpId();
  }
  else
  {
    switch(stAdr->mode)
    {
    case ZB_APSDE_ADDRMODE_NOTPRESENT:
      adr_int.mode = zAdr_t::MODE_INVALID;
      break;

    case ZB_APSDE_ADDRMODE_GROUP:
      adr_int.mode = zAdr_t::MODE_GROUP;
      adr_int.nwkAdr = stAdr->nwkAddr;
      adr_int.epId = stAdr->endpoint;
      break;

    case ZB_APSDE_ADDRMODE_SHORT:
      adr_int.mode = zAdr_t::MODE_SHORT;
      adr_int.nwkAdr = stAdr->nwkAddr;
      adr_int.epId = stAdr->endpoint;
      break;

    case ZB_APSDE_ADDRMODE_EXT:
      adr_int.mode = zAdr_t::MODE_EXTENDED;
      adr_int.extAdr = stAdr->extAddr;
      adr_int.epId = stAdr->endpoint;
      break;

    case ZB_APSDE_ADDRMODE_IPGROUP:
      adr_int.mode = zAdr_t::MODE_INTERPAN;
      adr_int.nwkAdr = stAdr->nwkAddr;
      adr_int.panID = stAdr->panId;
      break;
    }
  }
}
