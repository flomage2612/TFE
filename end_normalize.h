/*
* 	Functions for post normalization and normalization operations
*/

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



#include "pattern.h"
#include "hashtable_r.h"
#include "genData.h"
#include "composition.h"
#include "test_auto.h"
#include "addition.h"

automaton* end_normalize(hash_tab* states, int alph_max);

automaton* l_normalize(int order);

automaton* r_normalize(int order);