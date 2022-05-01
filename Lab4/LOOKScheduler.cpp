#include "Scheduler.h"

LOOKScheduler::LOOKScheduler() {}

LOOKScheduler::LOOKScheduler(vector<IOoperation>& IOq, int& currPos, bool& trackDir)
{
	this->IOqueue = &IOq;
	this->currPos = &currPos;
	this->trackDir = &trackDir;
	// track direction; (default) low to high = true, high to low = false.
}

IOoperation LOOKScheduler::strategy()
{
	int minDiff = 9999;
	int selectPos = 0;
	bool foundInSameDir = false;

	IOoperation a;
	if ((*trackDir))    // low to high, find right most
	{

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
		if (foundInSameDir == false)	// not found on right side, search left
		{
			for (int i = 0; i != (*IOqueue).size(); i++)
			{
				int diff = ((*IOqueue)[i].track_number - (*currPos));
				if (diff < 0 && abs(diff) < minDiff)
				{
					minDiff = abs(diff);
					selectPos = i;
				}
			}
		}
	}
	else			 // high to low, find left most
	{
		for (int i = 0; i != (*IOqueue).size(); i++)
		{
			int diff = ((*IOqueue)[i].track_number - (*currPos));
			if (diff <= 0 && abs(diff) < minDiff)
			{
				minDiff = abs(diff);
				selectPos = i;
				foundInSameDir = true;
			}
		}
		if (foundInSameDir == false)	// not found on left side, search right
		{
			for (int i = 0; i != (*IOqueue).size(); i++)
			{
				int diff = ((*IOqueue)[i].track_number - (*currPos));
				if (diff > 0 && diff < minDiff)
				{
					minDiff = diff;
					selectPos = i;
				}
			}
		}
	}

	a = IOqueue->at(selectPos);
	IOqueue->erase(IOqueue->begin() + selectPos);
	return a;
}
