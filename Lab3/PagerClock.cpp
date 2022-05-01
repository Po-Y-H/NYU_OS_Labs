#include "Pager.h"

PagerClock::PagerClock() {}

PagerClock::PagerClock(int ftsize, vector<Process*>& procsVec)
{
	this->sizeOfFT = ftsize;
	this->count = 0;
	this->hand = 0;
	this->procsV = procsVec;
}

FRAME* PagerClock::selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES])
{
	int startf = count;
	int pid = 0;
	int vpage = 0;
	FRAME* hand = &frameTable[count];
	FRAME* victim = &frameTable[count];

	pid = hand->PID;
	vpage = hand->Vpage;
	PTE* p = procsV[pid]->getPTE(vpage);

	while (p->referenced)
	{
		p->referenced = 0;
		count++;
		if (sizeOfFT == count)
		{
			count = 0;
		}
		hand = &frameTable[count];
		pid = hand->PID;
		vpage = hand->Vpage;
		p = procsV[pid]->getPTE(vpage);
	}

	if (p->referenced == 0)
	{
		victim = &frameTable[count];
		victim->victim;  //??
		count++;
		if (sizeOfFT == count)
		{
			count = 0;
		}
		hand = &frameTable[count];
	}
	
	//cout << "ASELECT: " << startf << " " << hand << endl;
	//cout << "victim PID: " << nextVictim->PID << " vpage:" << nextVictim->Vpage << endl;
	return victim;
}