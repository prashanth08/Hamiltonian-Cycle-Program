

#ifndef _BACKTRACK_H_
#define _BACKTRACK_H_


/************************************************************/
/* defines and data structures */
/************************************************************/

/* degsortflag parameters */
#define DEGSORT_RAND 0
#define DEGSORT_MIN 1
#define DEGSORT_MAX 2

/* pruneoptflag parameters: are the same as the HC_PRUNE_X parameters */

/* initvertflag parameter: see INITVERT_X parameters for select_initvert() */

/* options structure */
typedef struct {
  int initvertflag;
  int degsortflag;
  int pruneoptflag;
  int restart_increment;
  int max_nodes;
}  backtrack_alg_options_type;


/************************************************************/
/* function declarations */
/************************************************************/


int master_backtrack_alg( graph_type *graph, stat_pertrial_type *trialstats,
	int solution[]);

int parse_backtrack_alg_options( FILE *fp, char word[]);

void init_backtrack_alg_options();

void print_backtrack_alg_options( FILE *fp);


#endif /* _BACKTRACK_H_ */

