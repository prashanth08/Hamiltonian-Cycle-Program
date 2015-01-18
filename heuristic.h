

#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_


/************************************************************/
/* defines and data structures */
/************************************************************/

/* defines for parameters for options structure */

/* visitflag parameters */
#define VISIT_SMART 0
#define VISIT_RAND 1

/* completeflag parameters (for converting from line to cycle, to 
 * complete the solution) */
#define COMPLETE_NORM 0
#define COMPLETE_SMART 1

/* edgepruneflag parameters:  how many edges to remove initially */
#define EDGEPRUNE_ALL 0
#define EDGEPRUNE_BASIC 1
#define EDGEPRUNE_NONE 2

/* cycleextend parameters */
#define NOCYCLEEXTEND 0
#define CYCLEEXTEND 1

/* options structure */
typedef struct {

  int visitflag;
  int completeflag;
  int cycleextendflag;

}  heuristic_alg_options_type;


/* defines for return values for hc_verify_solution */
#define HC_NOT_VERIFY 0
#define HC_VERIFY 1

/* defines for prunelevel in hc_init_pruning() */
#define INIT_PRUNE_ALL 0
#define INIT_PRUNE_3CYC 1

/************************************************************/
/* function declarations */
/************************************************************/


int master_heuristic_alg( graph_type *graph, stat_pertrial_type *trialstats,
	int solution[]);

int parse_heuristic_alg_options( FILE *fp, char word[]);

void init_heuristic_alg_options();

void print_heuristic_alg_options( FILE *fp);

#endif /* _HEURISTIC_H_ */

