#include "zDevice.h"
#include "zCluster.h"
#include "zEndpoint.h"

bool Tzd_stm32::init(struct ZigBeeT *aZb)
{
  zb = aZb;

  if(endpoints == 0)
  {
    return false;
  }

  Tze_stm32wb* aktEp = (Tze_stm32wb*) endpoints;
  while(aktEp != 0)
  {
    aktEp->init();
    aktEp = (Tze_stm32wb*) aktEp->getNext();
  }

  return true;
}
