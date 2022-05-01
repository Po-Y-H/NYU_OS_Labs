#include "Pager.h"

PagerAging::PagerAging() {}

PagerAging::PagerAging(int ftsize, vector<Process*>& procsVec)
{
	this->sizeOfFT = ftsize;

	this->hand = 0;
	this->procsV = procsVec;
}

FRAME* PagerAging::selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES])
{
	int pid = 0;
	int vpage = 0;
	int rbit = 0;

	FRAME* victim = &frameTable[hand];
	FRAME* handFrame = &frameTable[hand];

	for (int i = 0; i < sizeOfFT; i++)
	{
		int id  = (i + hand) % sizeOfFT;
		handFrame = &frameTable[id];
		pid = handFrame->PID;
		vpage = handFrame->Vpage;

		PTE* p = procsV[pid]->getPTE(vpage);
		rbit = p->referenced;

		handFrame->age = handFrame->age >> 1;

		if (rbit == 1) 
		{
			handFrame->age = (handFrame->age | 0x80000000);
			p->referenced = 0;
		}

		if (handFrame->age < victim->age)
		{
			victim = handFrame;
		}
	}

	hand = (victim->frameNum + 1) % sizeOfFT;
	victim->victim = true;
	return victim;
}
