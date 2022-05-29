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
  TzdBase_pre() { endpoints = 0; };

  bool addEndpoint(TzeBase_pre* aEndpoint);

protected:
  TzeBase_pre* endpoints;
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
