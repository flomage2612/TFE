#include "pattern.h"


int incPat(pattern* p, int alph_max)
{
		for(int i = p->size-1; i >= 0; i--)
		{
			if(p->values[i] < alph_max)
			{
				p->values[i]++;
				if(i < p->size-1)
				{
					for(int j = i+1; j < p->size; j++)
						p->values[j] = 0;
				}
				return 0;
			}
		}
		return 1;
}

int compLexPat(pattern* p, pattern* o)
{
	int m = o->size;
	if(p->size < o->size)
		m = p->size;
	for(int i = 0; i < m; i++) 
	{
		if(p->values[i] > o->values[i])
			return -1;
		else if(p->values[i] < o->values[i])
			return 1;
			
	}
	if(o->size == p->size)
		return 0;
	else return (p->size < o->size)? 1:-1;
}

pattern* subPattern(pattern* p, int size)
{
	if(size >= p-> size)
		return p;
	pattern* res = malloc(sizeof(pattern));
	res->size = size;
	res->values = malloc(size*sizeof(int));
	for(int i = 0; i < size; i++)
	{
		res->values[i] = p->values[i]; 
	}
	return res;
}

pattern** getPrefixes(pattern* p)
{
	pattern** result = malloc(p->size*sizeof(pattern*));
	if(result == NULL)
		return NULL;
	int i = 0; 
	while(i < p->size - 1) 
	{
		result[i] = malloc(sizeof(pattern));
		if(result[i] == NULL)
			return NULL;
			
		result[i]->size = i+1;
		result[i]->values = malloc((i+1)*sizeof(int));
		int j = 0;
		for(;j <= i; j++)
		{
			result[i]->values[j] = p->values[j];
		}
		i++;
	}	
	return result;

}

pattern* getSuffix(pattern* p, pattern* prefix)
{
	int j = prefix->size;
	pattern* result = malloc(sizeof(pattern));
	result->values = malloc((p->size - prefix->size)*sizeof(int));
	result->size = p->size - prefix->size;
	int i = 0;
	while(j < p->size)
	{
		result->values[i] = p->values[j];
		j++;
		i++;
	}
	return result;
}

pattern* copyPattern(pattern* p)
{
	pattern* copy = malloc(sizeof(pattern));
	copy->size = p->size;
	copy->values = malloc(copy->size*(sizeof(int)));
	for(int i = 0; i < copy->size; i++)
	{
		copy->values[i] = p->values[i];
	}
	return copy;
}

int getMax(pattern* p)
{
	int max = 0;
	for(int i = 0; i < p->size; i++ )
	{
		if(p->values[i] > max)
			max = p->values[i];
	}
	return max;
}

void printPattern(pattern* p)
{
	int i = 0; 
	printf("Pattern : ");
	for(;i < p->size; i++)
	{
		printf("%i",p->values[i]);
	}
	printf("\n");
}