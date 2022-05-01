#include "Scheduler.h"

SchedulerRR::SchedulerRR() {}

void SchedulerRR::addProcess(Process* proc)
{
	readyQueue.push(proc);
}

Process* SchedulerRR::getNextProcess()
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

bool SchedulerRR::isEmpty()
{
	return readyQueue.size() == 0;
}

bool SchedulerRR::testPreempt(Process* p, int currtime) // false but for ¡¥E¡¦    
{
	return false;
}

SchedulerRR::~SchedulerRR() {}