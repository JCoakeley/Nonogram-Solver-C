#ifndef TIMING_H
#define TIMING_H

#include <time.h>

typedef enum {
	TOTAL,
	FILEREADING,
	INIT,
	OVERLAP,
	SOLVING,
	EDGE_DEDUCTION,
	GENERATION,
	FILTERING,
	COMMON
} TimingPoint;

typedef struct {
	struct timespec fileStart;
	struct timespec fileEnd;
	struct timespec initStart;
	struct timespec initEnd;
	struct timespec overlapStart;
	struct timespec overlapEnd;
	struct timespec solvingStart;
	struct timespec solvingEnd;
	struct timespec totalStart;
	struct timespec totalEnd;
	struct timespec edgeStart;
	struct timespec edgeEnd;
	struct timespec generationStart;
	struct timespec generationEnd;
	struct timespec filteringStart;
	struct timespec filteringEnd;
	struct timespec commonStart;
	struct timespec commonEnd;
} Timings;

void timingStart (Timings *, TimingPoint);

void timingEnd (Timings *, TimingPoint);

void summationTiming (struct timespec *);

#endif
