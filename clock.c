#include <stdio.h>
#include <unistd.h>
#include <sys/times.h>
#include <stdio.h>
#include "clock.h"
#include "builtin.h"

static clock_t st_time;
static clock_t en_time;
static struct tms st_cpu;
static struct tms en_cpu;

void start_clock() {
    st_time = times(&st_cpu);
}

void end_clock(double* user_time, double* system_time)
{
    en_time = times(&en_cpu);
    long ticks_per_second = sysconf(_SC_CLK_TCK);

    *user_time = ((en_cpu.tms_cutime + en_cpu.tms_utime) - (st_cpu.tms_cutime+st_cpu.tms_stime))/ticks_per_second;
    *system_time = ((en_cpu.tms_cstime + en_cpu.tms_stime) - (st_cpu.tms_cstime + st_cpu.tms_stime)) /ticks_per_second;

}
