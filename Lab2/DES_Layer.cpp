#include "DES_Layer.h"

Event* DES_Layer::getEvent()   // return Event
{
	//printEventQueue();
	return eventQueue.front();
}

void DES_Layer::putEventDES(Event* newEvt, int newEvtTime)
{
	//cout << "put evt DES: " << newEvt->getEventTimeStamp() << " " << newEvt->getEventType() << endl;
	list<Event*>::iterator it;
	for (it = eventQueue.begin(); it != eventQueue.end(); ++it)
	{
		if (newEvtTime < (*it)->getEventTimeStamp())
		{
			//cout << "breaK" << endl;
			//cout << newEvtTime << " " << (*it)->getEventTimeStamp() << endl;
			break;
		}
	}
	eventQueue.insert(it, newEvt);
	newEvt->getEventProcess()->setNextTimeStamp(newEvtTime);    // current process's next event time stamp
	//cout << "----- after put evt" << endl;
	//printEventQueue();
}

void DES_Layer::preRmEvent(Process* rmProc, int time)
{
	list<Event*>::iterator it;
	int rmEvtPID = rmProc->getPID();
	//cout << " target evt pid: " << rmEvtPID << " target time:  " << time << endl;
	for (it = eventQueue.begin(); it != eventQueue.end(); ++it)
	{
		//cout << " ts: " << (*it)->getEventTimeStamp() << " pid: " << (*it)->getEventProcess()->getPID() << endl;
		if (time == (*it)->getEventTimeStamp() && rmEvtPID == (*it)->getEventProcess()->getPID())
		{
			//cout << " --- rm evt --- " << endl;
			//cout << " itr time: " << time << " " << (*it)->getEventTimeStamp() << endl;
			eventQueue.erase(it);
			break;
		}
	}
	//cout << "----- after rm evt" << endl;
	//printEventQueue();
}

void DES_Layer::rmEvent()
{
	eventQueue.pop_front();
}

int DES_Layer::isEmpty()
{
	return eventQueue.empty();
}

int DES_Layer::getNextEvtTime()
{
	if (isEmpty())
	{
		return -1;
	}
	else
	{
		Event* nextEvt = eventQueue.front();
		int nextEvtTime = nextEvt->getEventTimeStamp();
		return nextEvtTime;
	}
}

void DES_Layer::setCurrTime(int currTime) { this->currTime = currTime; }

void DES_Layer::setIOBusyTime(int IOBTime) { IOBusyTime = IOBTime; }

void DES_Layer::setCPUBusyTime(int CPUBTime) { CPUBusyTime = CPUBTime; }

void DES_Layer::setFinishTime(int fTime) { this->desFinishTime = fTime; }

void DES_Layer::setNumProc(int num) { this->numProcs = num; }

void DES_Layer::printSummary(int sumTurnaround,int sumCPUWaiting)
{
	double cpu_til = 100.0 * (CPUBusyTime / (double)desFinishTime);
	double io_util = 100.0 * (IOBusyTime / (double)desFinishTime);
	double att = double(sumTurnaround) / numProcs;
	double acwt = double(sumCPUWaiting) / numProcs;
	double throughput = 100.0 * (numProcs / (double)desFinishTime);
	
	printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", desFinishTime, cpu_til, io_util, att, acwt, throughput);
	/*
		Summary Information - Finally print a summary for the simulation:
		1. Finishing time of the last event (i.e. the last process finished execution)
		2. CPU utilization (i.e. percentage (0.0 �V 100.0) of time at least one process is running
		3. IO utilization (i.e. percentage (0.0 �V 100.0) of time at least one process is performing IO
		4. Average turnaround time among processes
		5. Average cpu waiting time among processes
		6. Throughput of number processes per 100 time units
	*/
}

void DES_Layer::printEventQueue()
{
	list<Event*>::iterator it;
	for (it = eventQueue.begin(); it != eventQueue.end(); ++it)
	{
		cout << "ts: " << (*it)->getEventTimeStamp() << " pid: " << (*it)->getEventProcess()->getPID() 
			<< " evtType: " << (*it)->getEventType() << endl;
	}
}