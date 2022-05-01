#include "Scheduler.h"

SchedulerFCFS::SchedulerFCFS() {}

void SchedulerFCFS::addProcess(Process* proc)
{
	readyQueue.push(proc);
}

Process* SchedulerFCFS::getNextProcess()
{
	if (isEmpty())
	{
		return nullptr;
	}
	Process* p = readyQueue.front();
	//cout << "front proc: " << p->getPID() << endl;
	readyQueue.pop();
	return p;
}

bool SchedulerFCFS::isEmpty()
{
	return readyQueue.size() == 0;
}

bool SchedulerFCFS::testPreempt(Process* p, int currtime) // false but for ¡¥E¡¦    
{
	return false;
}

SchedulerFCFS::~SchedulerFCFS() {}