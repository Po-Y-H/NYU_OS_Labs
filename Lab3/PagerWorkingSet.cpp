#include "Pager.h"

PagerWorkingSet::PagerWorkingSet() {}

PagerWorkingSet::PagerWorkingSet(int ftsize, vector<Process*>& procsVec, unsigned long long& instrcCount)
{
	this->sizeOfFT = ftsize;

	this->hand = 0;
	//this->timelastuse = 0;
	this->currInstrcCount = &instrcCount;
	this->procsV = procsVec;
}

FRAME* PagerWorkingSet::selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES])
{
	int pid = 0;
	int vpage = 0;
	int rbit = 0;
	int tau = 0;
	int timelastuse = 0;
	int minimuntau = -1;

	FRAME* victim = &frameTable[hand];
	FRAME* handFrame = &frameTable[hand];
	
	//cout << "curr Instr count: " << (*currInstrcCount) << endl;

	for (int i = 0; i < sizeOfFT; i++)
	{
		int id = (i + hand) % sizeOfFT;
		handFrame = &frameTable[id];
		pid = handFrame->PID;
		vpage = handFrame->Vpage;

		PTE* p = procsV[pid]->getPTE(vpage);
		rbit = p->referenced;
		//cout << " minimun : " << minimuntau << endl;

		timelastuse = ((*currInstrcCount) - 1) - handFrame->tau;
		//tau = handFrame->tau;
		//# (refbit, proc:vpage, timelastuse) 
		//cout << "hand: " << hand  << " ( " << rbit << " " << pid << " : " << vpage << " tau: " << handFrame->tau << " ) tlu : " << timelastuse << endl;
		if (rbit) 
		{
			p->referenced = 0;
			handFrame->tau = (*currInstrcCount) - 1;
		}
		else 
		{
			// 1. 50 or more instructions have passed since the time of ¡§last use¡¨ 
			// 2. frame and the reference bit is not set
			if (timelastuse >= 50)
			{
				victim = handFrame;
				victim->victim = true;
				hand = (victim->frameNum + 1) % sizeOfFT;
				//cout << " > 50 : " << victim->Vpage << endl;
				return victim;
			}
			else // select the first frame with minimum tau
			{
				//cout << "timelastuse: " << timelastuse << " minimuntau: " << minimuntau << endl;
				if (timelastuse > minimuntau)
				{
					minimuntau = timelastuse;
					//cout << " minimun : " << minimuntau << endl;
					victim = handFrame;
				}
			}
		}
	}
	hand = (victim->frameNum + 1) % sizeOfFT;
	victim->victim = true;
	return victim;
}
