#define _POSIX_C_SOURCE 199309L

#include "../include/timing.h"

void timingStart (Timings * timings, TimingPoint timingPoint)
{
	switch (timingPoint)
	{
		case TOTAL:
			clock_gettime(CLOCK_MONOTONIC, &timings->totalStart);
		break;

		case FILEREADING:
			clock_gettime(CLOCK_MONOTONIC, &timings->fileStart);
		break;

		case INIT:
			summationTiming(&timings->initStart);
		break;

		case OVERLAP:
			clock_gettime(CLOCK_MONOTONIC, &timings->overlapStart);
		break;

		case SOLVING:
			clock_gettime(CLOCK_MONOTONIC, &timings->solvingStart);
		break;

		case COUNTING:
			summationTiming(&timings->countStart);
		break;

		case GENERATION:
			summationTiming(&timings->generationStart);
		break;

		case FILTERING:
			summationTiming(&timings->filteringStart);
		break;

		case COMMON:
			summationTiming(&timings->commonStart);
		break;
	}
	return;
}

void timingEnd (Timings * timings, TimingPoint timingPoint)
{
	switch (timingPoint)
	{
		case TOTAL:
			clock_gettime(CLOCK_MONOTONIC, &timings->totalEnd);
		break;

		case FILEREADING:
			clock_gettime(CLOCK_MONOTONIC, &timings->fileEnd);
		break;

		case INIT:
			summationTiming(&timings->initEnd);
		break;

		case OVERLAP:
			clock_gettime(CLOCK_MONOTONIC, &timings->overlapEnd);
		break;

		case SOLVING:
			clock_gettime(CLOCK_MONOTONIC, &timings->solvingEnd);
		break;

		case COUNTING:
			summationTiming(&timings->countEnd);
		break;

		case GENERATION:
			summationTiming(&timings->generationEnd);
		break;

		case FILTERING:
			summationTiming(&timings->filteringEnd);
		break;

		case COMMON:
			summationTiming(&timings->commonEnd);
		break;
	}
	return;
}

void summationTiming (struct timespec * target)
{
	struct timespec temp;

	clock_gettime(CLOCK_MONOTONIC, &temp);

	target->tv_sec += temp.tv_sec;
	target->tv_nsec += temp.tv_nsec;

	if (target->tv_nsec >= 1000000000L)
	{
		target->tv_sec += 1;
		target->tv_nsec -= 1000000000L;
	}

	return;
}
