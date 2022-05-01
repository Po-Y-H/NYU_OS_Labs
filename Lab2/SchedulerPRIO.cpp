#include "Scheduler.h"

SchedulerPRIO::SchedulerPRIO() {}

SchedulerPRIO::SchedulerPRIO(int maxPrio)
{
	this->maxprio = maxPrio;
	activeQueue = new vector<list<Process*>>(maxPrio);
	expiredQueue = new vector<list<Process*>>(maxPrio);
	/*
	  Queue<Process*> *activeQ = new Queue<Process*> [ maxprios ];
      Queue<Process*> *expiredQ = new Queue<Process*> [ maxprios ];
	*/
}

void SchedulerPRIO::addProcess(Process* proc)
{
	//cout << " add Proc PID: " << proc->getPID() << "getExoF: " << proc->getExpiredFlag() << " dPrio: " << proc->getDynamicPrio() << endl;
	
	int level = maxprio - 1 - proc->getDynamicPrio();
	if (proc->getExpiredFlag()) 
	{
		(*expiredQueue)[level].push_back(proc);
		proc->setExpiredFlag(false);
	}
	else 
	{
		(*activeQueue)[level].push_back(proc);
	}
	//printQueue();
	/*
	  if (dynamic prio < 0)
		reset and enter into expireQ
	  else
		add to activeQ
	*/
}

Process* SchedulerPRIO::getNextProcess()
{
	if (!isEmpty(activeQueue)) 
	{
		for (int i = 0; i < activeQueue->size(); i++) 
		{
			if (!(*activeQueue)[i].empty()) 
			{
				// pick activeQ[highest prio].front()
				Process* p = (*activeQueue)[i].front();
				(*activeQueue)[i].pop_front();
				//cout << "PID: " << p->getPID() << " dPrio: " << p->getDynamicPrio() << endl;
				return p;
			}
		}
	}

	// swap(activeQ,expiredQ) and try again
	if (isEmpty(expiredQueue)) { return nullptr; }
	else 
	{
		vector<list<Process*>>* t = expiredQueue;
		expiredQueue = activeQueue;
		activeQueue = t;
		return getNextProcess();
	}
	/*
	  if activeQ not empty 
	 	pick activeQ[highest prio].front()
      else
		swap(activeQ,expiredQ) and try again
		[ swapping means swapping the pointers to array 
		  not swapping the entire arrays .. Smart programming ]
	*/
}

bool SchedulerPRIO::isEmpty() { return false; }

bool SchedulerPRIO::isEmpty(vector<list<Process*>>* q)
{
	for (int i = 0; i < q->size(); i++) {
		if (!(*q)[i].empty()) {
			return false;
		}
	}
	return true;
}

bool SchedulerPRIO::testPreempt(Process* p, int currtime) // false but for ¡¥E¡¦    
{
	return false;
}

void SchedulerPRIO::printQueue() 
{
	vector<list<Process*>>& tmp1 = *activeQueue;
	vector<list<Process*>>& tmp2 = *expiredQueue;

	cout << "( ";
	for (auto q : tmp1)
	{
		cout << "[";
		for (auto it = q.begin(); it != q.end(); ++it)
		{
			if (it != q.begin())
			{
				cout << ",";
			}
			Process* proc = *it;
			cout << proc->getPID();
		}
		cout << "]";
	}
	cout << " ) : ( ";
	for (auto q : tmp2)
	{
		cout << "[";
		for (auto it = q.begin(); it != q.end(); ++it)
		{
			if (it != q.begin())
			{
				cout << ",";
			}
			Process* proc = *it;
			cout << proc->getPID();
		}
		cout << "]";
	}
	cout << " )" << endl;

}

SchedulerPRIO::~SchedulerPRIO() {}