#pragma once

typedef struct FRAME
{
	bool victim = false;
	int frameNum = -1;			// default = -1 [0 - 127]
	int PID = -1;				// default = -1 
	int Vpage = -1;				// default = -1
	unsigned long age = 0;		//32-bit unsigned c
	unsigned long long tau = 0;

} FRAME;