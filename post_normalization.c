#include <stdlib.h>
#include <stdio.h>

#include "pattern.h"
#include "rule.h"
#include "hashtable_r.h"



void postNormalize(pattern* p, int order, int* value, int alph_max);
void genRules(int alphMax, int order, int* values, hash_table* t, hash_table* states );
void expand(pattern* p, int order, int* values, int alph_max);
void outputState(pattern* output, int order, int* value, int max_alph, hash_table* rules);
void applyRules(hash_table* rules, pattern* p, int order);

// input format :  m [ 1 1 1 ] [ 4 2 1 ]"
int main(int argc, char *argv[])
{


	if(argc < 3)
		return 1;
	
	int order = atoi(argv[1]);
	int coefficients[order];
	int values[order];
	int i = 2,j = 0, k = 0;

	for( ; i < argc && *argv[i] != '['; i++);

	j = i+1;
	while(*argv[j]  != ']')
	{			 
		coefficients[k] = atoi(argv[j]);
		j++;
		k++;				
	}

	i = j;
	k = 0;
	for( ; i < argc && *argv[i] != '['; i++);

	j = i+1;
	while(*argv[j]  != ']')
	{	 
		values[k] = atoi(argv[j]);
		j++;
		k++;				
	}
	hash_table* rules = createhash_table(order*10);
	hash_table* states = createhash_table(order*10);
	genRules(1, order, values, rules, states);
	//printTablePattern(states);
	//printTableRules(rules);
/*	
	d->size = 4;
	int tab[4] = {0,3,2,2};
	d->values = &tab;
	if(getRule(rules, d, hashKey(rules->capacity, d)) == NULL)
		printf("y'a pass");

	applyRules(rules, d, order);
	printPattern(d);
	*/
	
	for(int i = 0; i < states->capacity; i++)
	{
		if(states->tab[i] == NULL)
			continue;
		pattern* p;
		hash_entry* e = states->tab[i];
		while(e->next != NULL)
		{
			p = ((pattern*) e->payload);
			pattern* o = copyPattern(p);
			printPattern(o);
			outputState(o, order, values, 1, rules);
			printPattern(o);
			printf(" \n");
			e = e->next;
		}
		printf(" \n");
	}
	
	

}

void genRules(int alphMax, int order, int* values, hash_table* t, hash_table* states )
{
	
	int p = alphMax*2 - 2;
	for(int h = 0; h <= p; h++)
	{
		pattern i;
		i.size = order-1;
		i.values = malloc(i.size*sizeof(int));
		for(int j = 0; j < i.size; j++) 
		{
			i.values[j] = 0; 
		}
		
		do
		{
			for(int l = 1; l <= p+2; l++)
			{
				pattern* left;
				left = malloc(sizeof(pattern));
				left->size = order+1;
				left->values = malloc((order+1)*sizeof(int));
				
				pattern* right;
				right = malloc(sizeof(pattern));
				right->size = order+1;
				right->values = malloc((order+1)*sizeof(int));
				
				left->values[0] = h;
				right->values[0] = h+1;
				for(int j = 1; j < order; j++)
				{
					left->values[j] = p+1+i.values[j-1];
					right->values[j] = p+i.values[j-1]; 
				}
				left->values[order] = l;
				right->values[order] = l-1;
				
				rule* new_rule = malloc(sizeof(rule));
				new_rule->left = left;
				new_rule->right = right;
			
				
				hash_entry* e = malloc(sizeof(hash_entry));
				e->payload = new_rule;
				e->key = hashKey(t->capacity, new_rule->left);
				e->next = NULL;
				
				insertEntry(t,e, e->key);
				
				
				// etats qui en decoulent
				pattern** prefix_left = getPrefixes(left);
				for(int n = 0; n < left->size-1; n++)
				{
					// collisions et unicite !!
					hash_entry* entry = malloc(sizeof(hash_entry));
					entry->payload = prefix_left[n];
					entry->key = hashKey(states->capacity, prefix_left[n]);
					entry->next = NULL;
					if(! existPattern(states,prefix_left[n], entry->key ))
						insertEntry(states, entry, entry->key );
				}
				
			}
			
			for(int j = 1; j < order; j++) 
			{
				
				pattern* iprime = subPattern(&i, j);
				if(p+ iprime->values[j-1] -1 >= 0)
				{
					pattern* nu = malloc(sizeof(pattern));
					nu->size = order;
					nu->values = malloc(nu->size*sizeof(int));
					for(int l = 0; l < nu->size; l++) 
					{
						nu->values[l] = 0; 
					}
	
					do {

						pattern* nuprime = subPattern(nu, order-j);

						pattern* tocomp = malloc(sizeof(pattern));
						tocomp->size = order-j;
						tocomp->values = malloc((order-j)*sizeof(int));
						
						for(int k = 0; k < order-j-1; k++)
						{
							tocomp->values[k]= p+1;
						}
						
						tocomp->values[order-j-1] = 1;

						if(compLexPat(nuprime, tocomp) <= 0)
							continue;
						
						pattern* left2 = malloc(sizeof(pattern));
						left2->size = order+j+1;
						left2->values = malloc((order+j+1)*sizeof(int));
						
						pattern* right2 = malloc(sizeof(pattern));
						right2->size = order+j+1;
						right2->values = malloc((order+j+1)*sizeof(int));
						
						left2->values[0] = h;
						right2->values[0] = h+1;
						
						for(int k = 0; k < iprime->size; k++)
						{
							left2->values[k+1]= p+iprime->values[k]+1;
							right2->values[k+1]= p+iprime->values[k];
							if(k == j-1)
							{
								right2->values[k+1]--;
							}
						}
						for(int k = 0; k < nu->size; k++) 
						{
							left2->values[j+k+1] = nu->values[k];
							right2->values[j+k+1] = nu->values[k];
							if(k >= order-j)
							{
								right2->values[j+k+1]++;
							}	
						}
						
						rule* new_rule2 = malloc(sizeof(rule));
						new_rule2->left = left2;
						new_rule2->right = right2;
						
						hash_entry* e = malloc(sizeof(hash_entry));
						e->payload = new_rule2;
						e->key = hashKey(t->capacity, new_rule2->left);
						e->next = NULL;
				
						insertEntry(t, e, e->key);
										
						
						pattern** prefixes_st = getPrefixes(left2);
						
						for(int k = 0; k < left2-> size-1; k++)
						{
							hash_entry* entry = malloc(sizeof(hash_entry));
							entry->payload = prefixes_st[k];
							entry->key = hashKey(states->capacity, prefixes_st[k]);
							entry->next = NULL;
							if(!existPattern(states, prefixes_st[k], entry->key))
								insertEntry(states, entry, entry->key);
							
						}
										
						
					}while(incPat(nu, p+2) == 0);
				} 
			}
		}while(incPat(&i, 2) == 0);
	}
	
}

void postNormalize(pattern* p, int order, int* value, int max_alph)
{
	int size_pat = p->size;
	if(p->size > order)
	{
		size_pat = order;
	}
			
	int i;
	
	int max = 0;
	for(i = 0; i < p->size; i++ )
	{
		if(p->values[i] > max)
			max = p->values[i];
	}
	
	if(max <= max_alph)
		return;
	
	int val = 0;
	int j = p->size-1;
	i = order - 1;
	while(i >= 0 && (j >= 0 && j >= p->size - order)) 
	{
		val += p->values[j]*value[i];
		i--;
		j--;
	}
	// If the pattern is for example "03201" for order 3, we want to expand the 2
	if(p->size > order )
	{
		pattern* suff = getSuffix(p, subPattern(p, p->size - order));
		max = getMax(suff);
	}	
	
	int r = val;
	int max_rep = 0;
	for(int i = 0; i < order; i++)
	{
		max_rep += (max-1)*value[i];
	}
	if(r > max_rep)
	{
		expand(p, order, value, max);
		return;
	}
	
	
	

	int* normalized = malloc((order)*sizeof(int));
	if(normalized == NULL)
		return;	
	i = 0;
	while(i < order)
	{
		normalized[i] = 0;
		if(r == value[i])
		{
			normalized[i] = 1;
			r -= value[i];
		}
		if(r > value[i]) 
		{
			int j = 1;
			while(r < (max-j)*value[i])
			{
				j++;
			}

			if(r - (max - j)*value[i] >= 0)
			{
				normalized[i] = (max - j);
				r -= (max - j)*value[i];		
			}
				
		}
		
		i++;
	}
	if(p->size <= order)
	{
		p->values = normalized;
		p->size = order;
	}
	else
	{
		for(int i = p->size - order, j= 0; i < p->size; i++, j++)
			p->values[i] = normalized[j];
	}		
}

void expand(pattern* p, int order, int* values, int alph_max)
{
	int dep = 0;
	if(p->size > order)
		dep = p->size - order;
	int found = 0;
	int r = 0;
	for(int i = dep; i < p->size; i++)
	{
		if(p->values[i] == alph_max && found == 0 && i < p->size-1)
		{
			p->values[i]--;
			found = 1;
			r = values[i-dep];
			continue;
		}
		
		if(found == 1 && r > 0)
		{
			if(values[i-dep] == r)
			{
				r = 0;
				p->values[i]++;
			}
			
			if(r > values[i-dep]) 
			{
				int j = r/values[i-dep];
				
				if(p->values[i] >= alph_max && i < p->size)
					j--;
				p->values[i] +=j;
				r -= j*(values[i-dep]);	
			}
			
		}
		
	}
}

void applyRules(hash_table* rules, pattern* p, int order)
{
	// need to generate prefixes of size m-2m-1
	for(int i = order; i < 2*order && i <= p->size; i++)
	{
		pattern* sub = subPattern(p, i);
		int key = hashKey(rules->capacity, sub );
		rule* r = getRule(rules, sub, key);
		if(r != NULL)
		{
			sub = r->right;
			for(int j = 0; j < sub->size; j++)
			{
				p->values[j] = sub->values[j];
			}
			return;
		}
	}
	
	
}


void outputState(pattern* output, int order, int* value, int max_alph, hash_table* rules)
{
	int max = getMax(output);
	if(max <= max_alph)
		return;
	
	postNormalize(output, order, value, max_alph);
	applyRules(rules, output, order);
	
	if(getMax(output) < max)
		return;
	outputState(output, order, value, max_alph, rules);
}



