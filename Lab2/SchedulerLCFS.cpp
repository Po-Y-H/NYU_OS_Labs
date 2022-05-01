#include "Scheduler.h"

SchedulerLCFS::SchedulerLCFS() {}

void SchedulerLCFS::addProcess(Process* proc)
{
	readyQueue.push_back(proc);
}

Process* SchedulerLCFS::getNextProcess()
{
	if (isEmpty())
	{
		return nullptr;
	}
	Process* p = readyQueue.back();
	//cout << "front proc: " << p->getPID() << endl;
	readyQueue.pop_back();
	return p;
}

bool SchedulerLCFS::isEmpty()
{
	return readyQueue.size() == 0;
}

bool SchedulerLCFS::testPreempt(Process* p, int currtime) // false but for ¡¥E¡¦    
{
	return false;
}

SchedulerLCFS::~SchedulerLCFS() {}