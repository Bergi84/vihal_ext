/*
 * hwrtc.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#include "platform.h"

#ifndef HWPWR_H_PRE_
#define HWPWR_H_PRE_

class THwPwr_pre
{
public:

};


#endif

#ifndef HWPWR_PRE_ONLY




#ifndef HWPWR_H_
#define HWPWR_H_

#include "mcu_impl_ext.h"

class THwPwr : public HWPWR_IMPL
{


};

#endif

#else
  #undef HWPWR_PRE_ONLY
#endif
