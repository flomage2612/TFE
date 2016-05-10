#ifndef _PATTERN
#define _PATTERN

#include <stdlib.h>
#include <stdio.h>

typedef struct pattern
{
	int* values;
	int size;
}pattern;

int incPat(pattern* p, int alph_max);

int compLexPat(pattern* p, pattern* o);

pattern* subPattern(pattern* p, int size);

pattern** getPrefixes(pattern* p);

pattern* getSuffix(pattern* p, pattern* prefix);

void printPattern(pattern* p);

pattern* copyPattern(pattern* p);

int getMax(pattern* p);

#endif
