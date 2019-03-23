#include "dmstrsrv.h"

#include "root.h"

#include "apiv1.h"

using namespace Cutelyst;

dmstrsrv::dmstrsrv(QObject *parent) : Application(parent)
{
}

dmstrsrv::~dmstrsrv()
{
}

bool dmstrsrv::init()
{
    new ApiV1 (this);
    new Root(this);

    return true;
}

