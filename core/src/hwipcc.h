/*
 * hwrtc.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#include "platform.h"

#ifndef HWIPCC_H_PRE_
#define HWIPCC_H_PRE_

class THwIpcc_pre
{
public:

};


#endif

#ifndef HWIPCC_PRE_ONLY




#ifndef HWIPCC_H_
#define HWIPCC_H_

#include "mcu_impl_ext.h"

class THwIpcc: public HWIPCC_IMPL
{


};

#endif

#else
  #undef HWIPCC_PRE_ONLY
#endif
