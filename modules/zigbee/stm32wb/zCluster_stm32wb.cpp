/*
 * zCluster_stm32wb.cpp
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */
#include "zDevice.h"
#include "zCluster.h"
#include "zEndpoint.h"


enum ZclStatusCodeT Tzc_stm32wb::cbHandler(uint16_t cmd, TzcBase_pre* pCluster)
{
  uint8_t len;
  cmdCbRec_t* cbList = pCluster->getCbList(&len);

  if(len < cmd)
  {
    return ZCL_STATUS_FAILURE;
  }

  if(cbList[cmd].pObj)
  {
    if(cbList[cmd].pMFunc)
    {
      (cbList[cmd].pObj->*cbList[cmd].pMFunc)();
      return ZCL_STATUS_SUCCESS;
    }
    else
    {
      return ZCL_STATUS_FAILURE;
    }
  }
  else
  {
    if(cbList[cmd].pFunc)
    {
      cbList[cmd].pFunc();
      return ZCL_STATUS_SUCCESS;
    }
    else
    {
      return ZCL_STATUS_FAILURE;
    }
  }
}
