#ifndef __HARD_REALTIME_H__
#define __HARD_REALTIME_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/select.h>

#define	HR_WATCH_FOR_READ_MASK				0x01
#define	HR_WATCH_FOR_WRITE_MASK				0x02
#define	HR_WATCH_FOR_EXCEPTION_MASK			0x04
#define HR_WATCH_FOR_ALL_MASK				0x07



// Return 0 - no change in fd / timeout / error
// Return 1 - targeted change took place
bool hr_monitor_fd(int *fd, uint32_t timeout_ms, uint8_t mask);
bool hr_dynamic_time_buffer_wait(uint32_t max_time_ms);

#endif

