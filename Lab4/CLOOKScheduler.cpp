#include "Scheduler.h"

CLOOKScheduler::CLOOKScheduler() {}

CLOOKScheduler::CLOOKScheduler(vector<IOoperation>& IOq, int& currPos, bool& trackDir)
{
	this->IOqueue = &IOq;
	this->currPos = &currPos;
	this->trackDir = &trackDir;
	// track direction; (default) low to high = true, high to low = false.
}

IOoperation CLOOKScheduler::strategy()
{
	int minDiff = 9999;
	int maxDiff = 0;
	int selectPos = 0;
	bool foundInSameDir = false;
	IOoperation a;

	for (int i = 0; i != (*IOqueue).size(); i++)
	{
		int diff = ((*IOqueue)[i].track_number - (*currPos));
		if (diff >= 0 && diff < minDiff)
		{
			minDiff = diff;
			selectPos = i;
			foundInSameDir = true;
		}
	}
	if (foundInSameDir == false)	// not found on right side, search left most
	{
		for (int i = 0; i != (*IOqueue).size(); i++)
		{
			int diff = ((*IOqueue)[i].track_number - (*currPos));
			if (diff < 0 && abs(diff) > maxDiff)
			{
				maxDiff = abs(diff);
				selectPos = i;
			}
		}
	}

	a = IOqueue->at(selectPos);
	IOqueue->erase(IOqueue->begin() + selectPos);
	return a;
}