#include "Pager.h"

PagerFIFO::PagerFIFO() {}

PagerFIFO::PagerFIFO(int ftsize)
{
	this->sizeOfFT = ftsize;
	this->count = 0;
}

FRAME* PagerFIFO::selectVictimFrame(FRAME (&frameTable)[MAX_FRAMES])
{
	FRAME* victim = &frameTable[count];
	count++;
	if (sizeOfFT == count)
	{
		count = 0;
	}
	victim->victim = true;
	//cout << "victim PID: " << nextVictim->PID << " vpage:" << nextVictim->Vpage << endl;
	return victim;
}