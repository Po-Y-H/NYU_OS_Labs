#include "Scheduler.h"

SSTFScheduler::SSTFScheduler() {}

SSTFScheduler::SSTFScheduler(vector<IOoperation>& IOq, int& currPos)
{
	this->IOqueue = &IOq;
    this->currPos = &currPos;
}

IOoperation SSTFScheduler::strategy()
{
    int minDiff = 9999;
    int selectPos = 0;
    IOoperation a;

    for (int i = 0; i != (*IOqueue).size(); i++)
    {
        int diff = abs((*IOqueue)[i].track_number - (*currPos));
        if (diff < minDiff)
        {
            minDiff = diff;
            selectPos = i;
        }
    }
    a = IOqueue->at(selectPos);
    IOqueue->erase(IOqueue->begin() + selectPos);
    return a;
}