#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <array>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Process.h"
#include "Pager.h"
#include "Frame.h"
#include "PTE.h"

using namespace std;

#define MAX_FRAMES 128

// Member Function

void simulation(int &nf, char &alg, string &opts);
void readExeFormat(int argc, char* argv[], int& nf, char& alg, string& opts);
void readInFile(string fileName);
void readRFile(string fileName);
void selectOptions(string &opts);
Pager* selectPager(char &alg);

FRAME* getFrame();
FRAME* frameFromFreeList();
void getNextInstruction(string &operation, int &vpage);
void addCost(int cost);

void printProcResult(int PID);
void printPageTable(int PID);
void printFrameTable();
void printSummary();

// statoc variables & struct

typedef struct INSTRUCTION
{
	string operation;
	int vpage;

	/* instrctions:
	��c <procid>��: specifies that a context switch to process #<procid> is to be performed. It is guaranteed that the first instruction will always		            be a context switch instruction, since you must have an active pagetable in the MMU (in real systems).
	��r <vpage>��:  implies that a load/read operation is performed on virtual page <vpage> of the currently running process.
	��w <vpage>��:  implies that a store/write operation is performed on virtual page <vpage> of the currently running process.
	��e <procid>��: current process exits, we guarantee that <procid> is the current running proc, so you can ignore it.
	*/
} INSTRUCTION;

bool optO = false;   // default: false
bool optP = false;
bool optF = false;
bool optS = false;
bool optX = false;
bool optY = false;
bool optf = false;
bool opta = false;

static int SnumOfProc = 0;
static int maxFrames = MAX_FRAMES;
static ifstream inFile;					// input file
static ifstream rFile;					// randon number file

static vector<int> randVals;		    // random values from rfile
static vector<Process*> procsVec;       // vector store processes
static vector<INSTRUCTION> instrcVec;   // vector store instrcution sinulations

long long countUNMAP = 0;
long long countMAP = 0;
long long countIN = 0;
long long countOUT = 0;
long long countZERO = 0;

unsigned long ctxSwitches = 0;
unsigned long processExits = 0;
unsigned long long cost = 0;
unsigned long long instrcCounter = 0;	// counter of instrction execution
unsigned long instrcSize = 0;			// number of instruction

Pager* currentPager = nullptr;
FRAME frameTable[MAX_FRAMES];			// global frame_table: physical frame,  MAX_FRAMES by input order
list<FRAME*> freeFrames;				// free frame list

// ---- simulation function body ----

void simulation(int& nf, char& alg, string& opts)
{
	string ops = "";    // instruction operation
	int vpage = 0;		// instruction page, current vpage
	
	// select Options
	selectOptions(opts);

	// assign frame number
	maxFrames = nf;

	// select Pager
	currentPager = selectPager(alg);

	//currentPager = new PagerWorkingSet(maxFrames, procsVec, instrcCounter);
	//currentPager = new PagerAging(maxFrames, procsVec);
	//currentPager = new PagerNRU(maxFrames, procsVec, instrcCounter);
	//currentPager = new PagerClock(maxFrames, procsVec);
	//currentPager = new PagerRandom(maxFrames, randVals);
	//selectPager = new PagerFIFO(maxFrames);

	//cout << nf << " " << alg << " " << opts << endl;

	// initialize frame table & free list
	for (unsigned int i = 0; i < maxFrames; i++)
	{
		FRAME* it = &frameTable[i];
		(*it).frameNum = i;
		freeFrames.push_back(it);
	}

	/* check free list
	list<FRAME*>::iterator it;
	for (it = freeFrames.begin(); it != freeFrames.end(); ++it)
	{
		cout << " free list frame: " << (*it)->frameNum << endl;
	}
	*/

	// current process
	Process* currentProcess = nullptr;
	Process* reverseProcess = nullptr;
	currentProcess = procsVec[0];		// select first process
	instrcSize = instrcVec.size();		// set instruction size
	
	// Hardware behavior: #
	while (instrcCounter < instrcSize)
	{
		FRAME* newFrame = nullptr;

		// get next instruction
		getNextInstruction(ops, vpage);
		
		// get current process PTE
		PTE* pte = currentProcess->getPTE(vpage);

		if (optO)  // print instruction
		{
			cout << instrcCounter << ": ==> " << ops << " " << vpage << endl;
			//cout << "instrcCounter : " << instrcCounter << "instrcSize : " << instrcSize << endl;
		}

		// handle special case of ��c�� and ��e�� instruction
		if (ops == "e")    //Proc Exit
		{
			cout << "EXIT current process " << vpage << endl;
			for (int i = 0; i < 64; i++)
			{
				PTE* p = currentProcess->getPTE(i);
				if (p->present)
				{
					// reset frame info
					//cout << p->phys_frame << endl;
					FRAME* unmapframe = &frameTable[p->phys_frame];

					// UNMAP
					cout << " UNMAP " << unmapframe->PID << ":" << unmapframe->Vpage << endl;
					currentProcess->incrUNMAP();				// increase UNMAP
					addCost(400);								// add COST unmaps = 400
					unmapframe->victim = false;
					unmapframe->PID = -1;						// default = -1 
					unmapframe->Vpage = -1;						// default = -1
					unmapframe->age = 0;						//32-bit unsigned c
					unmapframe->tau = 0;
					freeFrames.push_back(unmapframe);

					// FOUT
					if (p->modified && p->in_file)
					{
						cout << " FOUT" << endl;				// cout << FOUT
						currentProcess->incrFOUTS();			// increase FOUT
						addCost(2400);							// add COST fouts = 2400
					}
				}
				p->present = 0;
				p->referenced = 0;
				p->pageout = 0;
			}
			processExits++;										// count process exit
			addCost(1250);										// add COST process exits instructions = 1250.
			instrcCounter++;									// move on to next instruction
			continue;
		} 
		else if (ops == "c")    // Context Switch
		{
			currentProcess = procsVec[vpage];
			ctxSwitches++;										// count context switch
			addCost(130);										// add COST context_switches instructions = 130,
			instrcCounter++;									// move on to next instruction
			//cout << currentProcess->getPID();
			continue;
		}

		
		// now the real instructions for read and write
		addCost(1);												// add COST read/write (load/store) instructions count = 1,
		// this in reality generates the page fault exception and now you execute
		// verify this is actually a valid page in a vma if not raise error and next inst
		if (!pte->present)
		{
			// rewrite this part
			
			// determine that the vpage can be accessed in VMAs
			bool segFault = false;
			
			if (pte->has_hole)		//reduce comp cost
			{
				segFault = true;
				//cout << "has hole" << endl;
			}
			else  // if noSEGV output line must be created and move on to the next instruction.
			{
				//cout << "size of VMAs : "<< currentProcess->getSizeOfVMAs() << endl;
				for (int i = 0; i < currentProcess->getSizeOfVMAs(); i++)
				{
					if (vpage >= currentProcess->getVMAstartP(i) && vpage <= currentProcess->getVMAendP(i))
					{
						pte->in_file = currentProcess->getInfileMapped(i);
						pte->write_protect = currentProcess->getWriteProtected(i);
						pte->has_hole = 1;
						segFault = true;
						break;
					}
				}
			}
			
			if (segFault == false)    // segmentation fault
			{
				if (optO) { cout << " SEGV" << endl; }			// cout << SEGV
				currentProcess->incrSEGV();						// increase SEGV
				addCost(340);									// add COST segv = 340
				instrcCounter++;								// move on to next instruction
				continue;
			}
			
			if (!freeFrames.empty())    // use free frame
			{
				newFrame = getFrame();
			}
			else    // UNMAP
			{
				newFrame = getFrame();
				
				// reverse mapping
				int revMapProcPID = newFrame->PID;
				int revMapVpage = newFrame->Vpage;

				reverseProcess = procsVec[revMapProcPID];
				PTE* revPTE = reverseProcess->getPTE(revMapVpage);
				//cout << "rev present bit:  " << revPTE->present << endl;
				revPTE->present = 0;

				// UNMAP										// cout << UNMAP
				if (optO) { cout << " UNMAP " << reverseProcess->getPID() << ":" << revMapVpage << endl; }
				reverseProcess->incrUNMAP();					// increase UNMAP
				addCost(400);									// add COST unmaps = 400

				//cout << " rev vpage: " << revMapVpage << " rev modi: " << revPTE->modified << endl;
				if (revPTE->modified)  // FOUT || OUT (Modified)
				{
					if (revPTE->in_file)
					{
						revPTE->modified = 0;
						if (optO) { cout << " FOUT" << endl; }	// cout << FOUT
						reverseProcess->incrFOUTS();			// increase FOUT
						addCost(2400);							// add COST fouts = 2400
					}
					else 
					{
						revPTE->modified = 0;
						revPTE->pageout = 1;
						if (optO) { cout << " OUT" << endl; }	// cout << OUT
						reverseProcess->incrPAGEOUTS();			// increase OUT
						addCost(2700);							// add COST outs = 2700
					}

				}
				revPTE->phys_frame = 0;					// reset phy frame number
			}
			//currentProcess->printProcResult();
			if (pte->in_file)    // FIN
			{
				if (optO) { cout << " FIN" << endl; }			// cout << FIN
				currentProcess->incrFINS();						// increase FINS
				addCost(2800);									// add COST fins = 2800
			}
			else 
			{
				if (pte->pageout)    // IN
				{
					if (optO) { cout << " IN" << endl; }		// cout << IN
					currentProcess->incrPAGEINS();				// increase PAGEINS
					addCost(3100);								// add COST ins = 3100
				}
				else    // ZERO 
				{
					if (optO) { cout << " ZERO" << endl; }		// cout << ZERO
					currentProcess->incrZEROS();				// increase ZEROS
					addCost(140);								// add COST zeros = 140
				}
			}
			
			// update New Frame information
			int newPID = currentProcess->getPID();
			newFrame->PID = newPID;
			newFrame->Vpage = vpage;
			newFrame->tau = instrcCounter;
			newFrame->age = 0;

			if (optO) { cout << " MAP " << newFrame->frameNum << endl; }
			currentProcess->incrMAP();							// increase MAP
			addCost(300);										// add COST maps = 300

			// update PTE information
			pte->present = 1;
			pte->phys_frame = newFrame->frameNum;				
			//-> figure out if/what to do with old frame if it was mapped
			// see general outline in MM-slides under Lab3 header and writeup below
			// see whether and how to bring in the content of the access page.

		}
	
		// simulate instruction execution by hardware by updating the R/M PTE bits 
		if (ops == "r" || ops == "w")
		{
			pte->referenced = 1;
		}
		
		// check write protection
		// update_pte(read / modify); bits based on operations.
		if (ops == "w")
		{
			if (pte->write_protect)  // check write protection
			{
				if (optO) { cout << " SEGPROT" << endl; }		// cout << SEGPROT
				currentProcess->incrSEGPROT();					// increase SEGPROT
				addCost(420);									// add COST segprot = 420
			}
			else
			{
				pte->modified = 1;
				//cout << pte->modified << " modifief " << endl;
			}
		}
		//printPageTable(0);
		//printPageTable(1);
		instrcCounter++;
	}
	// print result
	printSummary();
}

FRAME* getFrame()
{
	FRAME* f = frameFromFreeList();
	if (f == nullptr)
	{
		f = currentPager->selectVictimFrame(frameTable);
		//printFrameTable();
	}
	return f;
}

FRAME* frameFromFreeList()
{
	if (freeFrames.empty())
	{
		return nullptr;
	}
	else 
	{
		FRAME* frame = freeFrames.front();
		freeFrames.pop_front();
		return frame;
	}
}

void getNextInstruction(string &ops, int &vpage)
{
	ops = instrcVec[instrcCounter].operation;	 // war: possible lose of data
	vpage = instrcVec[instrcCounter].vpage;		 // war: possible lose of data
	//cout << "ops: " << ops << " vpage: " << vpage << endl;
}

void addCost(int addCost) 
{
	/*  cost:
		0. read/write (load/store) instructions count = 1,
		1. context_switches instructions = 130,
		2. process exits instructions = 1250.
		3. maps = 300,
		4. unmaps = 400,
		5. ins = 3100, PAGEINS
		6. outs = 2700 PAGEOUTS
		7. fins = 2800,
		8. fouts = 2400
		9. zeros = 140,
		10.segv = 340,
		11.segprot = 420
	*/
	//cout << " cost add: " << addCost << endl;
	cost += addCost;
	//cout << " curr cost : " << cost << endl;
}

// ---- main function body ----

void readInFile(string fileName)
{
	string line;
	string res;
	int numOfProc = 0;  // number of process
	int numOfVMA = 0;   // number of VMA in current process
	int currPID = 0;    // current PID

	Process* currentProc;
	bool flag_numOfProc = false;
	bool flag_numOfVMA = false;

	inFile.open(fileName, ifstream::in);

	while (getline(inFile, line))
	{
		if (line[0] == '#') 
		{ 
			continue; 
		}
		else if (flag_numOfProc == false)    // read number of process
		{
			numOfProc = stoi(line);    // pid : [0 ~ (numOfProc - 1)]
			SnumOfProc = numOfProc;
			flag_numOfProc = true;
			continue;
		}
		else if (flag_numOfVMA == false && currPID < numOfProc)    // read number of VMA
		{
			numOfVMA = stoi(line);
			flag_numOfVMA = true;

			// create and store new process
			Process* proc = new Process(currPID);
			currentProc = proc;
			procsVec.push_back(proc);
			continue;
		}
		else if (flag_numOfVMA == true && numOfVMA != 0)    // read VMA info
		{
			currentProc = procsVec[currPID];

			// get and store VMA to proc
			vector<int> porcVMAs;
			stringstream input(line);
			while (input >> res)
			{
				
				porcVMAs.push_back(stoi(res));
			}
			int sv = porcVMAs[0];		// start_vpage
			int ev = porcVMAs[1];		// end_vpage
			int wp = porcVMAs[2];		// write_protected
			int im = porcVMAs[3];		// intfile_mapped
			
			currentProc->setVMA(sv, ev, wp, im);

			numOfVMA--;
			if (numOfVMA == 0)  // last VMA, sitch to next process
			{ 
				flag_numOfVMA = false;
				currentProc = nullptr;
				currPID++;
			}
			continue;
		}
		else    // read instructions
		{
			vector<string> porcInstrcs;
			stringstream input(line);
			while (input >> res)
			{
				porcInstrcs.push_back(res);
			}
			
			string operation = porcInstrcs[0];
			int vpage = stoi(porcInstrcs[1]);
			instrcVec.push_back({operation, vpage});
		}
	}
	inFile.close();
	/*
	cout << " process vector " << endl;
	cout << " process vector size: " << procsVec.size() << endl;
	vector<Process*>::iterator it;
	for (it = procsVec.begin(); it != procsVec.end(); ++it)
	{
		cout << (*it)->getPID();
	}
	for (auto it = procsVec.begin(); it != procsVec.end(); ++it)
	{
		cout << " pid: " << (*it)->getPID() << endl;
		//(*it)->printVMAs();
	}
	cout << " instrc vector" << endl;
	for (auto it = instrcVec.begin(); it != instrcVec.end(); ++it)
	{
		cout << " type :" << (*it).operation << " page :" << (*it).vpage << endl;
	}
	*/
}

void readRFile(string fileName)
{
	string input;
	rFile.open(fileName, ifstream::in);
	while (getline(rFile, input))
	{
		randVals.push_back(stoi(input));
	}
	randVals.erase(randVals.begin());    //remove the first line; 10000
	rFile.close();

}

void readExeFormat(int argc, char* argv[], int& nf, char& alg, string& opts)
{
	// ./mmu �Vf<num_frames> -a<algo> [-o<options>] inputfile randomfile
	int c;
	opterr = 0;
	while ((c = getopt(argc, argv, "f:a:o:")) != -1)
	{
		switch (c)
		{
		case 'f':
			nf = atoi(optarg);
			//cout << " maxFrames: " << maxFrames << endl;
			break;
		case 'a':
			alg = (*optarg);
			//cout << " alg: " << alg << endl;
			break;
		case 'o':
			opts = string(optarg);
			//cout << " exeStr: " << exeStr << endl;
			break;
		default:
			abort();
		}
	}

}

Pager* selectPager(char &alg) 
{
	/*
		FIFO: F
		Random : R
		Clock : C
		Enhanced Second Chance / NRU : E
		Aging : A
		Working Set : W
	*/
	//cout << "select P: " << alg << endl;

	Pager* P = nullptr;
	switch (alg)
	{
	case 'f':
		P = new PagerFIFO(maxFrames);
		//cout << " pager f! " << endl;
		break;
	case 'r':
		P = new PagerRandom(maxFrames, randVals);
		break;
	case 'c':
		P = new PagerClock(maxFrames, procsVec);
		break;
	case 'e':
		P = new PagerNRU(maxFrames, procsVec, instrcCounter);
		break;
	case 'a':
		P = new PagerAging(maxFrames, procsVec);
		break;
	case 'w':
		P = new PagerWorkingSet(maxFrames, procsVec, instrcCounter);
		break;
	}
	return P;
}

void selectOptions(string &opts) 
{
	
	if (opts.find("O") != -1)
	{
		optO = true;
		//cout << " opt O " << endl;
	}
	if (opts.find("P") != -1)
	{
		optP = true;
	}
	if (opts.find("F") != -1)
	{
		optF = true;
	}
	if (opts.find("S") != -1)
	{
		optS = true;
	}
	/*
	if opts.find("x")
	{
		bool optX = false;
	}
	if opts.find("y")
	{
		bool optY = false;
	}
	if opts.find("f")
	{
		bool optF = false;
	}
	if opts.find("a")
	{
		bool optA = false;
	}
	*/
}

void printProcResult(int PID)
{
	procsVec[PID]->printProcResult();
}

void printPageTable(int PID) 
{
	procsVec[PID]->printPageTable();
}

void printFrameTable()
{
	cout << "FT: ";
	for (unsigned int i = 0; i < maxFrames; i++)
	{
		if(i == (maxFrames - 1))
		{
			if (frameTable[i].PID != -1)
			{	
				cout << frameTable[i].PID << ":" << frameTable[i].Vpage;
			}	
			else
			{		
				cout << "*";
			}
		}
		else
		{
			if (frameTable[i].PID != -1)
			{
				cout << frameTable[i].PID << ":" << frameTable[i].Vpage << " ";
            }
            else
            {
            	cout << "* ";
            }
		}	
	}
	cout << endl;
}

void printSummary() 
{
	if (optP)  // pagetable option 
	{
		for (unsigned int i = 0; i < SnumOfProc; i++)
		{
			printPageTable(i);
		}
	}
	if (optF)  // frame table option
	{
		printFrameTable();
	}
	if (optS)  // process statistics
	{
		//cout << " process vector: " << SnumOfProc << endl;
		for (int i = 0; i < SnumOfProc; i++)
		{
			printProcResult(i);
		}
		printf("TOTALCOST %lu %lu %lu %llu %lu\n", instrcSize, ctxSwitches, processExits, cost, sizeof(PTE));
	}
}

int main(int argc, char* argv[])
{
	int nf = 0;
	char alg;
	string opts = "";
	readExeFormat(argc, argv, nf, alg, opts);  //argc, argv
	readRFile(string(argv[argc - 1]));
	readInFile(string(argv[argc - 2]));
	//readRFile("rfile");
	//readInFile("in11");
	simulation(nf, alg, opts);

	return 0;
}
