#include "Event.h"

Event::Event() {}

Event::Event(Process* proc, int time, int evtType) //constructor
{
	this->evtTimeStamp = time;
	this->evtProcess = proc;
	switch (evtType)
	{
		case event_Type::CREATED:  // 0. create
			//CREATED
			this->eventType = 0;
			this->newState = 1;
			this->evtTypeStr = "CREATED";
			break;
		case event_Type::CREATED_to_READY:  // 1. create -> ready
			this->eventType = 1;
			this->oldState = 0,
			this->newState = 1;
			this->evtTypeStr = "CREATED->READY";
			break;
		case event_Type::BLOCK_to_READY:  // 2. blocked -> ready
			this->eventType = 2;
			this->oldState = 3,
			this->newState = 1;
			this->evtTypeStr = "BLOCK->READY";
			break;
		case event_Type::READY_to_RUNNG:  // 3. ready -> running
			this->eventType = 3;
			this->oldState = 1,
			this->newState = 2;
			this->evtTypeStr = "READY->RUNNG";
			break;
		case event_Type::RUNNG_to_BLOCK:  // 4. running -> blocked
			this->eventType = 4;
			this->oldState = 2,
			this->newState = 3;
			this->evtTypeStr = "RUNNG->BLOCK";
			break;
		case event_Type::RUNNG_to_READY:  // 5. running -> ready (preemption)
			this->eventType = 5;
			this->oldState = 2,
			this->newState = 1;
			this->evtTypeStr = "RUNNG->READY";
			break;
		case event_Type::RUNNG_to_END:  // 6. End
			this->eventType = 6;
			this->oldState = 2,
			this->newState = 4;
			this->evtTypeStr = "Done";
			break;
	}
	/*
	    CREATED,  // 0. Created
		READY,    // 1. Ready
		RUNNG,    // 2. Runng
		BLOCK,    // 3. Block
		DONE      // 4. Done
	*/
}

Process* Event::getEventProcess() 
{
	return evtProcess;
}

int Event::getEventType()
{
	return eventType;
}

int Event::getEventTimeStamp() 
{
	return evtTimeStamp;
}

string Event::printEventType()
{
	return evtTypeStr;
}

Event::~Event() {}
