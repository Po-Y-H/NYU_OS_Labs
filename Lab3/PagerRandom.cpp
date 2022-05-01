#include "Pager.h"

PagerRandom::PagerRandom() {}

PagerRandom::PagerRandom(int ftsize, vector<int> randVals)
{
	this->sizeOfFT = ftsize;
	this->count = 0;
	this->randVec = randVals;
}

FRAME* PagerRandom::selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES])
{
	count = randVec[ofs] % sizeOfFT;
	FRAME* victim = &frameTable[count];
	ofs++;

	if (ofs >= randVec.size())
	{
		ofs = 0;
	}
	victim->victim = true;
	//cout << "victim PID: " << nextVictim->PID << " vpage:" << nextVictim->Vpage << endl;
	return victim;
}