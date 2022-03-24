/*
 * hwrtc.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#include "platform.h"

#ifndef HWSEMA_H_PRE_
#define HWSEMA_H_PRE_

class THwSema_pre
{
public:

};


#endif

#ifndef HWSEMA_PRE_ONLY




#ifndef HWSEMA_H_
#define HWSEMA_H_

#include "mcu_impl_ext.h"

class THwSema : public HWSEMA_IMPL
{


};

#endif

#else
  #undef HWSEMA_PRE_ONLY
#endif
