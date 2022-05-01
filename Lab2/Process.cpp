#include "Process.h"

Process::Process() {}

Process::Process(int pid, int at, int tc, int cb, int ib, int staticPrio)
{
	this->pid = pid;
	this->at = at;
	this->tc = tc;
	this->cb = cb;
	this->ib = ib;
	this->sPrio = staticPrio;
	this->dPrio = staticPrio - 1;
	this->remainTime = tc;
	this->currTimeStamp = at;
}

//Getter
int Process::getPID() { return pid; }

int Process::getTotalCPUTime() { return tc; }

int Process::getCPUBurst() { return cb; }

int Process::getIOBurst() { return ib; }

int Process::getStaticPrio() { return sPrio; }

int Process::getDynamicPrio() { return dPrio; }

int Process::getCurrState() { return currState; }    // return current state;

int Process::getCurrStateTime() { return currTimeStamp; }    // return time of process in current states

int Process::getNextTimeStamp() { return nextTimeStamp; }  // return next evt time

int Process::getRemain() { return remainTime; }    // return remain execution time

int Process::getTurnaround() { return finishTime - at; } // return total turnaround time

int Process::getCurrCPUBurst() { return currCPUBurst; }   // return curr CPU Burst

int Process::getRemCPUBurst() { return remCPUBurst; }    // return remain CPU burst

int Process::getTotalCPUWait() { return totalCPUWaitTime; }   // return total CPU waiting time

bool Process::getQuatEndFlag() { return quatEndFlag; }

bool Process::getExpiredFlag() { return expiredFlag; }

bool Process::getPreemptFlag() { return preemptedFalg; }   // return preempted Flag

//Setter
void Process::setcurrIOBurst(int currIOBurst) 
{ 
	this->currIOBurst = currIOBurst; 
	totalIOtime += currIOBurst;
}

void Process::setCurrCPUBurst(int currCPUBurst) 
{ 
	this->remCPUBurst = currCPUBurst;
	totalCPUtime += currCPUBurst;
}

void Process::setRemCPUBurst(int nextCPUB)
{
	this->currCPUBurst = nextCPUB;
	remCPUBurst -= nextCPUB;
}

void Process::setWaitStartTime(int wSTime) { waitingStartTime = wSTime; }

void Process::setWaitEndTime(int WETime) 
{ 
	this->totalCPUWaitTime += (WETime - waitingStartTime); 
	//cout << "WETime: " << WETime << " waitingStartTime: " << waitingStartTime << endl;
	//cout << "totalCPUWaitTime : " << totalCPUWaitTime << endl;
}

void Process::setFinishTime(int fTime) { this->finishTime = fTime; }

void Process::setRemain(int nextCPUB) { this->remainTime -= nextCPUB; }

void Process::setDynamicPrio(int dPrio) { this->dPrio = dPrio; }

void Process::setCurrStateTime(int currTimeStamp) { this->currTimeStamp = currTimeStamp; }

void Process::setNextTimeStamp(int nextTimeStamp) { this->nextTimeStamp = nextTimeStamp; }

void Process::setCurrState(int newState)
{
	switch (newState)
	{
		case CREATED:
			this->currState = CREATED;
			break;
		case READY:
			this->currState = READY;
			break;
		case RUNNG:
			this->currState = RUNNG;
			break;
		case BLOCK:
			this->currState = BLOCK;
			break;
		case DONE:
			this->currState = BLOCK;
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

void Process::setquatEndFlag(bool qflag) { quatEndFlag = qflag; }

void Process::setExpiredFlag(bool eflag) { expiredFlag = eflag; }

void Process::setPreemptFalg(bool pflag) { preemptedFalg = pflag; }

void Process::resetDynamicPrio() 
{ 
	this->dPrio = sPrio - 1; 
	//cout << " -- reset dPrio -- pid: " << pid << " dPrio: " << dPrio << endl;
}

void Process::preemptDedyctTime(int dTime)
{
	remainTime = (remainTime + currCPUBurst) - dTime;
	remCPUBurst = (remCPUBurst + currCPUBurst) - dTime;
}

void Process::decrDPrio()
{
	dPrio--; 
	if (dPrio < 0)
	{
		expiredFlag = true;    // add proc to expired queue
		dPrio = sPrio - 1;     // reset dynamic prioroty to [sPrio -1]
	}
	//cout << " -- Decr dprio pid: " << pid << " dprio: " << dPrio << endl;
}

void Process::printResult()
{
	int turAroundTime = getTurnaround();
	printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", 
		    pid, at, tc, cb, ib, sPrio, finishTime, turAroundTime, totalIOtime, totalCPUWaitTime);
/*
	Per process information(see below) :
	for each process(assume processes start with pid = 0), the correct desired format is shown below :
	pid: AT TC CB IO PRIO | FT TT IT CW
	FT : Finishing time
	TT : Turnaround time(finishing time - AT)
	IT : I / O Time(time in blocked state)
	PRIO : static priority assigned to the process(note this only has meaning in PRIO / PREPRIO case)
	CW : CPU Waiting time(time in Ready state)
*/
}

Process::~Process() {}