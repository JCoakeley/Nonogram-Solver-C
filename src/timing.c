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
			summationTiming(&timings->fileStart);
		break;

		case INIT:
			summationTiming(&timings->initStart);
		break;

		case OVERLAP:
			summationTiming(&timings->overlapStart);
		break;

		case SOLVING:
			summationTiming(&timings->solvingStart);
		break;

		case EDGE_DEDUCTION:
			summationTiming(&timings->edgeStart);
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
			summationTiming(&timings->fileEnd);
		break;

		case INIT:
			summationTiming(&timings->initEnd);
		break;

		case OVERLAP:
			summationTiming(&timings->overlapEnd);
		break;

		case SOLVING:
			summationTiming(&timings->solvingEnd);
		break;

		case EDGE_DEDUCTION:
			summationTiming(&timings->edgeEnd);
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
