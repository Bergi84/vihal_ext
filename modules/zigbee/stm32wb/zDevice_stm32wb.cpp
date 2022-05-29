#include <zDevice_stm32wb.h>
#include "zCluster_stm32wb.h"
#include "zEndpoint_stm32wb.h"

bool Tzd_stm32::init(struct ZigBeeT *aZb)
{
  zb = aZb;

  if(endpoints == 0)
  {
    return false;
  }

  Tze_stm32wb* aktEp = endpoints;
  while(aktEp != 0)
  {
    aktEp->init();
    aktEp = aktEp->next;
  }
}
