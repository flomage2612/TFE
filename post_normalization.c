//#define TEST_AUTO
//#define TEST_COMPO
//#define TEST_TEST_AUTO
//#define TEST_ADD
//#define END_NOR
#define TEST_INTE
//#define TEST_MAX_SIZE

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>


#include "lash.h"
#include "lash-diag.h"
#include "lash-auto.h"
#include "auto-io-print.h"
#include "auto-io-dots.h"
#include "lash-auto-operations.h"
#include "auto-minimize.h"
#include "auto-serialize.h"
#include "auto-quotient.h"



#include "pattern.h"
#include "hashtable_r.h"
#include "genData.h"
#include "composition.h"
#include "test_auto.h"
#include "addition.h"
#include "end_normalize.h"
#include "fib_add.h"


uint1* concat(int in, uint1* output, int sizeOut, int order);

automaton* get_red(int order, int alph_max);
void red_add(automaton* aut, uint4 state, uint1 toRed, bool isRest, uint1 alph, int nbTrans);
automaton* get_un(int order, int alph_max);
void un_add(automaton* aut, uint4 state, int j, int curr, uint1 alph, int order, bool red);
automaton* decalage(automaton* a, int nb_dec, int alph);
void add_identity(automaton* a, int alph);
void final_accepting(automaton* a, int alph_max);
void add_loop(automaton* a, int alph);
automaton* accept_all_size(int alph_max, int size_max);

// input format :  m a [ 1 1 1 ] [ 4 2 1 ]"
int main(int argc, char *argv[])
{
	uint1 g = 0;
	printf(" byte %u \n", g);
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


	hash_tab* rules = createhash_tab(pow(10,order+2));
	hash_tab* states = createhash_tab(pow(10,order));
	hash_tab* transitions = createhash_tab(pow(10,order)*alph_max);

	genData(order, alph_max, values, rules, states, transitions);
	printf("Data generated... \n");

	

	
	if (lash_init() < 0)
    lash_perror("lash_init");
	

	automaton* a1 = get_un(order, alph_max);
	automaton* a2 = get_red(order, alph_max);
	automaton* base = auto_product_union(a1, a2);
	auto_minimize(base);

	

	
#ifdef TEST_COMPO
	automaton* dec = decalage(base, 2, 3);
	add_identity(dec, 3);
	final_accepting(base,3);
	base = compose(base,2, dec,2, 3,3);
	base = auto_seq_projection_separ(base, 3, 1, NULL);
	auto_minimize(base);
	base = auto_unserialize(base, 2, NULL);
	auto_serialize_write_dot_file(base, "output.dot", LASH_EXP_DIGIT);
#endif 

#ifdef TEST_ADD
	automaton* addit = addition( 1);
	auto_serialize_write_dot_file(addit, "add.dot", LASH_EXP_DIGIT);
	automaton* r_nor = r_normalize(order);
	
	auto_minimize(r_nor);
	automaton* l_nor = l_normalize(order);
	
	auto_minimize(l_nor);
	
	automaton* normalizer = compose(l_nor, 2, r_nor, 2,3,3);
	auto_minimize(normalizer);
	normalizer = auto_seq_projection_separ(normalizer, 3, 1, NULL);
	
	
	auto_minimize(normalizer);	
	normalizer = auto_unserialize(normalizer, 2, NULL);
	r_nor = auto_unserialize(r_nor, 2, NULL);
	l_nor = auto_unserialize(l_nor, 2, NULL);
	auto_serialize_write_dot_file(r_nor, "r_nor.dot", LASH_EXP_DIGIT);
	auto_serialize_write_dot_file(l_nor, "l_nor.dot", LASH_EXP_DIGIT);
	auto_serialize_write_dot_file(normalizer, "nor.dot", LASH_EXP_DIGIT);	
	//auto_serialize_write_dot_file(l_nor, "ln.dot", LASH_EXP_DIGIT);


#endif	
#ifdef TEST_MAX_SIZE

	automaton* a = accept_all_size(3, 8*order);
	auto_serialize_write_dot_file(a, "max_size.dot", LASH_EXP_DIGIT);
	automaton* fib = fib_addition();
	fib = compose(fib, 3, a, 1, 3, 3);
	auto_serialize_write_dot_file(fib, "fib_size.dot", LASH_EXP_DIGIT);
#endif
#ifdef END_NOR
	automaton* en = end_normalize(states, alph_max);
	en = auto_unserialize(en, 2, NULL);
	auto_serialize_write_dot_file(en, "en.dot", LASH_EXP_DIGIT);	
#endif
	 
#ifdef TEST_AUTO
	automaton* next = auto_copy(base);
	add_identity(next, 3);
	add_loop(next, 3);

	auto_minimize(next);
	 
	for(int k = 1; k <= order + 1; k++)
	{
		automaton* dec = decalage(base, k, 3);
		add_identity(dec, 3);
		add_loop(dec, 3);
		auto_minimize(dec);
		next = compose(next,2, dec,2, 3, 3);
		auto_minimize(next);

		next = auto_seq_projection_separ(next, 3, 1, NULL);
		auto_minimize(next);
	}
	auto_prune(next);
	add_loop(next, 3);
	auto_minimize(next);
	auto_serialize_write_dot_file(next, "result.dot", LASH_EXP_DIGIT);
	
#ifdef TEST_TEST_AUTO
	uint1 word[6] = {0,0,2,2,2,0};
	//scanf("%6c", word);
	automaton* b = test_automata(next, word, 6, order);

	// automaton which accepts only the aimed alphabet
	automaton* limited = auto_new_empty(1);
	uint4* i_st = 0;
	auto_add_new_state(limited, & i_st);
	auto_add_new_i_state(limited, i_st);
	auto_mark_accepting_state(limited, i_st);
	uint1 zer = 0, one = 1, two = 2;
	auto_add_new_transition(limited, i_st, i_st, 1, &zer );
	auto_add_new_transition(limited, i_st, i_st, 1, &one );
	//auto_add_new_transition(limited, i_st, i_st, 1, &two );


	automaton* limit_alph = auto_intersection(b, limited);
	auto_minimize(limit_alph);
	auto_serialize_write_dot_file(limit_alph, "limit.dot", LASH_EXP_DIGIT);
#endif
#endif

#ifdef TEST_INTE

	automaton* addit = addition(1);
	automaton* r_nor = r_normalize(order);
	automaton* l_nor = l_normalize(order);

	auto_minimize(addit);
	auto_minimize(r_nor);
	auto_minimize(l_nor);

	automaton* en = end_normalize(states, alph_max);
	auto_serialize_write_dot_file(en, "en.dot", LASH_EXP_DIGIT);

	auto_minimize(en);

	printf("Addition bit by bit, normalizations and alphabet limitation generated \n ");
	automaton* next = auto_copy(base);
	add_identity(next, 3);
	//add_loop(next, 3);
	final_accepting(next, 3);

	auto_minimize(next);
	base = auto_copy(next);
	automaton* next2 = decalage(next, 4*order, 3);
	automaton* base2 = auto_copy(next2);
	auto_minimize(next2);

	
	for(int k = 1; k < 2*order; k++)
	{
		automaton* dec = decalage(base, k, 3);
		auto_serialize_write_dot_file(dec, "dec.dot", LASH_EXP_DIGIT);
		automaton* dec2 = decalage(base2, k, 3);
		add_identity(dec, 3);
		add_identity(dec2, 3);
		
		final_accepting(dec, 3);
		final_accepting(dec2, 3);
		auto_minimize(dec);
		auto_minimize(dec2);
		next = compose(next,2, dec,2, 3, 3);
		next2 = compose(next2,2, dec2,2, 3, 3);
		auto_minimize(next);
		auto_minimize(next2);
		next = auto_seq_projection_separ(next, 3, 1, NULL);
		auto_minimize(next);
		next2 = auto_seq_projection_separ(next2, 3, 1, NULL);
		auto_minimize(next2);

	}
	//add_loop(next, 3);
	next = compose(next, 2, next2, 2, 3, 3);
	next = auto_seq_projection_separ(next, 3, 1, NULL);
	final_accepting(next, 1);
	add_loop(next, 3);

	auto_prune(next);
	auto_minimize(next);
	next = compose(next, 2, en, 2, 3, 3);
	next = auto_seq_projection_separ(next, 3, 1, NULL);
	auto_minimize(next);
	auto_minimize(next);
	uint1 word[4] = {0 ,2,0,0,0,0};
	//scanf("%6c", word);
	automaton* normalize_test = test_automata(next, word, 4, order);
	auto_serialize_write_dot_file(normalize_test, "nor_test.dot",LASH_EXP_DIGIT);
	automaton* n2 = auto_unserialize(next, 2, NULL);
	auto_minimize(n2);
	auto_serialize_write_dot_file(n2, "nor_c.dot",LASH_EXP_DIGIT);


	//add_loop(next2, 3);
	//auto_prune(next2);
	//auto_minimize(next2);
	//next = compose(next, 2, next2,2, 3, 3);
	//next = auto_seq_projection_separ(next, 3, 1, NULL);
	//auto_minimize(next);
	//add_loop(next, 3);
	//auto_prune(next);
	//auto_minimize(next);
	printf("Normalization post add generated  %d \n ", auto_nb_states(next));

	automaton* result = compose(addit, 3, next, 2, 3, 4);
	result = auto_seq_projection_separ(result, 4, 2, NULL);
	auto_minimize(result);
	result = compose(result, 3, next, 2, 3, 4);
	result = auto_seq_projection_separ(result, 4, 2, NULL);
	auto_minimize(result);
	printf("Composition of addition and post normalization %d \n ", auto_nb_states(result));
	
	
	printf("Composition of end normalization \n ");
	//result = auto_seq_projection_separ(result, 4, 2, NULL);


	auto_serialize_write_dot_file(result, "prenor.dot", LASH_EXP_DIGIT);

	result = compose(result, 3, l_nor, 2, 3, 4);
	result = auto_seq_projection_separ(result, 4, 2, NULL);
	auto_minimize(result);
	//automaton* r_nor_r = auto_reverse(r_nor);
	result = compose(result, 3, r_nor, 2, 3, 4);
	result = auto_seq_projection_separ(result, 4, 2, NULL);
	auto_minimize(result);
	automaton* result2 = auto_unserialize(result, 3, NULL);
	auto_minimize(result2);
	printf("Final normalizations %d \n ", auto_nb_states(result2));
	auto_serialize_write_dot_file(result2, "add.dot", LASH_EXP_DIGIT);

	automaton* fib = fib_addition();
	automaton* a = accept_all_size(3, 8*order);
	auto_serialize_write_dot_file(a, "max_size.dot", LASH_EXP_DIGIT);
	fib = compose(fib, 3, a, 1, 3, 3);
	auto_serialize_write_dot_file(fib, "fib_add.dot", LASH_EXP_DIGIT);
	auto_minimize(fib);
	if(auto_inclusion(fib, result) == 1)
		printf("SUCCEEEESSS\n");
	automaton* dif = auto_difference(fib,result);
	auto_minimize(dif);
	dif = auto_unserialize(dif, 3, NULL);
	auto_minimize(dif);

	auto_serialize_write_dot_file(dif, "dif.dot", LASH_EXP_DIGIT);
	printf("%d \n",auto_nb_states(dif));

	auto_serialize_write_dot_file(result, "addition.dot", LASH_EXP_DIGIT);

	uint1 w1[7] = {0 ,2,0,2,0,2,0};;
	uint1 w2[7] = {0,0,0,0,0,0,0};
	//scanf("%6c", word);
	automaton* b = test_automata2(result, w1,7,w2,7, order);
	automaton* c = test_automata2(fib, w1,4,w2,4, order);

	auto_serialize_write_dot_file(b, "test.dot", LASH_EXP_DIGIT);
	auto_serialize_write_dot_file(c, "test2.dot", LASH_EXP_DIGIT);
	


#endif
	
	if (lash_end() < 0)
	    lash_perror("lash_end");

	printf("### statistics ###\n");
	printf("residual memory : %llu byte(s).\n", lash_get_mem_usage());
	printf("max memory      : %llu byte(s).\n", lash_get_max_mem_usage());
	printf("### end of the test ###\n");   
	
	for(int i = 0; i < states->capacity; i++)
	{
		if(states->tab[i] != NULL)
		{
			freeStatef(states->tab[i]->payload);
		}
	}
	freehash_tab(states);

	for(int i = 0; i < rules->capacity; i++)
	{
		if(rules->tab[i] != NULL)
		{
			freeRule(rules->tab[i]->payload);
		}
	}
	freehash_tab(rules);

	for(int i = 0; i < transitions->capacity; i++)
	{
		if(transitions->tab[i] != NULL)
		{
			freeTransition(transitions->tab[i]->payload);
		}
	}
	freehash_tab(transitions);


	exit(0);

}


automaton* get_un(int order, int alph_max)
{
	automaton* aut = auto_new_empty(sizeof(char));

	if(aut == NULL)
  		lash_perror("automaton creation");

  	uint4 init = 0;

  	if(auto_add_new_state(aut, &(init)) != 0)
			lash_perror("state creation");

	auto_add_new_i_state(aut, init);

	uint4 second = 0;
	if(auto_add_new_state(aut, &(second)) != 0)
			lash_perror("state creation");	
	if(auto_add_new_transition(aut, init, second, 2, createLabel(0,1)) !=0 )
		lash_perror("T0");

	for(int j = 1 ; j < order; j++)
	{
		un_add(aut, second, j, 1, alph_max*2, order, false);
	}

	return aut;
}

void un_add(automaton* aut, uint4 state, int j, int curr, uint1 alph, int order, bool red)
{
	if(curr ==  j+order+1 )
		return;
	uint4 newState = 0;
	if(auto_add_new_state(aut, &newState) != 0)
			lash_perror("state creation");
	if(j == curr)
	{
		for(uint1 x = 2; x <= alph+1; x ++)
		{
			auto_add_new_transition(aut, state, newState, 2, createLabel(x, x-2));

		}
		un_add(aut, newState, j, curr+1, alph, order, false);
	}
	else if(curr < j)
	{
		for(int x = 1 ; x <= alph+1; x++)
		{
			auto_add_new_transition(aut, state, newState, 2, createLabel(x, x-1));
		}
		un_add(aut, newState, j, curr+1, alph, order, false);
	}
	else if(j < curr && curr <= order )
	{
		if(red)
		{
			for(int x = 0; x <= alph; x++)
			{
				auto_add_new_transition(aut, state, newState, 2, createLabel(x,x));
			}
			un_add(aut, newState, j, curr+1, alph, order, red);
		}
		else
		{
			uint4 newState2 = 0;
			auto_add_new_transition(aut, state, newState, 2, createLabel(alph-2, alph-2));
			un_add(aut, newState, j,curr+1, alph, order, true);

			

			if(curr != order)
			{
				auto_add_new_state(aut, &newState2);
				auto_add_new_transition(aut, state, newState2, 2, createLabel(alph-1, alph-1));
				un_add(aut, newState2, j,curr+1, alph, order, false);
			}
			

		}
	}

	else if(order < curr && curr <= j+order)
	{
		for(uint1 i = 0; i <= alph; i++)
		{
			auto_add_new_transition(aut, state, newState, 2, createLabel(i, i+1));
			
		}
		un_add(aut, newState, j, curr+1, alph, order, red);

	}

	if(curr == j+order)
	{
		auto_mark_accepting_state(aut, newState);
	}

}

automaton* get_red(int order, int alph_max)
{
	automaton* aut = auto_new_empty(sizeof(char));

	if(aut == NULL)
  		lash_perror("automaton creation");

  	uint4 init = 0;

  	if(auto_add_new_state(aut, &(init)) != 0)
			lash_perror("state creation");

	auto_add_new_i_state(aut, init);

  	int red_max = alph_max*2;

  	for(uint1 i = 1; i <= red_max; i++)
  	{
  		uint4 second = 0; 
  		if(auto_add_new_state(aut, &second) != 0)
  			lash_perror("second state creation");

  		auto_add_new_transition(aut, init, second,2, createLabel(0,i));

  		red_add(aut, second, i, false, alph_max*2, order);


  	}
  	return aut;

}
/**
* alph: upper bound of the input alphabet (in general 2)
**/
void red_add(automaton* aut, uint4 state, uint1 toRed, bool isRest, uint1 alph, int nbTrans)
{
	if(nbTrans == 0)
		return;

	uint4 newState = 0;
	auto_add_new_state(aut, &newState);

	if(! isRest)
	{
		auto_add_new_transition(aut, state, newState, 2, createLabel(toRed, 0));
		red_add(aut, newState, toRed, true, alph, nbTrans-1);

		if(nbTrans != 1)
		{
			uint4 newState2 = 0;
			auto_add_new_state(aut, &newState2);

			for(uint1 i = toRed+1; i <= alph + 1; i++)
			{
				auto_add_new_transition(aut, state, newState2, 2, createLabel(i, i-toRed));
			}
			red_add(aut, newState2, toRed, false, alph, nbTrans-1);
		}
	}
	else
	{
		for(uint1 i = toRed; i <= alph  + 1; i++)
		{
			auto_add_new_transition(aut, state, newState, 2, createLabel(i, i-toRed));
		}
		red_add(aut, newState, toRed, isRest, alph, nbTrans-1);
	}

	if(nbTrans == 1)
		auto_mark_accepting_state(aut, newState);

}


uint1* concat(int in, uint1* output, int sizeOut, int order)
{
	uint1* res = calloc(2*order, sizeof(int));
	uint1* k = (uint1*) &in;
	int sizeIn = order*sizeof(int);
	for(int i = 0; i < sizeof(int) ; i++)
	{
		res[sizeIn-1-i] = k[sizeof(int)-1-i];
	}
	if(output == NULL)	
		return res;

	
	for(int i = 0; i < sizeOut; i++)
	{
		res[sizeIn+i] = output[i];
	}
	return res;

}

automaton* decalage(automaton* a, int nb_dec, int alph)
{
	automaton* b = auto_copy(a);
	uint4 i_state = 0;
	auto_i_state(a, 0, &i_state);
	auto_remove_i_states(b);
	uint4 newInit= 0;
	auto_add_new_state(b, &newInit);
	auto_add_new_i_state(b, newInit);
	auto_mark_accepting_state(b, newInit);
	uint4 prev = newInit;
	for(int i = 1; i < nb_dec; i++)
	{
		uint4 newState = 0;
		auto_add_new_state(b, &newState);
		for(int j = 0; j <= alph; j++)
		{
			auto_add_new_transition(b, prev, newState, 2, createLabel(j,j));
		}
		prev = newState;
	}
	

	for(int i = 0; i <= alph; i++)
	{
		auto_add_new_transition(b, prev, i_state, 2, createLabel(i,i));
	}
	return b;
}

void add_loop(automaton* a, int alph)
{
	uint4 init = 0;
	auto_i_state(a, 0, &init);
	
	for(int i = 0; i < auto_nb_states(a); i++)
	{
		if(auto_accepting_state(a, i))
		{
			auto_unmark_accepting_state(a, i);
			auto_add_new_transition(a, i, init, 0, NULL);
		}
	}
	auto_mark_accepting_state(a, init);

}

void final_accepting(automaton* a, int alph_max)
{
	for(int i = 0; i < auto_nb_states(a); i++)
	{
		if(auto_accepting_state(a, i))
		{
			for(uint1 j = 0; j <= alph_max; j++)
			{
				auto_add_new_transition(a, i, i, 2, createLabel(j,j));
			}
		}
	}


}

void add_identity(automaton* a, int alph)
{
	uint4 puits = 0;
	auto_add_new_state(a, &puits);
	int init = 0;  
	auto_i_state(a, 0, &init); 
	auto_add_new_transition(a, init, puits, 0, NULL );
	for(int i = 0; i <= alph; i++)
	{
		auto_add_new_transition(a, init, puits, 2, createLabel(i,i) );
		auto_add_new_transition(a, puits, puits, 2, createLabel(i,i));
	}
	auto_mark_accepting_state(a, puits);
}

automaton* accept_all_size(int alph_max, int size_max)
{
	automaton* a = auto_new_empty(1);
	uint4 init;
	if(auto_add_new_state(a, &init) != 0)
		lash_perror("state creation");
	auto_add_new_i_state(a, init);
	uint4 prev = init;
	auto_mark_accepting_state(a, init);
	for(int i = 0 ; i < size_max; i++)
	{
		uint4 new;
		if(auto_add_new_state(a, &new) != 0)
			lash_perror("state creation");
		auto_mark_accepting_state(a, new);
		for(int j = 0; j <= alph_max; j++)
		{
			uint1 label = j;
			if(auto_add_new_transition(a, prev, new, 1, & label) != 0 )
				lash_perror("transition creattion");
		}
		prev = new;
	}
	return a;
}
