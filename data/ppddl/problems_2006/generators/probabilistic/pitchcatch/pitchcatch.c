
/* Created by Robert Givan and Sungwook Yoon */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_BITS 100

int gnum_bit_types = 5;
int gnum_ball_types = 5;
int gnum_catchable_arity;

char *difficulty_strings[] = {"Hard", "Medium", "Easy"};


void print_domain() 
{
  int i,j;
  float cum, cur;
  
  printf("(define (domain pitchcatch)\n");
  printf("(:requirements :adl :probabilistic-effects)\n");
  printf("(:types balltype bit difficulty)\n");

  /* constants */
  printf("(:constants  ");
  for(i=0; i< gnum_ball_types; i++)
    printf("T%d ", i);
  printf("- balltype ");
  printf("Easy Medium Hard - difficulty ");
  for(i=0; i< gnum_bit_types; i++)
    printf("B%d ", i);
  printf("- bit)\n");

  
  printf("(:predicates\n");
  printf("  (alive)\n");
  printf("  (ball-in-the-air-of-type ?t - balltype)\n");
  printf("  (catchable ?t - balltype ");
  /* catchable arity */
  for(i=0; i< gnum_catchable_arity; i++)
    printf("?B%d ",i);
  printf("- bit)\n");
  printf("  (bit-on ?b - bit)\n");
  printf("  (bit-off ?b - bit)\n");
  printf("  (thrown)\n");
  printf("  (not-thrown)\n");
  printf("  (caught ?t - balltype)\n");
  printf("  (deposited ?t - balltype)\n");
  printf("  (similar ?t1 ?t2 - balltype)\n");
  printf("  (difficulty-setting ?b - bit ?d - difficulty)\n");
  printf(")\n\n");


  /* actions */
  printf("(:action catch\n");
  printf(" :parameters (?T ?Tball - balltype ");
  for(i=0; i< gnum_catchable_arity; i++)
      printf("?B%d ", i);
  printf("- bit)\n");

  printf(" :precondition (and (thrown)\n");
  printf("                    (ball-in-the-air-of-type ?Tball)\n");
  printf("                    (catchable ?T ");
  for(i=0; i< gnum_catchable_arity; i++)
    printf("?B%d ", i);
  printf("))\n");
  
  printf(" :effect (and (not (thrown))\n");
  printf("              (not-thrown)\n");

  /*printf("              (forall (?t1 - balltype) (not (ball-in-the-air-of-type ?t1)))\n");*/
  
  for(i=0; i< gnum_catchable_arity; i++) {
    printf("              (not (bit-on ?B%d))\n", i);
    printf("              (bit-off ?B%d)\n", i);
  }
  printf("              (probabilistic 4/10\n");
  printf("              (when (and \n");
  for(i=0; i< gnum_catchable_arity; i++)
    printf("                          (bit-on ?B%d)\n",i);
  printf("                          (= ?T ?Tball))\n");
  printf("                          (caught ?Tball)))\n");
  printf("              (probabilistic 1/10\n");
  printf("              (when (and \n");
  for(i=0; i< gnum_catchable_arity; i++)
    printf("                          (bit-on ?B%d)\n",i);
  printf("                          (similar ?T ?Tball))\n");
  printf("                          (caught ?Tball)))))\n");

  printf("(:action deposit-ball\n");
  printf(" :effect      (and (forall (?t1 - balltype) (not (ball-in-the-air-of-type ?t1)))\n");
  printf("                   (forall (?t1 - balltype) (when (caught ?t1) (deposited ?t1)))))\n");
  
  printf("(:action pass\n");
  printf(" :precondition (thrown)\n");
  printf(" :effect (and (not (thrown))\n");
  printf("              (not-thrown)\n");
  printf("              (forall (?t - balltype) (not (ball-in-the-air-of-type ?t)))\n");
  printf("              (probabilistic 5/100 (not (alive)))))\n");  

  printf("(:action pitch\n");
  printf(" :precondition (and (not-thrown)\n");
  printf("                    (alive))\n");
  printf(" :effect  (and (thrown)\n");
  printf("               (not (not-thrown))\n");
  for(i=0; i < gnum_ball_types; i++)
    printf("               (probabilistic %d/100 (ball-in-the-air-of-type T%d))\n",
	   /*(float)(rand() % 60)/ (float)100,*/ (rand() % 50) + 10,
	   i);  
  /*
  printf("               (probabilistic \n");
  cum = 0.0;
  for(i=0; i< gnum_ball_types; i++) {
    cur =  1.0 / (float)gnum_ball_types -
          (1.0 / (float)gnum_ball_types) * (float) pow( 1.0/2.0, (double)(i+1));
    cum += cur;
    if( i == (gnum_ball_types-1) )
      printf("                         %f (ball-in-the-air-of-type T%d)\n", 1- cum + cur,i);
    else
      printf("                         %f (ball-in-the-air-of-type T%d)\n", cur ,i);
  }
  printf(")\n");
  */
  printf("               (forall (?B - bit)\n");
  printf("                       (probabilistic\n");
  printf("                           1/10 (and (bit-on ?B) (not (bit-off ?B)))))))\n");

  printf("(:action set-bit\n");
  printf(" :parameters (?B - bit)\n");
  printf(" :precondition (not-thrown)\n");
  printf(" :effect (and (bit-on ?B)\n");
  printf("              (not (bit-off ?B))\n");
  printf("              (probabilistic 5/100\n");
  printf("                    (when (difficulty-setting ?B Easy)\n");
  printf("                          (not (alive))))\n");
  printf("              (probabilistic 1/10 \n");
  printf("                    (when (difficulty-setting ?B Medium)\n");
  printf("                          (not (alive))))\n");
  printf("              (probabilistic 2/10\n");
  printf("                    (when (difficulty-setting ?B Hard)\n");
  printf("                          (not (alive))))))\n");
  
  printf(")\n");
}


void print_problem() 
{
  int i,j,k;
  int bitmemory[MAX_BITS];
  int cur_bit, go_on;
  
  printf("(define (problem a-pitchcatch-problem%d) \n", rand() % 1000);
  printf("(:domain pitchcatch)  \n");

  printf("(:init (alive) \n");
  printf("       (not-thrown)\n");
  for(i=0; i< gnum_bit_types; i++)
    printf("       (bit-on B%d)\n", i);
  for(i=0; i< gnum_bit_types; i++)
    printf("       (difficulty-setting B%d %s)\n", i,
	   difficulty_strings[rand() % 3]);
  
  /* need to check whether duplicate catchable exists */
  /* for example 1 case */
  for(i=0; i< gnum_ball_types; i++) {
    printf("       (catchable T%d ", i);
    for(j=0; j< gnum_catchable_arity; j++) {
      go_on = 1;
      while(go_on) {
	cur_bit = rand() % gnum_bit_types;
	for(k=0; k<j; k++) 
	  if (bitmemory[k] == cur_bit)
	    break;
	if(k == j)
	  go_on = 0;
      }
      printf("B%d ", cur_bit);
      bitmemory[j] = cur_bit;
    }
    printf(")\n");
  }

  for(i=0; i< gnum_ball_types; i++) {
    for(j=0; j < gnum_ball_types; j++) {
      if( j < i && !(rand() % 10)) {
	printf("       (similar T%d T%d)\n", i, j);
      	printf("       (similar T%d T%d)\n", j, i);
      }
      
    }
  }
  
  printf(")\n");
  printf("(:goal (and \n");
  for(i=0; i< gnum_ball_types; i++) 
    printf("            (deposited T%d)\n", i);
  printf(")))\n");
}

int process_command_line( int argc, char *argv[] )

{

  char option;
  
  while ( --argc && ++argv ) {
    if ( *argv[0] != '-' || strlen(*argv) != 2 ) {
      return 0;
    }
    option = *++argv[0];
    switch ( option ) {
    default:
      if ( --argc && ++argv ) {
	switch ( option ) {

	case 't':
	  sscanf( *argv, "%d", &gnum_ball_types );
	  break;
	case 'b':
	  sscanf( *argv, "%d", &gnum_bit_types);
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

int main(int argc, char *argv[]) 
{

  int i,j;

  /* randomize */
  srand( (unsigned int)time( NULL ) );

  /* read in command arguments */
  if(!process_command_line(argc, argv)) {
    printf("example: pitchcatch -t 5 \n");
    exit(1);
  }
  
  gnum_catchable_arity = (int)(log10((double)gnum_ball_types) / log10(2.0)) + 1;
  gnum_bit_types = gnum_catchable_arity * 2;
  
  print_domain();
  print_problem();

}
