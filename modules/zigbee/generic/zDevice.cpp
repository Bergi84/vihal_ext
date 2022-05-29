#include "zCluster.h"
#include "zEndpoint.h"
#include "zDevice.h"


bool TzdBase_pre::addEndpoint(TzeBase_pre* aEndpoint)
{
  uint8_t locId;
  if(aEndpoint->epId == TzeBase_pre::invalidId)
  {
    locId = 1;
  }
  else
  {
    locId = aEndpoint->epId;
  }

  if(endpoints == 0 || locId < endpoints->epId)
  {
    // insert at begin
    aEndpoint->device = this;
    aEndpoint->prev = 0;
    aEndpoint->next = endpoints;
    endpoints->prev = aEndpoint;
    endpoints = aEndpoint;

    return true;
  }
  else
  {
    TzeBase_pre* aktEp = endpoints;
    while(aktEp != 0)
    {
      if(aktEp->next == 0 || locId < aktEp->next->epId)
      {
        if(aktEp->next->epId == locId)
        {
          if(aEndpoint->epId == TzeBase_pre::invalidId)
          {
            locId++;
          }
          else
          {
            return false;
          }
        }
        else
        {
          aEndpoint->device = this;
          aEndpoint->prev = aktEp;
          aEndpoint->next = aktEp->next;

          if(aktEp->next != 0)
            aktEp->next->prev = aEndpoint;
          aktEp->next = aEndpoint;
          return true;
        }
      }
      aktEp = aktEp->next;
    }
  }

  return false;
}
