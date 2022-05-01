#pragma once
#include <iostream>
#include <vector>

#include "IOoperation.h"

using namespace std;

class Scheduler    // virtual base class
{
public:
	virtual IOoperation strategy() = 0;

};

class FIFOScheduler : public Scheduler		  // (i)
{
public:
	FIFOScheduler();
	FIFOScheduler(vector<IOoperation> &IOq);
	IOoperation strategy();

	vector<IOoperation> *IOqueue;
};


class SSTFScheduler : public Scheduler		 // (j) 
{
public:
	SSTFScheduler();
	SSTFScheduler(vector<IOoperation>& IOq, int &currPos);
	IOoperation strategy();

	vector<IOoperation>* IOqueue;
	int* currPos;
};

class LOOKScheduler : public Scheduler		// (s)
{
public:
	LOOKScheduler();
	LOOKScheduler(vector<IOoperation>& IOq, int& currPos, bool& trackDir);
	IOoperation strategy();

	vector<IOoperation>* IOqueue;
	int* currPos;
	bool* trackDir;
};
											
class CLOOKScheduler : public Scheduler		// (c)
{
public:
	CLOOKScheduler();
	CLOOKScheduler(vector<IOoperation>& IOq, int& currPos, bool& trackDir);
	IOoperation strategy();

	vector<IOoperation>* IOqueue;
	int* currPos;
	bool* trackDir;
};

class FLOOKScheduler : public Scheduler		// (f) 
{
public:
	FLOOKScheduler();
	FLOOKScheduler(vector<IOoperation>& IOq, vector<IOoperation>& IOq2, int& currPos, bool& trackDir, bool& queueSwapped);
	IOoperation strategy();

	vector<IOoperation>* IOqueue;
	vector<IOoperation>* IOqueue2;
	int* currPos;
	bool* trackDir;
	bool* swapQ;
	bool currSwap;
};
