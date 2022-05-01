#include "Scheduler.h"

SchedulerSRTF::SchedulerSRTF() {}

void SchedulerSRTF::addProcess(Process* proc)
{
	list<Process*>::iterator it;
	for (it = readyQueue.begin(); it != readyQueue.end(); ++it) 
	{
		if (proc->getRemain() < (*it)->getRemain()) 
		{
			break;
		}
	}
	readyQueue.insert(it, proc);
}

Process* SchedulerSRTF::getNextProcess()
{
	if (isEmpty())
	{
		return nullptr;
	}
	Process* p = readyQueue.front();
	//cout << "front proc: " << p->getPID() << endl;
	readyQueue.pop_front();
	return p;
}

bool SchedulerSRTF::isEmpty()
{
	return readyQueue.size() == 0;
}

bool SchedulerSRTF::testPreempt(Process* p, int currtime) // false but for ¡¥E¡¦    
{
	return false;
}

SchedulerSRTF::~SchedulerSRTF() {}