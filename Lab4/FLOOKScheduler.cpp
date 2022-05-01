#include "Scheduler.h"

FLOOKScheduler::FLOOKScheduler() {}

FLOOKScheduler::FLOOKScheduler(vector<IOoperation>& IOq, vector<IOoperation>& IOq2, int& currPos, bool& trackDir, bool& queueSwapped)
{
	this->IOqueue = &IOq;
	this->IOqueue2 = &IOq2;
	this->currPos = &currPos;
	this->trackDir = &trackDir;
	this->swapQ = &queueSwapped;
	this->currSwap = false;
	// track direction; (default) low to high = true, high to low = false.
}

IOoperation FLOOKScheduler::strategy()
{
	int minDiff = 9999;
	int selectPos = 0;
	bool foundInSameDir = false;

	IOoperation a;
	
	// check if swapQ is needed
	if (currSwap != (*swapQ))	 // false ( add = IOQueue2 ) => true ( add = IOQueue )
	{
		vector<IOoperation>* t = IOqueue;
		IOqueue = IOqueue2;
		IOqueue2 = t;
		currSwap = (*swapQ);
	}

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