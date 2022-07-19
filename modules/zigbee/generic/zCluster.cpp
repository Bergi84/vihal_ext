#include "zCluster.h"
#include "zEndpoint.h"
#include "zDevice.h"

TzcBase_pre::~TzcBase_pre()
{

}

TzdBase_pre* TzcBase_pre::getDevice() {return endpoint->getDevice(); };

TzcBase::~TzcBase()
{

}
