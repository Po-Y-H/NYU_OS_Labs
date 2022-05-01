#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Scheduler.h"
#include "IOoperation.h"

using namespace std;

// member function

void simulation(char& alg);
void readExeFormat(int argc, char* argv[], char& alg);
void readFile(string fileName);

Scheduler* selectScheduler(char& alg);

void swapQueues();
void settleIOrequest(IOoperation* req);
void printIOrequest();
void printSummary();

// global variable

vector<IOoperation> IOoptVec;	// most 10000 IO operation
vector<IOoperation> IOqueue;	// request IO-queue
vector<IOoperation> IOqueue2;	// request IO-queue
vector<IOoperation> IOfinVec;	// IO operation finished

vector<IOoperation>* activeQueue = &IOqueue;	// active IO-queue
vector<IOoperation>* addQueue = &IOqueue2;		// add IO-queue

Scheduler* sched = nullptr;

static int numIOopt = 0;
static int timeCounter = 1;			// system time, start from 1
static int trackPos = 0;			// position of track head
static bool trackDir = true;		// track direction; (default) low to high = true, high to low = false.
static bool flook = false;			// mode FLOOK default = false
static bool queueSwapped = false;   // false ( add = IOQueue2 ) => true ( add = IOQueue )

static int total_time = 0;			// total simulated time, i.e.until the last I / O request has completed.
static int tot_movement = 0;		// total number of tracks the head had to be moved
static int tot_turnaround = 0;		// total turnaround time from time of submission to time of completion
static int tot_waittime = 0;		// total time from submission to issue of IO request to start disk operation
static double avg_turnaround = 0;	// average turnaround time per operation from time of submission to time of completion
static double avg_waittime = 0;		// average wait time per operation(time from submission to issue of IO request to start disk operation)
static int max_waittime = 0;		// maximum wait time for any IO operation.

// function body

int main(int argc, char** argv)
{
	char alg;
	readExeFormat(argc, argv, alg);
	readFile(string(argv[argc - 1]));
	simulation(alg);
	return 0;
}

void simulation(char& alg)
{
	// select scheduler
	sched = selectScheduler(alg);

	bool IOisRunng = false;
	IOoperation activeReq;

	/* while (true) */
	while (true)
	{
		//	if a new I / O arrived to the system at this current time
		//		�� add request to IO  queue
		if (IOoptVec.size() != 0 && timeCounter == IOoptVec.front().arrive_time)
		{
			if (flook)
			{
				addQueue->push_back(IOoptVec.front());
				IOoptVec.erase(IOoptVec.begin());
			}
			else 
			{
				IOqueue.push_back(IOoptVec.front());
				IOoptVec.erase(IOoptVec.begin());
			}
		}

		//	if an IO is activeand completed at this time
		//		�� Compute relevant infoand store in IO request for final summary
		if (IOisRunng == true && trackPos == activeReq.track_number)
		{
			activeReq.end_time = timeCounter;
			IOoperation* req = &activeReq;
			settleIOrequest(req);
			IOisRunng = false;
		}

		//if no IO request active now
		if (!IOisRunng)
		{
			if (flook) 
			{
				if (!activeQueue->empty()) 
				{
					activeReq = sched->strategy();
					activeReq.start_time = timeCounter;
					IOisRunng = true;

					// check head direction
					if (activeReq.track_number < trackPos && trackDir == true)
					{
						trackDir = false;  // change direction to decrease head position
					}
					else if (activeReq.track_number > trackPos && trackDir == false)
					{
						trackDir = true;	// change direction to increase head position
					}
				}
				else if (IOoptVec.empty() && activeQueue->empty() && addQueue->empty())
				{
					break;
				}
				else if (activeQueue->empty())
				{
					if (addQueue->empty())
					{
						timeCounter++;
						continue;
					}
					else
					{
						swapQueues();
						continue;
					}
				}
			}
			else 
			{
				//	if requests are pending
				// 		�� Fetch the next request from IO - queue and start the new IO.
				if (!IOqueue.empty())
				{
					activeReq = sched->strategy();
					activeReq.start_time = timeCounter;
					IOisRunng = true;

					// check head direction
					if (activeReq.track_number < trackPos && trackDir == true)
					{
						trackDir = false;  // change direction to decrease head position
					}
					else if (activeReq.track_number > trackPos && trackDir == false)
					{
						trackDir = true;	// change direction to increase head position
					}
				}
				//	else if all IO from input file processed
				//		�� exit simulation
				else if (IOoptVec.empty())
				{
					break;
				}
			}
		}

		//	if an IO is active
		//		�� Move the head by one unit in the direction its going(to simulate seek)
		if (IOisRunng)
		{
			if (activeReq.track_number == trackPos) // next req on current pos
			{
				continue;
			}
			else if (trackDir == true)				// trackDir = true; low to high
			{
				trackPos++;
				tot_movement++;
			}
			else									// trackDir = false; high to low
			{
				trackPos--;
				tot_movement++;
			}
		}
		// Increment time by 1
		timeCounter++;
	}
	printIOrequest();
	printSummary();
}

void swapQueues() 
{
	if (queueSwapped == false)   // false ( add = IOQueue2 ) => true ( add = IOQueue )
	{
		activeQueue = &IOqueue2;	// active IO-queue
		addQueue = &IOqueue;		// add IO-queue
		queueSwapped = true;
	}
	else						// true ( add = IOQueue ) => false ( add = IOQueue2 )
	{
		activeQueue = &IOqueue;		// active IO-queue
		addQueue = &IOqueue2;		// add IO-queue
		queueSwapped = false;
	}
}

void readFile(string fileName) 
{
	string line;
	string res;
	ifstream infile;
	int id = 0;
	int arrive_time = 0;
	int track_number = 0;

	infile.open(fileName, ifstream::in);
	while (getline(infile, line))
	{
		if (line[0] == '#')
		{
			continue;
		}
		else 
		{
			vector<int> porcVMAs;
			stringstream input(line);
			while (input >> res)
			{
				porcVMAs.push_back(stoi(res));
			}
			arrive_time = porcVMAs[0];
			track_number = porcVMAs[1];

			//store
			IOoptVec.push_back({ id, arrive_time ,track_number, 0, 0 });
			IOfinVec.push_back({ id, arrive_time ,track_number, 0, 0 });
		}
		id++;
	}
	numIOopt = IOoptVec.size();
	infile.close();
}

Scheduler* selectScheduler(char &alg)
{
	Scheduler* s = nullptr;
	switch (alg)
	{
	case 'i':
		s = new FIFOScheduler(IOqueue);													// (i)
		break;
	case 'j':
		s = new SSTFScheduler(IOqueue, trackPos);										// (j)
		break;
	case 's':
		s = new LOOKScheduler(IOqueue, trackPos, trackDir);								// (s)
		break;
	case 'c':
		s = new CLOOKScheduler(IOqueue, trackPos, trackDir);							// (c)
		break;
	case 'f':
		s = new FLOOKScheduler(IOqueue, IOqueue2, trackPos, trackDir, queueSwapped);	// (f)
		flook = true;
		break;
	}
	return s;
}

void readExeFormat(int argc, char* argv[], char& alg)
{
	// ./ iosched[�Vs<schedalgo> | -v | -q | -f] <inputfile>
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "s:")) != -1)
	{
		switch (c)
		{
		case 's':
			alg = (*optarg);
			break;
		default:
			abort();
		}
	}
}

void settleIOrequest(IOoperation* req)
{
	int waitTime = 0;
	tot_turnaround += (req->end_time - req->arrive_time);
	waitTime = (req->start_time - req->arrive_time);
	tot_waittime += waitTime;
	if (max_waittime < waitTime) { max_waittime = waitTime; }

	for (auto it = IOfinVec.begin(); it != IOfinVec.end(); it++)
	{
		if ((*req).id == (*it).id)
		{
			(*it).start_time = (*req).start_time;
			(*it).end_time = (*req).end_time;
			break;
		}
	}
}

void printIOrequest()
{
	for (auto req = IOfinVec.begin(); req != IOfinVec.end(); req++)
	{
		printf("%5d: %5d %5d %5d\n", req->id, req->arrive_time, req->start_time, req->end_time);
	}
	/*
	- IO-op#, 
	- its arrival to the system (same as inputfile)
	- its disk service start time
	- its disk service end time 
	*/
}

void printSummary()
{
	total_time = timeCounter;
	avg_turnaround = tot_turnaround / double(numIOopt);
	avg_waittime = tot_waittime / double(numIOopt);

	printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);
	/*
	- total_time: total simulated time, i.e. until the last I/O request has completed.
	- tot_movement: total number of tracks the head had to be moved
	- avg_turnaround: average turnaround time per operation from time of submission to time of completion
	- avg_waittime: average wait time per operation (time from submission to issue of IO request to start disk operation)
	- max_waittime: maximum wait time for any IO operation.
	*/
}