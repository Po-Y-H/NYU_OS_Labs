#ifndef EVENT_H
#define EVENT_H 
#include <string>

#include "Process.h"

using namespace std;

class Event
{
public:
	Event();
	Event(Process* proc, int time, int evtType);  //constructor

	Process* getEventProcess();
	int getEventType();
	int getEventTimeStamp();
	string printEventType();

	Process* evtProcess;
	int evtTimeStamp;
	int eventType;
	int oldState = 0;
	int newState = 0;
	string evtTypeStr;

	typedef enum event_Type
	{
		CREATED,           // 0.
		CREATED_to_READY,    // 1. create -> ready
		BLOCK_to_READY,      // 2. blocked -> ready
		READY_to_RUNNG,	   // 3. ready -> running
		RUNNG_to_BLOCK,      // 4. running -> blocked
		RUNNG_to_READY,	   // 5. running -> ready (preemption)
		RUNNG_to_END        // 6. End
	} eventTypeE;

	~Event();
private:
};
#endif