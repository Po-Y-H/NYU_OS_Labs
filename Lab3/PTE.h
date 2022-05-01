#pragma once

typedef struct PTE
{
	// can only be total of 32-bit size and will check on this
	// page table entries (PTE) 32-bits:

	unsigned int present : 1;       // PRESENT / VALID 1 bit
	unsigned int referenced : 1;    // REFERENCED 1 bit
	unsigned int modified : 1;      // MODIFIED 1 bit
	unsigned int write_protect : 1; // WRITE_PROTECT 1 bit
	unsigned int pageout : 1;       // PAGEDOUT(SWAPPED) 1 bit
	unsigned int in_file : 1;       // EXIST IN FILE 1 bit
	unsigned int has_hole : 1;		// EXIST HOLE
	unsigned int phys_frame : 7;    // 128 physical frame 7 bits
	//unsigned int page_entry = 18; //18 bits  pagetable entry
} PTE;
