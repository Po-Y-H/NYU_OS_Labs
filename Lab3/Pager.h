#pragma once
#ifndef PAGER_H
#define PAGER_H
#include <iostream>
#include <vector>
#include <array>

#include "Process.h"
#include "Frame.h"

using namespace std;

#define MAX_FRAMES 128

/* Algorithm
FIFO : F
Random : R
Clock : C
Enhanced Second Chance / NRU : E
Aging : A
Working Set : W
*/

class Pager										// Base class
{
public:
	virtual FRAME* selectVictimFrame(FRAME (&frameTable)[MAX_FRAMES]) = 0;  // virtual base class
};

class PagerFIFO : public Pager // F
{
public:
	PagerFIFO();
	PagerFIFO(int ftsize);
	FRAME* selectVictimFrame(FRAME (&frameTable)[MAX_FRAMES]);

	int sizeOfFT;
	int count;
};

class PagerRandom : public Pager // R
{
public:
	PagerRandom();
	PagerRandom(int ftsize, vector<int> randVals);
	FRAME* selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES]);

	int ofs = 0;
	int sizeOfFT;
	int count;
	vector<int> randVec;
};

class PagerClock : public PagerFIFO // C
{
public:
	PagerClock();
	PagerClock(int ftsize, vector<Process*> &procsVec);
	FRAME* selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES]);

	int hand;
	vector<Process*> procsV;
};

class PagerNRU : public Pager  // E
{
public:
	PagerNRU();
	PagerNRU(int ftsize, vector<Process*>& procsVec, unsigned long long& instrcCount);
	FRAME* selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES]);
	void printLC();
	
	int sizeOfFT;

	int lowestClass[4];
	int hand;
	unsigned long long lastResetCount;
	unsigned long long* currInstrcCount;
	vector<Process*> procsV;
};

class PagerAging : public Pager  // A
{
public:
	PagerAging();
	PagerAging(int ftsize, vector<Process*>& procsVec);
	FRAME* selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES]);
	
	int sizeOfFT;

	int hand;
	vector<Process*> procsV;
};

class PagerWorkingSet : public Pager // W
{
public:
	PagerWorkingSet();
	PagerWorkingSet(int ftsize, vector<Process*>& procsVec, unsigned long long& instrcCount);
	FRAME* selectVictimFrame(FRAME(&frameTable)[MAX_FRAMES]);
	
	int sizeOfFT;

	int hand;
	unsigned long long* currInstrcCount;
	vector<Process*> procsV;
};
#endif // !PAGER_H
