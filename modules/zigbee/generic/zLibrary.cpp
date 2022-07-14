/*
 * zLibrary.cpp
 *
 *  Created on: 29.05.2022
 *      Author: Klaus
 */

#include "zLibrary.h"

TzcOnOff::~TzcOnOff()
{

}

TzcOnOffServer_pre::~TzcOnOffServer_pre()
{

}

TzcOnOffClient_pre::~TzcOnOffClient_pre()
{

}


TzcOnOff::cmdCbRec_t* TzcOnOff::getCbList(uint8_t* len)
{
  if(len) *len = cmdListLen;
  return cmdCbList;
}

TzcOnOffClient::~TzcOnOffClient()
{
}

TzcOnOffServer::~TzcOnOffServer()
{

}
