#pragma once

typedef struct IOoperation
{
	int id;						// request number
	int arrive_time;
	int track_number;
	int start_time;				// -its disk service start time
	int end_time;				// - its disk service end time
} IOoperation;