#include "Pager.h"

PagerNRU::PagerNRU() {}

PagerNRU::PagerNRU(int ftsize, vector<Process*>& procsVec, unsigned long long& instrcCount)
{
	this->sizeOfFT = ftsize;

	this->hand = 0;
	this->lastResetCount = -1;
	this->currInstrcCount = &instrcCount;
	this->procsV = procsVec;

	for (int i = 0; i < 4; i++)
	{
		lowestClass[i] = -1;
		//cout << lowestClass[i] << " ";
	}
}

FRAME* PagerNRU::selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES])
{
	int pid = 0;
	int vpage = 0;
	int rbit = 0;
	int mbit = 0;
	int classOrd = 0;
	int lowestClass[4];

	int currInstrOrd = (*currInstrcCount);
	//cout << "inc: " << currInstrOrd << endl;
	bool checkClass = false;
	FRAME* victim = &frameTable[hand];
	FRAME* handFrame = &frameTable[hand];
	
	for (int i = 0; i < 4; i++)
	{
		lowestClass[i] = -1;
		//cout << lowestClass[i] << " ";
	}

	//cout << "curr Instr count: " << currInstrOrd << endl;
	//cout << "last InstrcCount: " << lastResetCount << endl;

	for (int i = 0; i < sizeOfFT; i++)
	{
		int id  = (i + hand) % sizeOfFT;
		handFrame = &frameTable[id];
		pid = handFrame->PID;
		vpage = handFrame->Vpage;
		
		PTE* p = procsV[pid]->getPTE(vpage);
		rbit = p->referenced;
		mbit = p->modified;

		if (rbit == 0 && mbit == 0) classOrd = 0;
		else if (rbit == 0 && mbit == 1) classOrd = 1;
		else if (rbit == 1 && mbit == 0) classOrd = 2;
		else if (rbit == 1 && mbit == 1) classOrd = 3;
		//cout << vpage << " r: " << rbit << " mbit: " << mbit << " ord: " << classOrd << endl;

		if (lowestClass[classOrd] == -1)
		{
			//cout << "vpage in class : " << vpage << " classOrd: " << classOrd << endl;
			//lowestClass[classOrd] = handFrame->frameNum;
			if (classOrd == 0)
			{
				//cout << "STOP class pid: " << pid << " vpage: " << vpage << " classOrd: " << classOrd << endl;
				victim = handFrame;
				victim->victim = true;
				checkClass = true;
				hand = (id + 1) % sizeOfFT;
				//cout << "hand num: " << hand << endl;
				break;
			}
			else 
			{
				//cout << "vpage in class pid: " << pid << " vpage: " << vpage << " classOrd: " << classOrd << endl;
				lowestClass[classOrd] = handFrame->frameNum;
			}
		}
	}
	//printLC();
	if (!checkClass)
	{
		// select victim from lowest class
		// hand is set to the next position of victim frame
		for (int i = 0; i < 4; i++)
		{
			//printLC();
			if (lowestClass[i] != -1) 
			{
				victim = &frameTable[lowestClass[i]];
				victim->victim = true;
				checkClass = true;
				hand = (victim->frameNum + 1) % sizeOfFT;
				//cout << "vic frame num: " << victim->frameNum << " hand : " << hand << endl;
				//lowestClass[i] = -1;	// resert
				break;
			}
		}
	}
	// reset reference bit if instrc counter >= 50
	if ((currInstrOrd - lastResetCount)>= 50)
	{
		for (int i = 0; i < sizeOfFT; i++) 
		{
			FRAME* f = &frameTable[i];
			if (f->PID != -1) 
			{
				procsV[f->PID]->pageTable[f->Vpage].referenced = 0;
			}
		}
		lastResetCount = currInstrOrd;
		//cout << "resert ref " << endl;
	}
	//cout << "hand : " << hand << endl;
	//printLC();

	//cout << "end select; vapge: " << victim->Vpage << endl;
	return victim;
}

void PagerNRU::printLC() 
{
	//cout << " hand : " << hand << endl;
	for (int i = 0; i < 4; i++)
	{
		cout << lowestClass[i] << " ";
	}
	cout << endl;
}
