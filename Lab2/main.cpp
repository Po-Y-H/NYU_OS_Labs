#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "DES_Layer.h"
#include "Event.h"
#include "Process.h"
#include "Scheduler.h"

using namespace std;

void Simulation(DES_Layer* desLayer, Scheduler* sched);
void putEvent(DES_Layer* des, Process* proc, int time, int eventType);
void printResult(DES_Layer* desLayer);

void readInFile(string fileName, DES_Layer* desLayer);
void readRFile(string fileName);
void readExeFormat(int argc, char* argv[], string& exeStr);
int getRandom(int burst);
Scheduler* selectScheduler(string& exeStr);


static ifstream inFile;				// input file
static ifstream rFile;				// randon number file
static vector<string> exeFormat;    // Execution and Invocation Format
static vector<int> randVals;		// random values from rfile
static list<Process*> processList;  // Processes

static int ofs = 0;
static int maxprios = 4;            // default = 4
static int quantum = 10000;         // default: 10k
static bool v = false;
static bool e = false;
static bool t = false;

void Simulation(DES_Layer* DesLayer, Scheduler* sched)
{
	Event* evt= nullptr;
	Scheduler* scheduler = sched;
	//scheduler = new SchedulerPREPRIO(maxprios);    // select scheduler selectScheduler()

	int currTime = 0;
	int nextEvtTime = 0;
	int CPUStatus = 0;     // CPU runng or waiting
	int IOStartTime = 0;   // beginning time for current process enters I/O
	int IOBusyTime = 0;    // total IO busy time
	int numIOProc = 0;     // number of process in IO 
	int remCPUBurst = 0;

	int evtCPUBurst;
	int evtIOBurst;
	
	bool callScheduler = false;         
	Process* currRunngProcess = nullptr;
	
	//cout << maxprios << " " << quantum << endl;

	/*
	CREATED,           // 0.
	CREATED->READY,    // 1. create -> ready
	BLOCK->READY,      // 2. blocked -> ready
	READY->RUNNG	   // 3. ready -> running
	RUNNG->BLOCK,      // 4. running -> blocked
	RUNNG->READY,	   // 5. running -> ready (preemption)
	RUNNG->END         // 6. End
	*/

	while (!DesLayer->isEmpty())
	{
		//cout << "ofs: " << ofs << endl;
		evt = DesLayer->getEvent();
		Process* currProc = evt->getEventProcess();    // this is the process the event works on
		currTime = evt->getEventTimeStamp();
		DesLayer->setCurrTime(currTime);
		int timeInPrevState = currTime - currProc->getCurrStateTime();
		bool preemptDPrioFlag = false;                 // cond 1. 
		bool preemptTimeflag = false;                  // cond 2.
		int preemptRunngTime = 0;                      // current process runng time before preemption
		int preemptDedyctTime = 0;                     // time decrease for preempted runng process
		int currRunProcNextTime = 0;                   // next time stamp of current runng process

		switch (evt->getEventType())
		{
		case 0:  // 0. create
			// create a new event: TRANS_TO_READY
			putEvent(DesLayer, currProc, currTime, 1);

			// print create
			//cout << currTime << " " << currProc->getPID() << ": " << " CREATED " << endl;
			break;

		case 1:  // 1. create -> ready 
			// must come from BLOCKED or from PREEMPTION
			// must add to run queue			
			// print create -> ready
			if (v)
			{
				cout << currTime << " " << currProc->getPID() << " 0: " << evt->printEventType() << endl;
			}

			// PREEMPTION
			if (scheduler->testPreempt(currProc, currTime) && currRunngProcess != nullptr)
			{
				currRunProcNextTime = currRunngProcess->getNextTimeStamp();
				// 1. unblocking process��s dynamic priority is higher than the currently running processes dynamic priority
				preemptDPrioFlag = currProc->getDynamicPrio() > currRunngProcess->getDynamicPrio();
				if (v)
				{
					cout << " currProc: " << currProc->getPID() << " dPrio : " << currProc->getDynamicPrio() << " : "
						<< " currRunngProc : " << currRunngProcess->getPID() << " dprio: " << currRunngProcess->getDynamicPrio() << endl;
				}
				// 2. currently running process does not have an event pending for the same time stamp
				preemptTimeflag = currRunProcNextTime != currTime;
				//cout << "pre Time : " << currRunProcNextTime << " : " << currTime << endl;

				//cout << "DPrio Flag: " << preemptDPrioFlag << " Time flag : " << preemptTimeflag << endl;
				if (preemptDPrioFlag && preemptTimeflag)
				{
					if (v)
					{
						cout << "---> PRIO preemption from : " << currRunngProcess->getPID() << " to: " << currProc->getPID()
							<< " next evt t : " << currRunProcNextTime << " cur t " << currTime << endl;
					}

					// remove
					DesLayer->preRmEvent(currRunngProcess, currRunProcNextTime);
					
					// put preemption evt
					putEvent(DesLayer, currRunngProcess, currTime, 5);       //add evt 5.running -> ready (preemption)
					preemptDedyctTime = currTime - currRunngProcess->getCurrStateTime();
					//cout << "preemptDedyctTime: " << preemptDedyctTime << endl;
					currRunngProcess->setquatEndFlag(true);			         // curr Runng Process quantum not end
					currRunngProcess->preemptDedyctTime(preemptDedyctTime);  // edit process runng properties
					//currRunngProcess->setPreemptFalg(true);                // reset d Prio to highest
					//currRunngProcess->resetDynamicPrio();                    // reset d Prio to highest
					//cout << "end preempt" << endl;
				}
			}

			// update process control block
			currProc->setCurrState(1);				      // set current process state to ready
			currProc->setCurrStateTime(currTime);         // set current process time
			currProc->setWaitStartTime(currTime);		  // set waiting start time

			scheduler->addProcess(currProc);              // conditional on whether something is run
			callScheduler = true;
			break;

		case 2:    // 2. blocked -> ready
			// must come from BLOCKED or from PREEMPTION
			// must add to run queue
			// print blocked -> readys
			if (v)
			{
				cout << currTime << " " << currProc->getPID() << " " << timeInPrevState << ": " << evt->printEventType() << endl;
			}

			// no process in IO state
			numIOProc--;
			//cout << "numIOPro decs: " << numIOProc << endl;
			if (numIOProc == 0)
			{
				IOBusyTime += currTime - IOStartTime;
				//cout << IOBusyTime << " " << currTime << " " << IOStartTime << endl;
			}
			currProc->resetDynamicPrio();			      // back from IO, reset dynamic prio 

			// PREEMPTION
			if (scheduler->testPreempt(currProc, currTime) && currRunngProcess != nullptr)
			{
				currRunProcNextTime = currRunngProcess->getNextTimeStamp();
				// 1. unblocking process��s dynamic priority is higher than the currently running processes dynamic priority
				preemptDPrioFlag = currProc->getDynamicPrio() > currRunngProcess->getDynamicPrio();
				if (v) 
				{
					cout << " currProc: " << currProc->getPID() << "pre DPrio : " << currProc->getDynamicPrio() << " : "
						<< " currRunngProc : " << currRunngProcess->getPID() << " dprio: " << currRunngProcess->getDynamicPrio() << endl;
				}
				
				// 2. currently running process does not have an event pending for the same time stamp
				preemptTimeflag = currRunProcNextTime != currTime;
				//cout << "pre Time : " << currRunProcNextTime << " : " << currTime << endl;

				//cout << "DPrio Flag: " << preemptDPrioFlag << " Time flag : " << preemptTimeflag << endl;
				if (preemptDPrioFlag && preemptTimeflag)
				{
					if (v)
					{
						cout << "---> PRIO preemption from : " << currRunngProcess->getPID() << " to: " << currProc->getPID()
							<< " next evt t : " << currRunProcNextTime << " cur t " << currTime << endl;
					}

					// remove
					DesLayer->preRmEvent(currRunngProcess, currRunProcNextTime);
					// put preemption evt
					putEvent(DesLayer, currRunngProcess, currTime, 5);       //add evt 5.running->ready(preemption)
					preemptDedyctTime = currTime - currRunngProcess->getCurrStateTime();
					//cout << "preemptDedyctTime: " << preemptDedyctTime << endl;
					currRunngProcess->setquatEndFlag(true);			         // curr Runng Process quantum not end
					currRunngProcess->preemptDedyctTime(preemptDedyctTime);  // edit process runng properties
					//currRunngProcess->setPreemptFalg(true);                // reset d Prio to highest
					//cout << "end preempt" << endl;
				}
			}

			// update process control block
			currProc->setCurrState(1);					  // set current process state to ready
			currProc->setCurrStateTime(currTime);         // set current process time
			currProc->setWaitStartTime(currTime);		  // set waiting start time
			
			//cout << " reset Proc PID: " << currProc->getPID() << " dPrio: " << currProc->getDynamicPrio() << endl;
			scheduler->addProcess(currProc);              // conditional on whether something is run			
			callScheduler = true;
			break;

		case 3:  // 3. ready -> running
			// create event for 4.blocking or 5.preemption or 6.done
			currRunngProcess = currProc;
			remCPUBurst = currProc->getRemCPUBurst();
			/*
			// preempted process get full quantum
			if (currRunngProcess->getPreemptFlag()) 
			{
				currRunngProcess->resetDynamicPrio();    // reset d Prio to highest
				currRunngProcess->setPreemptFalg(false);
			}
			*/
			//cout << "remCPUBurst: " << remCPUBurst << endl;
			if (remCPUBurst == 0)
			{
				evtCPUBurst = getRandom(currProc->getCPUBurst());
				currProc->setCurrCPUBurst(evtCPUBurst);              // set runng period to process
			}
			else
			{
				evtCPUBurst = remCPUBurst;
			}
			
			//cout << "evtCPUBurst: " << evtCPUBurst << endl;
			if (quantum >= evtCPUBurst)
			{
				if (evtCPUBurst >= currProc->getRemain())
				{
					evtCPUBurst = currProc->getRemain();
					nextEvtTime = currTime + evtCPUBurst;
					putEvent(DesLayer, currProc, nextEvtTime, 6);    // add evt 6.done
				}
				else
				{
					nextEvtTime = currTime + evtCPUBurst;
					putEvent(DesLayer, currProc, nextEvtTime, 4);    // add evt 4.running -> blocked
				}
			}
			else 
			{
				if(quantum >= currProc->getRemain())
				{
					evtCPUBurst = currProc->getRemain();
					nextEvtTime = currTime + evtCPUBurst;
					putEvent(DesLayer, currProc, nextEvtTime, 6);    // add evt 6.done
				}
				else
				{
					evtCPUBurst = quantum;
					nextEvtTime = currTime + quantum;
					putEvent(DesLayer, currProc, nextEvtTime, 5);    // add evt 5.running -> ready (quantum expired)
					currProc->setquatEndFlag(true);                  // quamtum ending, dPrio decrease by one
				}
			}

			// print ready -> running
			if (v)
			{
				cout << currTime << " " << currProc->getPID() << " " << timeInPrevState << ": " << evt->printEventType()
					<< " cb=" << currProc->getRemCPUBurst() << " rem=" << currProc->getRemain() << " prio=" << currProc->getDynamicPrio() 
					<< endl;
			}
			
			// update process control block
			currProc->setCurrState(2);					     // set current process state to runng
			currProc->setCurrStateTime(currTime);            // set current process time
			currProc->setRemCPUBurst(evtCPUBurst);           // set runng period to process
			currProc->setWaitEndTime(currTime);			     // set waiting period to process
			currProc->setRemain(evtCPUBurst);				 // remain deduct previous CPU burst;
			//DesLayer->putEvent(proc, currTime, 5);         // add evt 5.running -> ready (preemption)
			break;

		case 4: // 4. running -> blocked
			// create an event for when process becomes READY again 2.blocked -> ready
			currRunngProcess = nullptr;
			evtIOBurst = getRandom(currProc->getIOBurst());
			nextEvtTime = currTime + evtIOBurst;
			//print running -> blocked
			if (v)
			{
				cout << currTime << " " << currProc->getPID() << " " << timeInPrevState << ": " << evt->printEventType()
					<< "  ib=" << evtIOBurst << " rem=" << currProc->getRemain() << endl;
			}
			
			// record IO start time and count num of process in IO state
			if (numIOProc == 0) 
			{ 
				IOStartTime = currTime; 
			}
			numIOProc++;
			//cout << "numIOPro incr: " << numIOProc << endl;

			putEvent(DesLayer, currProc, nextEvtTime, 2);    // add evt 2.blocked -> ready
			currProc->setCurrState(3);					     // set current process state to blocked
			currProc->setCurrStateTime(currTime);            // set current process time
			currProc->setcurrIOBurst(evtIOBurst);            // set IO period to process
			//currProc->setRemCPUBurst(evtIOBurst);          // set remain CPU burst, RR: quamton, FLS: evtCPUBurst
			callScheduler = true;
			break;

		case 5: // 5. running -> ready (preemption)
			// add to runqueue (no event is generated)
			currRunngProcess = nullptr;
			
			// print running -> ready
			if (v)
			{
				cout << currTime << " " << currProc->getPID() << " " << timeInPrevState << ": " << evt->printEventType()
					<< "  cb=" << currProc->getRemCPUBurst() << " rem=" << currProc->getRemain() << " prio=" << currProc->getDynamicPrio() 
					<< endl;
			}
			
			// quantum expiration the dynamic priority decreases by one
			if (currProc->getQuatEndFlag())
			{
				currProc->decrDPrio();
				currProc->setquatEndFlag(false);
				//cout << " PID: " << currProc->getPID()  << " dPrio: " << currProc->getDynamicPrio() 
				//   << " Exp Flag: " << currProc->getExpiredFlag() << endl;
			}

			// update process control block
			currProc->setCurrState(1);					     // set current process state to ready
			currProc->setCurrStateTime(currTime);            // set current process time
			currProc->setWaitStartTime(currTime);            // set waiting start time
			
			scheduler->addProcess(currProc);                 // conditional on whether something is run
			callScheduler = true;
			break;

		case 6: // 6. End
			currRunngProcess = nullptr;
			
			// print done
			if (v)
			{
				cout << currTime << " " << currProc->getPID() << " " << timeInPrevState << ": " << evt->printEventType() << endl;
			}

			// update process control block
			currProc->setCurrState(4);						 // process 4. Done
			currProc->setCurrStateTime(currTime);            // set current process time
			currProc->setFinishTime(currTime);				 // set process finish time
			DesLayer->setFinishTime(currTime);               // set finish time of simulation layer

			callScheduler = true;
			break;
		}

		// remove current event object from Memory
		delete evt;
		evt = nullptr;
		DesLayer->rmEvent();

		if (callScheduler)
		{
			if (DesLayer->getNextEvtTime() == currTime)
			{
				continue; //process next event from Event queue
			}
			callScheduler = false; // reset global flag
			if (currRunngProcess == nullptr)
			{
				currRunngProcess = scheduler->getNextProcess();
				if (currRunngProcess == nullptr)
				{
					continue;
				}
				// create event to make this process runnable for same time.
				putEvent(DesLayer, currRunngProcess, currTime, 3);    // add evt 3. ready -> runng
				//cout << "put case 3" << endl;
 			}
		}
	}
	//cout << IOBusyTime << endl;
	DesLayer->setIOBusyTime(IOBusyTime);
	printResult(DesLayer);
}

void putEvent(DES_Layer* des, Process* proc, int time, int eventType)
{
	Event* newEvt = new Event(proc, time, eventType);
	proc->setNextTimeStamp(time);
	//cout << "put Evt: " << newEvt->printEventType() << endl;
	des->putEventDES(newEvt, time);
}

int getRandom(int burst)
{
	//cout << "getRandom: " << ofs << endl;
	int returnVal;
	if (ofs >= randVals.size())
	{
		ofs = 0;
	}
	returnVal = 1 + (randVals[ofs] % burst);
	ofs++;
	return returnVal;
}

void printResult(DES_Layer* des)
{
	int sumTurnaroundTime = 0;
	int sumCPUWaiting = 0;
	int sumCPURunng = 0;

	// pid: AT TC CB IO PRIO | FT TT IT CW
	// static list<Process*> processList;
	list<Process*>::iterator it;
	for (it = processList.begin(); it != processList.end(); ++it)
	{
		(*it)->printResult();
		sumTurnaroundTime += (*it)->getTurnaround();
		sumCPUWaiting += (*it)->getTotalCPUWait();
		sumCPURunng += (*it)->getTotalCPUTime();
	}
	// Summary Information - Finally print a summary for the simulation:
	des->printSummary(sumTurnaroundTime, sumCPUWaiting);
}

void readInFile(string fileName, DES_Layer* des)
{
	string line;
	int pid = 0;
	int totalCPUBusyTime = 0;

	inFile.open(fileName, ifstream::in);
	while (getline(inFile, line))
	{
		string res;
		int at = 0;    // arrive time
		int tc = 0;    // total cpu-time
		int cb = 0;    // CPU brust
		int ib = 0;    // IO brust
		vector<int> porcVals;
		stringstream input(line);
		while (input >> res)
		{
			//cout << res << endl;
			porcVals.push_back(stoi(res));
		}

		at = porcVals[0];    // arrive time
		tc = porcVals[1];    // total cpu-time
		cb = porcVals[2];    // CPU brust
		ib = porcVals[3];    // IO brust

		int sPrio = getRandom(maxprios);    //get stateic priority
		Process* proc = new Process(pid, at, tc, cb, ib, sPrio);
		//cout << " p create: " << pid << " sP: " << sPrio << " dp : " << proc->getDynamicPrio() << endl;
		processList.push_back(proc);
		putEvent(des, proc, at, 1);    // create event: 0.CREATED -> READY
		totalCPUBusyTime += tc;
		pid++;
	}
	des->setCPUBusyTime(totalCPUBusyTime);  // set total CPU utilization time
	des->setNumProc(processList.size());    // set num of procs
	inFile.close();
}

void readRFile(string fileName)
{
	string input;
	rFile.open(fileName, ifstream::in);
	while (getline(rFile, input))
	{
		randVals.push_back(stoi(input));
	}
	randVals.erase(randVals.begin());    //remove the first line; 40000
	rFile.close();
}

void readExeFormat(int argc, char* argv[], string& exeStr)
{
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "vets:")) != -1)
	{
		switch (c)
		{
		case 'v':
			v = true;
			break;
		case 'e':
			e = true;
			break;
		case 't':
			t = true;
			break;
		case 's':
			exeStr = string(optarg);
			//cout << " exeStr: " << exeStr << endl;
			//cout << " v: " << v << " e: " << e << " t: " << t << endl;
			break;
		default:
			abort();
		}
	}
}


Scheduler* selectScheduler(string& exeStr)
{
	/*
		// schedSpec
		�Vs[FLS   F = FCFS, L = LCFS,  S = SRTF
		   | R<num>
		   | P<num>[:<maxprio>]
		   | E<num>[:<maxprios>]]
			quantum [ numprios ]
	*/

	if (exeStr == "F")
	{
		cout << "FCFS" << endl;
		return new SchedulerFCFS();
	}
	else if (exeStr == "L") 
	{
		cout << "LCFS" << endl;
		return new SchedulerLCFS();
	}
	else if (exeStr == "S") 
	{
		cout << "SRTF" << endl;
		return new SchedulerSRTF();
	}
	else if (exeStr.at(0) == 'R')
	{
		string str = exeStr.substr(1);
		//cout << str << endl;
		quantum = stoi(str);
		//cout << " quantum: " << quantum << " maxprio: " << maxprios << endl;

		cout << "RR " << quantum << endl;
		return new SchedulerRR();
	}
	else if (exeStr.at(0) == 'P') 
	{
		int pos;
		pos = exeStr.find(":");
		if (pos < 0)
		{
			string str1 = exeStr.substr(1, pos - 1);
			quantum = stoi(str1);

			cout << "PRIO " << quantum << endl;
			return new SchedulerPRIO(maxprios);
		}
		else
		{
			string str1 = exeStr.substr(1);
			quantum = stoi(str1);
			string str2 = exeStr.substr(pos + 1);
			maxprios = stoi(str2);

			cout << "PRIO " << quantum << endl;
			return new SchedulerPRIO(maxprios);
		}
	}
	else if (exeStr.at(0) == 'E')
	{
		int pos;
		pos = exeStr.find(":");
		if (pos < 0) 
		{
			string str1 = exeStr.substr(1);
			quantum = stoi(str1);
			//cout << str1 << endl;
			cout << "PREPRIO " << quantum << endl;
			return new SchedulerPREPRIO(maxprios);
		}
		else 
		{
			string str1 = exeStr.substr(1, pos - 1);
			quantum = stoi(str1);
			string str2 = exeStr.substr(pos + 1);
			maxprios = stoi(str2);
			//cout << str1 << " " << str2 << endl;
			cout << "PREPRIO " << quantum << endl;
			return new SchedulerPREPRIO(maxprios);
		}
	}
	//return nullptr;
}

int main(int argc, char* argv[])
{
	string exeStr = "";
	// exeFormat
	// <program>[-v][-t][-e][-p][-s<schedspec>] inputfile randfile
	DES_Layer* DesLayer = new DES_Layer();
	readExeFormat(argc, argv, exeStr);
	Scheduler* scheduler = selectScheduler(exeStr);
	readRFile(string(argv[argc-1]));
	readInFile(string(argv[argc-2]), DesLayer);
	//readRFile("rfile");
	//readInFile("input6", DesLayer);
	Simulation(DesLayer, scheduler);
	return 0;
}