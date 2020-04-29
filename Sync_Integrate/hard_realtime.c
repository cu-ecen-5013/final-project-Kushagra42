
#include "hard_realtime.h"

bool hr_monitor_fd(int *file, uint32_t timeout_ms, uint8_t mask)
{
	fd_set fds;
	struct timeval tout;
	int resp;
	
	FD_ZERO(&fds);
	FD_SET(*file, &fds);
	
	tout.tv_sec = (uint32_t)(timeout_ms / 1000);
	tout.tv_usec = (timeout_ms - (tout.tv_sec * 1000)) * 1000;

	switch(mask & HR_WATCH_FOR_ALL_MASK)
	{
		case 0:
			resp = -1;
			break;
		case HR_WATCH_FOR_READ_MASK:
			resp = select(*file + 1, &fds, NULL, NULL, &tout);
			break;
		case HR_WATCH_FOR_WRITE_MASK:
			resp = select(*file + 1, NULL, &fds, NULL, &tout);
			break;
		case HR_WATCH_FOR_EXCEPTION_MASK:
			resp = select(*file + 1, NULL, NULL, &fds, &tout);
			break;
		case (HR_WATCH_FOR_READ_MASK | HR_WATCH_FOR_WRITE_MASK):
			resp = select(*file + 1, &fds, &fds, NULL, &tout);
			break;
		case (HR_WATCH_FOR_WRITE_MASK | HR_WATCH_FOR_EXCEPTION_MASK):
			resp = select(*file + 1, NULL, &fds, &fds, &tout);
			break;
		case (HR_WATCH_FOR_READ_MASK | HR_WATCH_FOR_EXCEPTION_MASK):
			resp = select(*file + 1, &fds, NULL, &fds, &tout);
			break;
		case HR_WATCH_FOR_ALL_MASK:
			resp = select(*file + 1, &fds, &fds, &fds, &tout);
			break;
		default:
			resp = -1;
			break;
	}

	if(resp <= 0)	return false;
	
	return true;
}


bool hr_dynamic_time_buffer_wait(uint32_t max_time_ms)
{
	static bool first_call = 1; 
	static struct timespec prev_t;
	int resp;
	uint32_t sec = max_time_ms / 1000; 
	
	if(first_call == 1)
	{
		first_call = 0;
		if(clock_gettime(CLOCK_REALTIME, &prev_t) != 0)		return false;
	}
	
	prev_t.tv_sec += sec;
	max_time_ms -= sec * 1000;
	
	if(prev_t.tv_nsec < (1000000000 - (max_time_ms * 1000000)))		prev_t.tv_nsec += max_time_ms * 1000000;

	else
	{
		prev_t.tv_sec += 1;
		prev_t.tv_nsec = (max_time_ms * 1000000) - (1000000000 - prev_t.tv_nsec);
	}
	
	do
	{
		resp = clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &prev_t, NULL);
	}while(resp != 0);
return true;
}


