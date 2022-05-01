#ifndef PROCESS_H_
#define PROCESS_H_
#include <iostream>

using namespace std;

class Process
{
public:
	Process();
	Process(int pid, int at, int tc, int cb, int ib, int staticPrio);    // Constructor

	int getPID();			 //return PID
	int getTotalCPUTime();   // return TC
	int getCPUBurst();		 // return IO burst
	int getIOBurst();		 // return IO burst

	int getStaticPrio();     // return static priority
	int getDynamicPrio();    // return Dynamic priority
	int getCurrState();      // return current state;
	int getCurrStateTime();  // return time of process in current states
	int getNextTimeStamp();  // return next evt time
	int getTurnaround();	 // return total turnaround time
	int getCurrCPUBurst();   // return curr CPU Burst
	int getRemCPUBurst();    // return remain CPU burst
	int getTotalCPUWait();   // return total CPU waiting time
	int getRemain();		 // return remain execution time
	bool getQuatEndFlag();   // return quat end flag
	bool getExpiredFlag();   // return expired flag
	bool getPreemptFlag();   // return preempted Flag

	void setcurrIOBurst(int currCPUBurst);
	void setCurrCPUBurst(int currCPUBurst);
	void setRemCPUBurst(int nextCPUB);
	void setFinishTime(int fTime);
	void setWaitStartTime(int wSTime);
	void setWaitEndTime(int WETime);
	void setRemain(int nextCPUB);

	void setDynamicPrio(int dPrio);
	void setCurrState(int currState);
	void setCurrStateTime(int currTimeStamp);
	void setNextTimeStamp(int nextTimeStamp);

	void setquatEndFlag(bool qflag);
	void setExpiredFlag(bool eflag);
	void setPreemptFalg(bool pflag);
	void preemptDedyctTime(int dTime);
	void decrDPrio();
	void resetDynamicPrio();
	void printResult();

	int pid = 0;   
    int at = 0;       // arrive time
	int tc = 0;       // total cpu-time
	int cb = 0;       // CPU brust
	int ib = 0;       // IO brust
	int sPrio = 0;    // static priority    myrandom(maxprio)
	int dPrio = 0;    // dynamic priority  [ 0 .. (static_priority-1) ]

	int currIOBurst = 0;
	int currCPUBurst = 0;

	int remCPUBurst = 0;        // remain CPU burst (X)
	int remainTime = 0;         // remain total CPU time
	int finishTime = 0;         // finish time
	int totalIOtime = 0;        // total IO time
	int totalCPUtime = 0;       // total CPU time
	int totalCPUWaitTime = 0;   // total CPU wait time
	int waitingStartTime = 0;   // time in Ready state

	bool quatEndFlag = false;   // quantum end flag
	bool expiredFlag = false;   // quantum expired flag
	bool preemptedFalg = false; // proc beem preempt
	int currTimeStamp = 0;
	int nextTimeStamp = 0;
	int currState = CREATED;

	typedef enum process_State 
	{
		CREATED,  // 0. Created
		READY,    // 1. Ready
		RUNNG,    // 2. Runng
		BLOCK,    // 3. Block
		DONE      // 4. Done
	} ProcessState;

	~Process();

private:
};
#endif