/*
 * hwrtc.h
 *
 *  Created on: 12.03.2022
 *      Author: Bergi
 */

#include "platform.h"

#ifndef HWCLKTREE_H_PRE_
#define HWCLKTREE_H_PRE_

class THwClkTree_pre
{

};


#endif

#ifndef HWCLKTREE_PRE_ONLY




#ifndef HWCLKTREE_H_
#define HWCLKTREE_H_

#include "mcu_impl_ext.h"

class THwClkTree : public HWCLKTREE_IMPL
{


};

#endif

#else
  #undef HWCLKTREE_PRE_ONLY
#endif
