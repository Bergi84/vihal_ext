/*
 * zDevice.h
 *
 *  Created on: 24.04.2022
 *      Author: Bergi
 */

#ifndef ZDEVICE_H_
#define ZDEVICE_H_

#include <stdint.h>

class TzeBase;

class TzdBase
{
public:
  TzdBase() { endpoints = 0; };

  bool addEndpoint(TzeBase* aEndpoint);

private:
  TzeBase* endpoints;
};





#endif /* ZDEVICE_H_ */
