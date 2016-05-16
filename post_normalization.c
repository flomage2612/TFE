#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "pattern.h"
#include "rule.h"
#include "hashtable_r.h"



void postNormalize(pattern* p, int order, int* value, int alph_max);
void genRules(int alphMax, int order, int* values, hash_table* t, hash_table* states );
void expand(pattern* p, int order, int* values);
void outputState(pattern* output, int order, int* value, int max_alph, hash_table* rules);
void genTransitions(hash_table* states, hash_table* rules, hash_table* transitions, int alph_max, int order, pattern* state);
int applyRules(hash_table* rules, pattern* p, int order);

// input format :  m [ 1 1 1 ] [ 4 2 1 ]"
int main(int argc, char *argv[])
{


	if(argc < 3)
		return 1;
	
	int order = atoi(argv[1]);
	int alph_max = atoi(argv[2]);
	int coefficients[order];
	int values[order];
	int i = 3,j = 0, k = 0;

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
	hash_table* rules = createhash_table(pow(10,order+2));
	hash_table* states = createhash_table(pow(10,order));
	hash_table* transitions = createhash_table(pow(10,order)*alph_max);
	genRules(alph_max, order, values, rules, states);
	
	//printTablePattern(states);
	//printTableRules(rules);	
/*
	for(int i = 0; i < states->capacity; i++)
	{
		if(states->tab[i] == NULL)
			continue;
			
		genTransitions(states,rules, transitions, alph_max, order, ((pattern*)states->tab[i]->payload));

		printf("\n");
	}
	printTableTransitions(transitions);
	//printTableRules(rules);
	*/
	pattern* d = malloc(sizeof(pattern));
	int tab[3] = {0,3, 2};
	d->values = tab;
	d->size = 3;
	rule* r = getRule(rules, d, hashKey(rules->capacity, d));
	if(r == NULL)
		printf("WTF \n");
	else
		printRule(r);
	genTransitions(states,rules, transitions, alph_max, order, d);
	printTableTransitions(transitions);
	/*
	printf("%i %i \n", rules->numberofelements, states->numberofelements);
	int curr = 0;
	for(int i = 0; i < states->capacity; i++)
	{
		if(states->tab[i] == NULL)
			continue;
		pattern* p;
		hash_entry* e = states->tab[i];
		p = ((pattern*) e->payload);
		printPattern(p);
		pattern* o = copyPattern(p);
		outputState(o, order, values, alph_max, rules);
						
		printPattern(o);
		printf("%i out of %i \n", curr, states->numberofelements);
		printf("\n");
		while(e->next != NULL)
		{
			e = e->next;
			p = ((pattern*) e->payload);
			printPattern(p);
			o = copyPattern(p);
			outputState(o, order, values, alph_max, rules);
							
			printPattern(o);
			printf("%i out of %i \n", curr, states->numberofelements);
			printf("\n");
			
			
			curr++;
		}
		curr++;
	}
	*/

	

	return 0;
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
			for(int l = 1; l <= p+3; l++)
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
				
				pattern* iprime = subPattern(&i,0, j-1);
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

						pattern* nuprime = subPattern(nu,0, order-j-1);

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


void expand(pattern* p, int order, int* values)
{
	int sum = 0;
	int val = 0;
	int dep = 0;
	if(p->size > order)
		dep = p->size - order;
		
	for(int i = 0, j  = dep; i < order, j < p->size; i++, j++)
	{
		sum += values[i];
		val += values[i]*p->values[j];
		 
	}
	int min = val/sum;

	

	for(int i = dep; i < p->size; i++)
		p->values[i] = min;	
	


	int r = val - min*sum;
	for(int i = dep; i < p->size; i++)
	{
				
			if(values[i-dep] == r)
			{
				r = 0;
				p->values[i]++;
			}
			
			if(r > values[i-dep]) 
			{
				
				p->values[i] += 1;
				r -= (values[i-dep]);	
			}
			
	}
}

int applyRules(hash_table* rules, pattern* p, int order)
{
	
	if(p->size <= order)
		return 0;
	
		
	// need to generate prefixes of size m+1 - 2m-1
	for(int i = order+1; i <= 2*order-1 && i <= p->size; i++)
	{
		for(int j = 0; j <= p->size - i; j++)
		{
			
			if(p->values[j] == 0)
			{

				pattern* sub = subPattern(p,j, i+j-1);

				int key = hashKey(rules->capacity, sub );
				rule* r = getRule(rules, sub, key);
				if(r != NULL)
				{
					sub = r->right;
					for(int k = 0; k < sub->size; k++)
					{
						p->values[j+k] = sub->values[k];
					}
					return 1;
				}
			}
		}
	}	
	
	return 0;
	
}


void outputState(pattern* output, int order, int* value, int max_alph, hash_table* rules)
{
	
	int max = getMax(output);
	if(max <= max_alph)
		return;
	expand(output, order, value);
	while(applyRules(rules, output, order) == 1)
	{
		expand(output, order, value);
	}
}

void genTransitions(hash_table* states, hash_table* rules, hash_table* transitions, int alph_max, int order, pattern* state)
{
	for(int i = 0; i <= alph_max*2; i ++)
	{
		pattern* current = append(state, i);
		rule *r = getRule(rules, current, hashKey(rules->capacity, current));
		if(r == NULL)
		{
			if(existPattern(states, current,hashKey(states->capacity, current) ))
			{
				transition* t = malloc(sizeof(transition));
				t->st_state = state;
				t->en_state = current;
				t->input = i;
				t->output = NULL;
				
				hash_entry* e = malloc(sizeof(hash_entry));
				e->payload = t;
				e->key = hashKey(transitions->capacity, state);
				e->next = NULL;
				insertEntry(transitions, e, e->key);
			}
			else 
			{
				pattern** suffixes = getSuffixes(current);
				for(int j = current->size-2; j >= 0; j--)
				{
					if(existPattern(states, suffixes[j],hashKey(states->capacity, suffixes[j]) ))
					{
						transition* t = malloc(sizeof(transition));
						t->st_state = state;
						t->en_state = suffixes[j];
						t->input = i;
						t->output = getPrefix(current, suffixes[j]->size);
				
						hash_entry* e = malloc(sizeof(hash_entry));
						e->payload = t;
						e->key = hashKey(transitions->capacity, state);
						e->next = NULL;
						insertEntry(transitions, e, e->key);
						break;
					}
				}
			}
		}
		else
		{
			pattern* next = r->right;
			pattern** suffixes = getSuffixes(next);
			bool done = false;
			for(int j = 0; j < next->size - 1  && !done; j++ )
			{
				pattern* out = getPrefix(next, suffixes[j]->size);
				if(existPattern(states, suffixes[j],hashKey(states->capacity, suffixes[j]) ) )
				//&& getMax(out) <= alph_max*2 - 1)
					{
						transition* t = malloc(sizeof(transition));
						t->st_state = state;
						t->en_state = suffixes[j];
						t->input = i;
						t->output = out;
				
						hash_entry* e = malloc(sizeof(hash_entry));
						e->payload = t;
						e->key = hashKey(transitions->capacity, state);
						e->next = NULL;
						insertEntry(transitions, e, e->key);
						done = true;
					}
			}
			if(!done)
			{
				if(existPattern(states, next,hashKey(states->capacity, next) ))
				{
					transition* t = malloc(sizeof(transition));
					t->st_state = state;
					t->en_state = next;
					t->input = i;
					t->output = NULL;
				
					hash_entry* e = malloc(sizeof(hash_entry));
					e->payload = t;
					e->key = hashKey(transitions->capacity, state);
					e->next = NULL;
					insertEntry(transitions, e, e->key);
				}
				else 
				{
					printf("SOUUUCIII\n");
					printPattern(current);
				}
			}

		}
	}
}



