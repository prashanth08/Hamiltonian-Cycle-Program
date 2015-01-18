


#ifndef _STATS_H_
#define _STATS_H_

/************************************************************/
/* stats data structures:  
 * basic stat: average + stddev
 * stat_pertrial: stats for each trial
 * stat_pergraph: stats for each graph
 * stat_exp: stats for entire experiment
 */
/************************************************************/

/* maximum # of trials that can be recorded */
#define MAXTRIALTESTS 10
#define MAXGRAPHTESTS 10000


typedef struct {
  float ave;
  float stddev;
  } stat_type;

typedef struct {
  int result;  /* = HC_FOUND, HC_NOT_FOUND, HC_NOT_EXIST */
  float time;
  int nodes;
  int edgeprune;
  int initprune;
  int retries;
  } stat_pertrial_type;

typedef struct {
  stat_pertrial_type trial[MAXTRIALTESTS];
  int graphham;    /* = HC_FOUND, HC_NOT_FOUND, HC_NOT_EXIST */
  int biconnected;	/* = 1 if biconnected, = 0 if not */
  int mindeg2;		/* = 1 if min degree >= 2, = 0 if not */
  } stat_pergraph_type;

typedef struct {
  stat_pergraph_type graph[MAXGRAPHTESTS]; 

  /* node statistics */
  stat_type nodes_tot;
  stat_type nodes_ham;
  stat_type nodes_noham;
  stat_type nodes_nofound;

  stat_type noderatio_tot;
  stat_type noderatio_ham;
  stat_type noderatio_noham;
  stat_type noderatio_nofound;

  /* heuristic algorithm statistics */
  stat_type algsuccess;
  stat_type tmsuccess;
  stat_type tmfail;
  stat_type tmexpect;
  
  /* backtrack algorithm statistics */
  stat_type tmham;
  stat_type tmnoham;
  stat_type tmtotal;
  stat_type perham;
  stat_type perbiconnect;
  stat_type permindeg2;

  } stat_exp_type;


/************************************************************/
/* function declarations */
/************************************************************/

float calc_list_mean( int list[], int num);

void calc_list_mean_stddev( int list[], int num, float *mean, float *stddev);

void calc_flist_stats( float list[], int num, float *mean, float *stddev,
	float *min, float *max);

void calc_exp_stats( stat_exp_type *stats);

void init_exp_stats( stat_exp_type *stats);

#endif /* _STATS_H_ */

