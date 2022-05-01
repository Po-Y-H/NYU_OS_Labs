#include "Process.h"

Process::Process(int pid) 
{
	this->pid = pid;
    for (int i = 0; i < MAX_VPAGES; i++)
    {
        pageTable[i].present = 0;
        pageTable[i].referenced = 0;
        pageTable[i].modified = 0;
        pageTable[i].write_protect = 0;
        pageTable[i].pageout = 0;
        pageTable[i].in_file = 0;
        pageTable[i].has_hole = 0;
        pageTable[i].phys_frame = i;
    }
}

// ---- setter ----

void Process::setVMA(int sv, int ev, int wp, int im)
{
    VMAs.push_back({ sv, ev, wp, im });
    /*
    struct VMA v;
    v.start_vpage = sv;
    v.end_vpage = ev;
    v.write_protected = wp;
    v.intfile_mapped = im;

    for (auto it = VMAs.begin(); it != VMAs.end(); ++it)
    {
        cout << (*it).start_vpage << endl;
    }
    */
}

void Process::updatePTE() {}

// ---- getter ----

PTE* Process::getPTE(int vpage) 
{
    PTE* p = &pageTable[vpage];
    //cout << p->phys_frame << endl;
    return p;
}

int Process::getPID() { return this->pid; }

int Process::getSizeOfVMAs() { return this->VMAs.size(); }

int Process::getVMAstartP(int VMA) { return VMAs[VMA].start_vpage; }

int Process::getVMAendP(int VMA) { return VMAs[VMA].end_vpage; }

int Process::getInfileMapped(int VMA) { return VMAs[VMA].intfile_mapped; }

int Process::getWriteProtected(int VMA) { return VMAs[VMA].write_protected; }

int Process::getReferencedBit(int VMA) { return pageTable[VMA].referenced; }


//-- increase function --

void Process::incrUNMAP() { this->unmaps++; }

void Process::incrMAP() { this->maps++; }

void Process::incrPAGEINS() { this->pageins++; }

void Process::incrPAGEOUTS() { this->pageouts++; }

void Process::incrFINS() { this->fins++; }

void Process::incrFOUTS() { this->fouts++; }

void Process::incrZEROS() { this->zeros++; }

void Process::incrSEGV() { this->segv++; }

void Process::incrSEGPROT() { this->segprot++; }

// -- printing function --

void Process::printPageTable()
{
    cout << "PT[" << this->pid << "]: ";
    for (unsigned int i = 0; i < MAX_VPAGES; i++)
    {
        if (this->pageTable[i].present == 1)
        {
            cout << i << ":";
            if (this->pageTable[i].referenced == 1) 
            {
                cout << "R";
            }
            else 
            {
                cout << "-";
            }
            if (this->pageTable[i].modified == 1)
            {
                cout << "M";
            }
            else 
            {
		 cout << "-";
            }
            if (this->pageTable[i].pageout == 1) 
            {
		if(i == 63)
		{
		    cout << "S";
		}
		else
		{
	            cout << "S ";
		}
            }
            else 
            {
               	if(i == 63)
		{
	            cout << "-";
		}
		else
		{
		    cout << "- ";
		}
            }    
        }
        else
        {
            if (this->pageTable[i].pageout == 1)
            {
		if (i == 63)
		{
		    cout << "#";
		} 
		else
		{
	            cout << "# ";
		}
            }
            else
            {
		if(i == 63)
		{
		    cout << "*";
		}
		else
		{
                    cout << "* ";
		}
            }
        }
    }
    cout << endl;
}

void Process::printVMAs() 
{
	//cout << "pid: " << pid << endl;
	for (auto it = VMAs.begin(); it != VMAs.end(); ++it)
	{
		cout << (*it).start_vpage << " " << (*it).end_vpage << " " << (*it).write_protected << " " << (*it).intfile_mapped << endl;
	}
}

void Process::printProcResult()
{
	printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
		    this->pid,
		    this->unmaps, 
            this->maps,
            this->pageins, 
            this->pageouts,
            this->fins, 
            this->fouts,
            this->zeros,
            this->segv, 
            this->segprot
          );
}
