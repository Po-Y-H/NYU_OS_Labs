#ifndef PROCESS_H
#define PROCESS_H
#include <iostream>
#include <vector>
#include <array>

using namespace std;

#include "PTE.h"

#define MAX_VPAGES 64             // 64 page table entries (PTE)

typedef struct VMA
{
	int start_vpage = 0;
	int end_vpage = 0;		      // (note the VMA has(end_vpage ¡V start_vpage + 1) virtual pages)
	int write_protected = 0;      // binary whether the VMA is write protected or not
	int intfile_mapped = 0;       // binary to indicate whether the VMA is mapped to a file or no

	/*
		specification for each said VMA comprised of 4 numbers:
		start_vpage: starting_virtual_page
		end_vpage: ending_virtual_page  (note the VMA has (end_vpage ¡V start_vpage + 1) virtual pages )
		write_protected: binary whether the VMA is write protected or not write_protected[0 / 1]
		file_mapped: binary to indicate whether the VMA is mapped to a file or not filemapped[0 / 1]
	
	VMA(int sv, int ev, int wp, int im) 
	{
		start_vpage = sv;
		end_vpage = ev;
		write_protected = wp;
		intfile_mapped = im;
	}
	*/
} VMA;



class Process
{
public:
	Process(int pid);

	void setVMA(int sv, int ev, int wp, int im);
	void updatePTE();	// update_pte(read / modify) bits based on operations.

	PTE* getPTE(int vpage);
	int getPID();
	int getSizeOfVMAs();

	int getVMAstartP(int VMA);
	int getVMAendP(int VMA);
	int getInfileMapped(int VMA);
	int getWriteProtected(int VMA);
	
	int getReferencedBit(int VMA);

	void incrUNMAP();
	void incrMAP();
	void incrPAGEINS();
	void incrPAGEOUTS();
	void incrFINS();
	void incrFOUTS();
	void incrZEROS();
	void incrSEGV();
	void incrSEGPROT();

	void printVMAs();
	void printPageTable();
	void printProcResult();

	int pid;
	unsigned long unmaps = 0;
	unsigned long maps = 0;
	unsigned long pageins = 0;
	unsigned long pageouts = 0;
	unsigned long fins = 0;
	unsigned long fouts = 0;
	unsigned long zeros = 0;
	unsigned long segv = 0;
	unsigned long segprot = 0;

	vector<VMA> VMAs;				// virtual memory areas
	PTE pageTable[MAX_VPAGES];		// a per process array of fixed size=64 of pte_t not pte_t pointers !
	//array<PTE, 64> pageTable;		// page table entries

};
#endif // !PROCESS_H