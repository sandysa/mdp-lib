#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ARITY 10
#define MAX_ACTION_ARITY 10
#define MAX_NUM_PROB_EFFECTS 10
#define MAX_NUM_PRECONDITIONS 10
#define MAX_NUM_EFFECTS 10
#define NUM_PREDICATES 100
#define NUM_ACTIONS 1000
#define LEN_ACTION_NAME 4
#define LEN_OBJECT_NAME 3
#define MAX_LENGTH 100

typedef struct
{
  int predicate_num;
  int arity;
} predicate_type;


typedef struct 
{
  int predicate_num;
  int args[MAX_ARITY];
  int negated;
} lifted_predicate_type;


typedef struct 
{
  int predicate_num;
  int args[MAX_ARITY];
  int negated;
} grounded_predicate_type;


typedef struct 
{
  int prob;
  int num_effs;
  lifted_predicate_type and_effects[MAX_NUM_EFFECTS];
} prob_effect_type;

typedef struct 
{
  lifted_predicate_type and_preconditions[MAX_NUM_PRECONDITIONS];
} precondition_type;

/* only check that I will do is preventing segmentation fault */

typedef struct 
{
  int valid;
  char name[LEN_ACTION_NAME];
  int arity;
  int num_preconds;
  int num_probeffs;
  int parameters[MAX_ACTION_ARITY];
  precondition_type precondition;
  prob_effect_type prob_effects[MAX_NUM_PROB_EFFECTS];
} prob_action_type;


typedef struct 
{
  char name[LEN_ACTION_NAME];
  int parameters[MAX_ACTION_ARITY];
  precondition_type precondition;
  int num_effects;
  lifted_predicate_type and_effects[MAX_NUM_EFFECTS];
} det_action_type;

typedef struct
{
  char name[LEN_ACTION_NAME];
  int num_prob_effects;
  int *probs;
  char *prob_name[LEN_ACTION_NAME+1];
} action_table_type;

struct _command_line {

  char path[MAX_LENGTH];
  char ops_file_name[MAX_LENGTH];
  char fct_file_name[MAX_LENGTH];
  int display_info;
  int debug;

};

char alphabet[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
		   'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
		   'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
		   'Y', 'Z'};


char small_alphabet[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
			 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
			 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
			 'y', 'z'};

char *params[] = {"?X", "?Y", "?Z", "?W", "?V", "?U"};

int len_predicate = 3;
int len_action = 4;
char **predicates;
char **objects; 
int *predicates_arity_info;
grounded_predicate_type *init_predicates;
int gnum_ractions;
struct _command_line gcmd_line;

/* variables */
int grandom_walk_length = 100;
int gmax_arity = 2; /*predicate arity*/
int gmax_action_arity = 3;
int gnum_prob_effects = 3;
int gnum_effects = 3;
int gnum_preconditions = 4;
int gnum_init_facts = 100;
int gnum_objects = 10;
int gnum_actions = 5;
int gnum_predicates = 5;
int gnum_prob_negated_preconds = 30; /* among 100 */
int gnum_prob_negated_effects = 30; /* among 100 */
float g_path_prob_limit = 0.3; 


void print_lifted_predicate(lifted_predicate_type pred, FILE *fp) 
{
  int i;

  if(pred.negated)
    fprintf(fp, "(not (%s", predicates[pred.predicate_num]);
  else
    fprintf(fp, "(%s", predicates[pred.predicate_num]);
  for(i=0; i< predicates_arity_info[pred.predicate_num]; i++)
    fprintf(fp, " %s", params[pred.args[i]]);

  if(pred.negated)
    fprintf(fp, ")) ");
  else
    fprintf(fp, ") ");
}


void print_grounded_predicate(grounded_predicate_type pred, FILE *fp) 
{
  int i;

  if(pred.negated)
    fprintf(fp, "(not (%s", predicates[pred.predicate_num]);
  else
    fprintf(fp, "(%s", predicates[pred.predicate_num]);
  for(i=0; i< predicates_arity_info[pred.predicate_num]; i++)
    fprintf(fp, " %s", objects[pred.args[i]]);

  if(pred.negated)
    fprintf(fp, ")) ");
  else
    fprintf(fp, ") ");
}

int same_grounded_predicate(grounded_predicate_type pred1, grounded_predicate_type pred2) 
{
  int i;

  if( pred1.predicate_num != pred2.predicate_num )
    return 0;

  for(i=0; i < predicates_arity_info[pred1.predicate_num]; i++)
    if( pred1.args[i] != pred2.args[i] )
      break;

  if (i == predicates_arity_info[pred1.predicate_num] )
    return 1;
  else
    return 0;
}

int same_lifted_predicate(lifted_predicate_type pred1, lifted_predicate_type pred2) 
{
  int i;
  
  if( pred1.predicate_num != pred2.predicate_num)
    return 0;

  for(i=0; i < predicates_arity_info[pred1.predicate_num]; i++)
    if( pred1.args[i] != pred2.args[i] )
      break;

  return (i == predicates_arity_info[pred1.predicate_num] );
}


/* consider negation also */
int same_lifted_predicate_2(lifted_predicate_type pred1, lifted_predicate_type pred2) 
{
  int i;
  
  if( pred1.predicate_num != pred2.predicate_num ||
      pred1.negated != pred2.negated )
    return 0;

  for(i=0; i < predicates_arity_info[pred1.predicate_num]; i++)
    if( pred1.args[i] != pred2.args[i] )
      break;

  if  (i == predicates_arity_info[pred1.predicate_num] )
    return 1;
  else
    return 0;
}


int same_precondition( precondition_type prec1, precondition_type prec2, int num_preconds ) 
{
  int i,j;

  for(i = 0; i < num_preconds; i++) {

    for(j=0; j< num_preconds; j++) 
      if( same_lifted_predicate_2(prec1.and_preconditions[i], prec2.and_preconditions[j]) )
	break;

    if( j == num_preconds )
      break;
  }

  if (i ==  num_preconds )
    return 1;
  else
    return 0;

}

int same_effects (prob_effect_type peff1, prob_effect_type peff2) 
{
  int i,j;

  if( peff1.num_effs != peff2.num_effs )
    return 0;

  /* deleted effects */
  for(i=0; i< peff1.num_effs; i++) {
    for(j=0; j< peff1.num_effs; j++)
      if( same_lifted_predicate_2(peff1.and_effects[i], peff2.and_effects[j]))
	break;
    if ( j == peff1.num_effs )
      break;
  }

  if( i < peff1.num_effs )
    return 0;

  return 1;
}

     
int same_prob_effects( prob_effect_type *peff1, prob_effect_type *peff2, int num_probeffs ) 
{
  int i,j;

  for(i=0; i< num_probeffs; i++) {
    for(j=0; j < num_probeffs; j++) {
      if( same_effects(peff1[i], peff2[j]) )
	break;
    }

    if( j == num_probeffs )
      break;
  }

  if (i ==  num_probeffs )
    return 1;
  else
    return 0;
}

int same_action(prob_action_type action1, prob_action_type action2) 
{
  if(action1.arity != action2.arity ||
     action1.num_preconds != action2.num_preconds ||
     action1.num_probeffs != action2.num_probeffs )
    return 0;

  if(same_precondition(action1.precondition, action2.precondition, action1.num_preconds) &&
     same_prob_effects(action1.prob_effects, action2.prob_effects, action1.num_probeffs))
    return 1;

  return 0;
}



/* I would not enforce this */
/*int pass_precondition_arg_check(lifted_predicate_type pred)*/

				
    /* precondition argument check later 
       for(i=1; i < predicates_arity_info[temp_pred_num]; i++) {
       for(j=0; j< i; j++) 
       if( actions[action_num].precondition.and_preconditions[p_num].args[j] ==
       actions[action_num].precondition.and_preconditions[p_num].args[i] )
       break;
       if(j < i)
       break;
       }
       if(i < predicates_arity_info[temp_pred_num] )
       continue;
    */


int lifted_predicates_consistency_check(lifted_predicate_type *preds, int num_preds) 
{
  int i,j;

  for(i=0; i< num_preds; i++) {
    
    for(j=0; j< i; j++)
      /* whethere it is negated or not they have to be different */
      if(same_lifted_predicate(preds[j], preds[i]))
	break;

    if(j < i)
      break;
    else
      continue;
  }

  if( i == num_preds )
    return 1; /* invalid */

  return 0; /* invalid */
}


int useless_addition_check( prob_action_type action) 
{
  int i,j,k;

  for(i=0; i< action.num_probeffs; i++) {
    for(j=0; j< action.prob_effects[i].num_effs; j++) {
      for(k=0; k< action.num_preconds; k++) {
	if( same_lifted_predicate_2( action.precondition.and_preconditions[k],
				     action.prob_effects[i].and_effects[j] ) )
	  return 0;
      }
    }
  }

  return 1;
}

int action_arity_usage_check(prob_action_type action) 
{
  int i,j,k;
  int arity[MAX_ACTION_ARITY];

  for(i=0; i< gmax_action_arity; i++)
    arity[i] = 0;

  /* precondition */
  for(i=0; i < action.num_preconds; i++) 
    for(j=0; j< predicates_arity_info[action.precondition.and_preconditions[i].predicate_num]; j++)
      arity[action.precondition.and_preconditions[i].args[j]] = 1;
  
  /* effects */
  for(i=0; i< action.num_probeffs; i++) 
    for(j=0; j < action.prob_effects[i].num_effs; j++)
      for(k=0; k< predicates_arity_info[action.prob_effects[i].and_effects[j].predicate_num]; k++)
	arity[action.prob_effects[i].and_effects[j].args[k]] = 1;


  for(i=0; i < action.arity; i++)
    if(!arity[i])
      break;

  if( i != action.arity )
    return 0;
  else
    return 1;
}


/* each prob effects must use all the arities */    
int action_arity_usage_check_strong(prob_action_type action) 
{
  int i,j,k,p;
  int arity[MAX_ACTION_ARITY];


  for(p=0; p< action.num_probeffs; p++) {
    
    for(i=0; i< action.arity; i++)
      arity[i] = 0;
    
  /* precondition */
    for(i=0; i < action.num_preconds; i++) 
      for(j=0; j< predicates_arity_info[action.precondition.and_preconditions[i].predicate_num]; j++)
	arity[action.precondition.and_preconditions[i].args[j]] = 1;
  
  
    for(i=0; i < action.arity; i++)
      if(!arity[i])
	break;

    if( i != action.arity ) {
      printf("sss gettingout");
      return 0;
    }
    
  }

  printf("sss gettingout");
  
  return 1;
  
}



void print_action_prob(prob_action_type action, FILE *prob_file) 
{
  int i, j;
  
  fprintf(prob_file, "(:action %s\n", action.name);

  
  fprintf(prob_file, " :parameters (");
  for(i =0; i < action.arity; i++)
    fprintf(prob_file, "%s ", params[action.parameters[i]]);
  fprintf(prob_file, ")\n");

  
  fprintf(prob_file, " :precondition (and \n");
  for(i=0; i< action.num_preconds; i++) {
    fprintf(prob_file, "\t\t ");
    print_lifted_predicate(action.precondition.and_preconditions[i], prob_file);
    fprintf(prob_file, "\n");
  }
  fprintf(prob_file, "  )\n");

  
  fprintf(prob_file, " :effect (probabilistic \n");
  for(i=0; i < action.num_probeffs ; i++) {
    fprintf(prob_file, "\t\t %d/100 (and ", action.prob_effects[i].prob
	    /*(float)action.prob_effects[i].prob / (float)100*/);
    for(j=0; j< action.prob_effects[i].num_effs; j++)
      print_lifted_predicate(action.prob_effects[i].and_effects[j], prob_file);
    fprintf(prob_file, ")  \n");
  }
    
  fprintf(prob_file, "          )\n");
  fprintf(prob_file, " )\n");
}

void print_pddl_prob(prob_action_type *actions, FILE *prob_file) 
{
  int i, j, peff_num, param_num, action_num, valid_action_cnt;
  
  fprintf(prob_file, "(define (domain prob_domain) \n");
  /* no typing nor conditional effects */
  fprintf(prob_file, " (:requirements :strips :probabilistic-effects) \n");
  /*printf(" (:types pos_x pos_y) \n");*/
  /* printf(" (:constants X1 X2 X3 X4 - pos_x Y1 Y2 Y3 Y4 - pos_y) \n"); */
  fprintf(prob_file, " (:predicates \n");
  for(i=0; i< gnum_predicates; i++) {
    fprintf(prob_file, "\t (%s ", predicates[i]);
    for(j=0; j< predicates_arity_info[i]; j++) 
      fprintf(prob_file, "%s ", params[j]);
    fprintf(prob_file, ") \n");
  }
  fprintf(prob_file, " )\n");

  valid_action_cnt = 0;
  for(action_num=0; action_num < NUM_ACTIONS; action_num++) {
    if(actions[action_num].valid) {
      valid_action_cnt++;
      if( valid_action_cnt > gnum_actions )
	break;
      print_action_prob(actions[action_num], prob_file);
    }
    
  }
  fprintf(prob_file, ")\n");

}


void print_pddl_det(prob_action_type* actions, FILE *det_file, FILE *at_file) 
{
  int i, j, peff_num, param_num, action_num, valid_action_cnt;
  
  /* start printing out - deterministic domain */
  fprintf(det_file, "(define (domain prob_domain) \n");
  /* no typing nor conditional effects */
  fprintf(det_file, " (:requirements :strips) \n");
  /*printf(" (:types pos_x pos_y) \n");*/
  /* printf(" (:constants X1 X2 X3 X4 - pos_x Y1 Y2 Y3 Y4 - pos_y) \n"); */
  fprintf(det_file, " (:predicates \n");
  for(i=0; i< gnum_predicates; i++) {
    fprintf(det_file, "\t (%s ", predicates[i]);
    for(j=0; j< predicates_arity_info[i]; j++) 
      fprintf(det_file, "%s ", params[j]);
    fprintf(det_file, ") \n");
  }
  /* add bogus-goal */
  fprintf(det_file, "\t (bogus-goal)\n");
  fprintf(det_file, " )\n");

  fprintf(at_file, "%d\n", gnum_actions);

  valid_action_cnt = 0;
  for(action_num=0; action_num < NUM_ACTIONS; action_num++) {
    if(actions[action_num].valid) {
      valid_action_cnt++;
      if(valid_action_cnt > gnum_actions)
	break;
      
      for(peff_num=0; peff_num < actions[action_num].num_probeffs ; peff_num++) {
	fprintf(det_file, "(:action %s%d\n", actions[action_num].name, peff_num);

	fprintf(det_file, " :parameters (");
	for(param_num =0; param_num < actions[action_num].arity; param_num++)
	  fprintf(det_file, "%s ", params[actions[action_num].parameters[param_num]]);
	fprintf(det_file, ")\n");

	fprintf(det_file, " :precondition (and \n");
	for(i=0; i< actions[action_num].num_preconds; i++) {
	  fprintf(det_file, "\t\t ");
	  print_lifted_predicate(actions[action_num].precondition.and_preconditions[i], det_file);
	  fprintf(det_file, "\t\t ");
	  fprintf(det_file, " \n");
	}
	fprintf(det_file, "  )\n");

	fprintf(det_file, " :effect (and ");
	for(i=0; i< actions[action_num].prob_effects[peff_num].num_effs; i++) {
	  print_lifted_predicate(actions[action_num].prob_effects[peff_num].and_effects[i], det_file);
	}
	fprintf(det_file, "          )\n");
	fprintf(det_file, " )\n");
      }
    
      fprintf(at_file, "%s %d ", actions[action_num].name, actions[action_num].num_probeffs);
      for(peff_num=0; peff_num<actions[action_num].num_probeffs; peff_num++) {
	fprintf(at_file, "%s%d %d ",
		actions[action_num].name, peff_num,
		actions[action_num].prob_effects[peff_num].prob);
      }
      fprintf(at_file, "\n");
    
    }
  }
  
  /* add bogus action */
  fprintf(det_file, "(:action bogus \n :effect (bogus-goal))\n");
  fprintf(det_file, ")\n");

}

void print_a_problem(FILE *problem_file) 
{
  int i, j, object_num, fact_num, temp_pred_num, temp_object_num;
  int go_on, go_on_cnt;
  
  fprintf(problem_file, "(define (problem a_problem) \n");
  fprintf(problem_file, " (:domain prob_domain) \n");


  /* create objects */
  objects = (char **)malloc(sizeof(char *) * gnum_objects);
  for( i=0; i < gnum_objects; i++ ) {
    objects[i] = (char *) malloc (sizeof(char) * LEN_OBJECT_NAME);
  }

  for(i = 0; i < gnum_objects; i++) {
    objects[i][LEN_OBJECT_NAME -1] = '\0';
  }

  for( i = 0; i< gnum_objects; i++ ) {
    go_on = 1;
    go_on_cnt=0;
    while(go_on) {
      
      for( j=0; j< LEN_OBJECT_NAME -1; j++) {
	objects[i][j] = alphabet[rand() % 26];
      }

      for(j=0; j< i; j++)
	if(!strcmp(objects[i],objects[j]))
	  break;

      if(j == i)
	go_on = 0;
    }
    
  }


  fprintf(problem_file, " (:objects ");
  for(object_num=0; object_num < gnum_objects; object_num++) 
    fprintf(problem_file, "%s ", objects[object_num]);
  fprintf(problem_file, ")\n");

  

  init_predicates = (grounded_predicate_type *)malloc(sizeof(grounded_predicate_type) *
						      gnum_init_facts);
  for(fact_num = 0; fact_num < gnum_init_facts; fact_num++) {
    go_on = 1;
    go_on_cnt = 0;
    while(go_on) {
      go_on_cnt++;
      if(go_on_cnt > 10000) {
	printf("creating problem problem \n");
	exit(2);
      }

      init_predicates[fact_num].predicate_num = temp_pred_num = rand() % gnum_predicates;
      init_predicates[fact_num].negated = 0;
      for(i=0; i<predicates_arity_info[temp_pred_num];i++) {
	init_predicates[fact_num].args[i] = temp_object_num = rand() % gnum_objects;
      }

      for(j=0; j< fact_num; j++)
	if(same_grounded_predicate(init_predicates[j], init_predicates[fact_num]))
	  break;

      if(j == fact_num)
	go_on = 0;
    }
  }

  fprintf(problem_file, " (:init \n");
  
  for(fact_num = 0; fact_num < gnum_init_facts; fact_num++) {
    print_grounded_predicate(init_predicates[fact_num], problem_file);
    fprintf(problem_file, "\n");
  }
  fprintf(problem_file, " \n)\n");
  
  fprintf(problem_file, " (:goal (and (bogus-goal)))) \n");
  
}

void print_b_problem(FILE *b_problem_file) 
{
  int i, object_num, fact_num;
  
  fprintf(b_problem_file, "(define (problem b_problem) \n");
  fprintf(b_problem_file, " (:domain prob_domain) \n");
  fprintf(b_problem_file, " (:objects ");
  for(object_num=0; object_num < gnum_objects; object_num++) 
    fprintf(b_problem_file, "%s ", objects[object_num]);
  fprintf(b_problem_file, ")\n");
  fprintf(b_problem_file, " (:init \n");
  for(fact_num = 0; fact_num < gnum_init_facts; fact_num++) {
    print_grounded_predicate(init_predicates[fact_num], b_problem_file);
  }

  fprintf(b_problem_file, " \n)\n");

}


int process_command_line( int argc, char *argv[] )

{

  char option;

  gcmd_line.display_info = 1;
  gcmd_line.debug = 0;
  
  while ( --argc && ++argv ) {
    if ( *argv[0] != '-' || strlen(*argv) != 2 ) {
      return 0;
    }
    option = *++argv[0];
    switch ( option ) {
    default:
      if ( --argc && ++argv ) {
	switch ( option ) {
	  /*case 'p':
	  strncpy( gcmd_line.path, *argv, MAX_LENGTH );
	  break;
	case 'o':
	  strncpy( gcmd_line.ops_file_name, *argv, MAX_LENGTH );
	  break;
	case 'f':
	  strncpy( gcmd_line.fct_file_name, *argv, MAX_LENGTH );
	  break;
	  */
	case 'r':
	  sscanf( *argv, "%d", &grandom_walk_length );
	  break;
	case 't':
	  sscanf( *argv, "%d", &gmax_arity);
	  break;
	case 'c':
	  sscanf( *argv, "%d", &gmax_action_arity);
	  break;
	case 'p':
	  sscanf( *argv, "%d", &gnum_prob_effects);
	  break;
	case 'e':
	  sscanf( *argv, "%d", &gnum_effects);
	  break;
	case 'd':
	  sscanf( *argv, "%d", &gnum_preconditions);
	  break;
	case 'i':
	  sscanf( *argv, "%d", &gnum_init_facts);
	  break;
	case 'a':
	  sscanf( *argv, "%d", &gnum_actions);
	  break;
	case 's':
	  sscanf( *argv, "%d", &gnum_predicates);
	  break;
	case 'n':
	  sscanf( *argv, "%d", &gnum_prob_negated_preconds);
	  break;
	case 'm':
	  sscanf( *argv, "%d", &gnum_prob_negated_effects);
	  break;
	case 'h':
	  sscanf( *argv, "%f", &g_path_prob_limit);
	  break;
	  
	default:
	  printf( "\nrgen: unknown option: %c entered\n\n", option );
	  return 0;
	}
      } else {
	return 0;
      }
    }
  }

  return 1;

}


void create_a_problem() 
{
  int i, j, object_num, fact_num, temp_pred_num, temp_object_num;
  int go_on, go_on_cnt;

  /* create objects */
  objects = (char **)malloc(sizeof(char *) * gnum_objects);
  for( i=0; i < gnum_objects; i++ ) {
    objects[i] = (char *) malloc (sizeof(char) * LEN_OBJECT_NAME);
  }

  for(i = 0; i < gnum_objects; i++) {
    objects[i][LEN_OBJECT_NAME -1] = '\0';
  }

  for( i = 0; i< gnum_objects; i++ ) {
    go_on = 1;
    go_on_cnt=0;
    while(go_on) {
      
      for( j=0; j< LEN_OBJECT_NAME -1; j++) {
	objects[i][j] = alphabet[rand() % 26];
      }

      for(j=0; j< i; j++)
	if(!strcmp(objects[i],objects[j]))
	  break;

      if(j == i)
	go_on = 0;
    }
    
  }


  init_predicates = (grounded_predicate_type *)malloc(sizeof(grounded_predicate_type) *
						      gnum_init_facts);

  for(fact_num = 0; fact_num < gnum_init_facts; fact_num++) {
    go_on = 1;
    go_on_cnt = 0;
    while(go_on) {
      go_on_cnt++;
      if(go_on_cnt > 10000) {
	printf("creating problem problem \n");
	exit(2);
      }

      init_predicates[fact_num].predicate_num = temp_pred_num = rand() % gnum_predicates;
      init_predicates[fact_num].negated = 0;
      for(i=0; i<predicates_arity_info[temp_pred_num];i++) {
	init_predicates[fact_num].args[i] = temp_object_num = rand() % gnum_objects;
      }

      for(j=0; j< fact_num; j++)
	if(same_grounded_predicate(init_predicates[j], init_predicates[fact_num]))
	  break;

      if(j == fact_num)
	go_on = 0;
    }
  }
}

void print_reset_domain_prob (prob_action_type* actions, FILE *prob_file) 
{
  int i, j, peff_num, param_num, action_num, valid_action_cnt, object_num;
  
  fprintf(prob_file, "(define (domain prob_domain) \n");
  /* no typing nor conditional effects */
  fprintf(prob_file, " (:requirements :strips :probabilistic-effects :conditional-effects) \n");

  /* print objects constants */
  fprintf(prob_file, " (:constants ");
  for(object_num=0; object_num < gnum_objects; object_num++) 
    fprintf(prob_file, "%s ", objects[object_num]);
  fprintf(prob_file, ")\n");

  /*printf(" (:types pos_x pos_y) \n");*/
  /* printf(" (:constants X1 X2 X3 X4 - pos_x Y1 Y2 Y3 Y4 - pos_y) \n"); */
  fprintf(prob_file, " (:predicates \n");
  for(i=0; i< gnum_predicates; i++) {
    fprintf(prob_file, "\t (%s ", predicates[i]);
    for(j=0; j< predicates_arity_info[i]; j++) 
      fprintf(prob_file, "%s ", params[j]);
    fprintf(prob_file, ") \n");
  }
  fprintf(prob_file, "(clear)\n");
  fprintf(prob_file, "(not-clear)\n");
  fprintf(prob_file, " )\n");

  valid_action_cnt = 0;
  for(action_num=0; action_num < NUM_ACTIONS; action_num++) {
    if(actions[action_num].valid) {
      valid_action_cnt++;
      if( valid_action_cnt > gnum_actions )
	break;
      print_action_prob(actions[action_num], prob_file);
    }
    
  }

  fprintf(prob_file,"(:action reset1 \n");
  fprintf(prob_file," :precondition (not-clear)\n");
  fprintf(prob_file," :effect (and \n");
  fprintf(prob_file,"	     (forall (?x) (and \n");
  for(i=0; i<gnum_predicates;i++) {
    if(predicates_arity_info[i] == 1)
      fprintf(prob_file, "      (not (%s ?x)) \n", predicates[i]);
    if(predicates_arity_info[i] == 2)
      for(j=0; j< gnum_objects; j++)
	fprintf(prob_file, "      (not (%s ?x %s)) \n", predicates[i], objects[j]);
  }
  fprintf(prob_file, "))\n");
  fprintf(prob_file,"(not (not-clear))\n");
  fprintf(prob_file,"(clear)))\n\n");

  fprintf(prob_file, "(:action reset2 \n");
  fprintf(prob_file, " :precondition (clear) \n");
  fprintf(prob_file, " :effect (and (not-clear)\n");
  fprintf(prob_file, "              (not (clear))\n");
  for(i = 0; i < gnum_init_facts; i++) {
    print_grounded_predicate(init_predicates[i], prob_file);
    fprintf(prob_file, "\n");
  }
  fprintf(prob_file, ")))\n");
  
}

void print_b_problem_reset (FILE *b_problem_file) 
{
  int i, object_num, fact_num;
  
  fprintf(b_problem_file, "(define (problem random-problem%d) \n", rand() % 1000);
  fprintf(b_problem_file, " (:domain prob_domain) \n");
  /* no objects needed, constants define them */
  fprintf(b_problem_file, " (:init \n");
  fprintf(b_problem_file, "(not-clear)\n");
  for(fact_num = 0; fact_num < gnum_init_facts; fact_num++) {
    print_grounded_predicate(init_predicates[fact_num], b_problem_file);
  }

  fprintf(b_problem_file, " \n)\n");

}



int main(int argc, char *argv[]) 
{

  int static first_time = 1;
  int i,j,k, go_on;
  char predicate[3];
  int num_predicates;
  int num_preconditions;
  int num_effects; /* one delete one add */
  int max_preconditions = 3;
  int precondition_storage = 3; /* predicate var1 var2 */
  int prob_effect_storage = 6; /* delete_predicate var 1 var2 add_predicate var1 var 2 */
  int num_prob_effects = 2;
  

  int **preconditions;
  lifted_predicate_type used_preconditions[MAX_NUM_PRECONDITIONS];
  prob_effect_type used_prob_effects[MAX_NUM_PROB_EFFECTS];
  prob_action_type actions[NUM_ACTIONS];
  
  int action_num;
  int precondition_num, p_num;
  int prob_effect_num;
  int peff_num;
  int precond_num;
  int param_num;
  int object_num;
  int fact_num;

  int cum_prob;

  FILE *det_file = fopen("det_domain", "w");
  FILE *prob_file = fopen("prob_domain", "w");
  FILE *at_file = fopen("action_table", "w");
  FILE *problem_file = fopen("a-problem", "w");
  FILE *b_problem_file = fopen("b-problem", "w");
  FILE *prob_file_reset = fopen("prob_domain_reset", "w");
  FILE *b_problem_reset = fopen("b-problem_reset", "w");


  int temp_pred_num, cur_action_arity, cur_num_effs, temp_object_num, temp_neg_num;
  int andeff_num;
  int valid_action_cnt;

  
    
  printf("starting \n");

  if(!process_command_line(argc, argv))
    exit(3);

  /* generate predicates */
  num_predicates = gnum_predicates;
  num_preconditions = 3;

  predicates = (char **)malloc(sizeof(char *) * num_predicates);
  predicates_arity_info = (int *)malloc(sizeof(int) * num_predicates);
  
  for( i=0; i < num_predicates; i++ ) {
    predicates[i] = (char *) malloc (sizeof(char) * len_predicate);
  }

  if( first_time ){
    first_time = 0;
    srand( (unsigned int)time( NULL ) );
    predicate[2] = '\0';
    for(i = 0; i < num_predicates; i++) {
      predicates[i][len_predicate -1] = '\0';
    }
  }

  for( i = 0; i< num_predicates; i++ ) {
    go_on = 1;

    while(go_on) {
      for( j=0; j< len_predicate -1; j++) {
	predicates[i][j] = alphabet[rand() % 26];
      }
      predicates_arity_info[i] = 1 + rand() % gmax_arity;

      for(j=0; j< i; j++)
	if(!strcmp(predicates[i],predicates[j]))
	  break;

      if(j == i)
	go_on = 0;
    }
    
  }

  /* print domain definitions */

  /* print action definitions */
  for( action_num=0; action_num < NUM_ACTIONS ; action_num++) {

    /* hope this action is valid */
    actions[action_num].valid = 1;
    

    /* action name */
    for( i=0; i < LEN_ACTION_NAME -1; i++)
      actions[action_num].name[i] = alphabet[rand() % 26];
    actions[action_num].name[LEN_ACTION_NAME-1] = '\0';


    cur_action_arity = actions[action_num].arity = 1 + rand() % gmax_action_arity;

    /* action parameters */
    for( param_num =0; param_num < actions[action_num].arity; param_num++)
      actions[action_num].parameters[param_num] = param_num;


    /* action preconditions */
    actions[action_num].num_preconds = 1+ rand() % gnum_preconditions;

    for(p_num=0; p_num< actions[action_num].num_preconds; p_num++) {

      /* first decide negation */
      temp_neg_num = rand() % 100;

      if( temp_neg_num  < gnum_prob_negated_preconds )
	actions[action_num].precondition.and_preconditions[p_num].negated = 1;
      else
	actions[action_num].precondition.and_preconditions[p_num].negated = 0;
      
      temp_pred_num =
	actions[action_num].precondition.and_preconditions[p_num].predicate_num =
	rand() % num_predicates;
      for(i=0; i < predicates_arity_info[temp_pred_num]; i++) {
	actions[action_num].precondition.and_preconditions[p_num].args[i] =
	  rand() % actions[action_num].arity;
      }
      
    }


    /* action effects */

    actions[action_num].num_probeffs = 1 + (rand() % gnum_prob_effects);
      
    for(peff_num=0; peff_num < actions[action_num].num_probeffs; peff_num++) {

      /* first probability of this effect */
      cum_prob = 0;
      for(i=0; i< peff_num; i++)
	cum_prob += actions[action_num].prob_effects[i].prob;

      if( peff_num == actions[action_num].num_probeffs -1)
	actions[action_num].prob_effects[peff_num].prob = 100 - cum_prob;
      else
	actions[action_num].prob_effects[peff_num].prob = rand() % (100 - cum_prob);
      /*printf("%d %d\n", cum_prob, actions[action_num].prob_effects[peff_num].prob);*/

      /* select number of effects */
      cur_num_effs = actions[action_num].prob_effects[peff_num].num_effs
	= 1+ rand() % gnum_effects;

 
      /* select effects, considering deletions */
      for(andeff_num=0; andeff_num< cur_num_effs; andeff_num++) {

	/* two cases deletions and addtions */
	temp_neg_num = rand() % 100;
	if( temp_neg_num < gnum_prob_negated_effects ) {
	  
	  actions[action_num].prob_effects[peff_num].and_effects[andeff_num].negated = 1;

	  precond_num = rand() % actions[action_num].num_preconds;

	  if( actions[action_num].precondition.and_preconditions[precond_num].negated ){
	    /*sorry I don't handle this */
	    actions[action_num].valid = 0;
	  }
	      
	  temp_pred_num =
	    actions[action_num].prob_effects[peff_num].and_effects[andeff_num].predicate_num =
	    actions[action_num].precondition.and_preconditions[precond_num].predicate_num;

	  for(i=0; i< predicates_arity_info[temp_pred_num]; i++)
	    actions[action_num].prob_effects[peff_num].and_effects[andeff_num].args[i] =
	      actions[action_num].precondition.and_preconditions[precond_num].args[i];
	} else {

	  actions[action_num].prob_effects[peff_num].and_effects[andeff_num].negated = 0;
	  
	  temp_pred_num =
	    actions[action_num].prob_effects[peff_num].and_effects[andeff_num].predicate_num =
	    rand() % num_predicates;

	  for(i=0;i< predicates_arity_info[temp_pred_num];i++)
	    actions[action_num].prob_effects[peff_num].and_effects[andeff_num].args[i] =
	      rand() % actions[action_num].arity;
	  
	}
	
      }
	
    }

  }

  /*
  for(i=0; i< NUM_ACTIONS; i++)
    printf("%d", actions[i].valid);
  printf("\n");
  */
  
  /* let's check off actions */
  for(action_num =0; action_num < NUM_ACTIONS; action_num++) {
    if(!actions[action_num].valid)
      continue;
    /* parameters */
    if(!action_arity_usage_check(actions[action_num])) {
      actions[action_num].valid = 0;
      continue;
    }
    /* precondition*/
    if(!lifted_predicates_consistency_check(actions[action_num].precondition.and_preconditions,
					    actions[action_num].num_preconds)) {
      actions[action_num].valid = 0;
      continue;
    }

    /* effect */

    /* consistency check for each prob effect sets */
    for(i=0; i<actions[action_num].num_probeffs; i++)
      if(!lifted_predicates_consistency_check(actions[action_num].prob_effects[i].and_effects,
					      actions[action_num].prob_effects[i].num_effs))
	break;
    if( i < actions[action_num].num_probeffs ) {
      actions[action_num].valid = 0;
      continue;
    }

    /* useless addition check */
    if( !useless_addition_check(actions[action_num]) ) {
      actions[action_num].valid = 0;
      continue;
    }
      
    /* effects duplicate check */
    for(i=0; i<actions[action_num].num_probeffs; i++) {
      for(j=0; j< i; j++)
	if(same_effects(actions[action_num].prob_effects[j],
			actions[action_num].prob_effects[i]))
	  break;

      if(j < i)
	break;
    }
    if( i < actions[action_num].num_probeffs ) {
      actions[action_num].valid = 0;
      continue;
    }


    /* action name duplicate check */
    for(i=0; i< action_num; i++) 
      if(actions[i].valid && !strcmp(actions[i].name,actions[action_num].name))
	break;
    if( i< action_num) {
      actions[action_num].valid = 0;
      continue;
    }

    /* finally action duplicate check */
    for(i=0; i< action_num; i++) 
      if(actions[i].valid && same_action(actions[i], actions[action_num]))
	break;
    if( i< action_num) {
      actions[action_num].valid = 0;
      continue;
    }
  }
  

  gnum_ractions =0;
  for(i=0; i< NUM_ACTIONS; i++) {
    /*printf("%d", actions[i].valid);*/
    if( actions[i].valid )
      gnum_ractions++;
  }
  

  valid_action_cnt = 0;
  for(i=0; i< NUM_ACTIONS; i++)
    if(actions[i].valid) {
      valid_action_cnt++;
      if(valid_action_cnt > gnum_actions)
	break;
      /*printf("\n%d\n", i);
	print_action_prob(actions[i], stdout);*/
    }

  create_a_problem();
  print_pddl_prob(actions, prob_file );
  print_pddl_det(actions, det_file, at_file);
  print_a_problem(problem_file);
  /* print_pddl_prob(actions, b_problem_file ); */
  print_b_problem(b_problem_file);
  print_reset_domain_prob (actions, b_problem_reset);
  print_b_problem_reset (b_problem_reset);
    
  /* closing files */
  fclose(det_file);
  fclose(prob_file);
  fclose(at_file);
  fclose(problem_file);
  fclose(b_problem_file);
  fclose(b_problem_reset);

  /* run FF and get a problem  */
  {
    char *com = malloc(sizeof(char) * 1000);
    char *rwl = malloc(sizeof(char) * 100);
    char *rprob = malloc(sizeof(char) * 100);

    strcpy(com, "ffmod -o det_domain -f a-problem -l ");
    sprintf(rwl, "%d", grandom_walk_length);
    sprintf(rprob, " -t %f", g_path_prob_limit);
    strcat(com, rwl);
    strcat(com,rprob);

    printf("%s\n", com);
    
    system(com);
    
  }

}
 
