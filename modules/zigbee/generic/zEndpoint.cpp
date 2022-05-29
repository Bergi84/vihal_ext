#include "zCluster.h"
#include "zEndpoint.h"
#include "zDevice.h"

bool TzeBase::addCluster(TzcBase* aCluster)
{
  uint8_t locId;
  if(aCluster->clId == TzeBase::invalidId)
  {
    locId = 1;
  }
  else
  {
    locId = aCluster->clId;
  }

  if(clusters == 0 || locId < clusters->clId)
  {
    // insert at begin
    aCluster->endpoint = this;
    aCluster->prev = 0;
    aCluster->next = clusters;
    clusters->prev = aCluster;
    clusters = aCluster;

    return true;
  }
  else
  {
    TzcBase* aktEp = clusters;
    while(aktEp != 0)
    {
      if(aktEp->next == 0 || locId < aktEp->next->clId)
      {
        if(aktEp->next->clId == locId)
        {
          if(aCluster->clId == TzeBase::invalidId)
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
          aCluster->endpoint = this;
          aCluster->prev = aktEp;
          aCluster->next = aktEp->next;

          if(aktEp->next != 0)
            aktEp->next->prev = aCluster;
          aktEp->next = aCluster;
          return true;
        }
      }
      aktEp = aktEp->next;
    }
  }

  return false;
}
