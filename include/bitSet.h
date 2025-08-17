#include <stdint.h>

typedef struct BitSet {
	uint64_t * words;
	int wordCount;
	int bitCount;
} BitSet;

BitSet * newBitSet (int);

void setAllBits (BitSet *);

void clearBit (BitSet *, int);

int nextSetBit (BitSet *, int);
