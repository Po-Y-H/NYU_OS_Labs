#include "Scheduler.h"

FIFOScheduler::FIFOScheduler() {}

FIFOScheduler::FIFOScheduler(vector<IOoperation>& IOq)
{
    this->IOqueue = &IOq;
}

IOoperation FIFOScheduler::strategy()
{
    IOoperation a;
    a = IOqueue->front();
    IOqueue->erase(IOqueue->begin());
    return a;
}