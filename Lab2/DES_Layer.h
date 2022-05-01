#ifndef DES_LAYER_H
#define DES_LAYER_H 
#include <list>
#include <iostream>

#include "Event.h"

class DES_Layer
{
public:
	Event* getEvent();
	void putEventDES(Event* newEvt, int time);    // add new event into eventQueue
	void preRmEvent(Process* rmProc, int time);   // Event* evt (preemption remove event)
	void rmEvent();
	int isEmpty();
	int getNextEvtTime();

	void setCurrTime(int currTime);
	void setIOBusyTime(int IOBusyTime);
	void setCPUBusyTime(int CPUBusyTime);
	void setFinishTime(int fTime);
	void setNumProc(int num);

	void printSummary(int sumTurnaround, int sumCPUWaiting);
	void printEventQueue();
	
	int currTime = 0;
	int desFinishTime = 0;
	int IOBusyTime = 0;
	int CPUBusyTime = 0;
	int numProcs = 0;

	list<Event*> eventQueue;
};
#endif