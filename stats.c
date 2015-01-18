

#define IN_STATS_FILE

/* project includes */
#include "main.h"


/************************************************************/
/* basic statistical calculation functions
 *
 */
/************************************************************/

/************************************************************/
/* this function calculates the mean of a list */
float
calc_list_mean(
  int list[],
  int num)
{

  int loop;
  float ret;

  ret = 0.0;
  for (loop = 0; loop < num; loop++)
  {
    ret += list[loop];
  }

  ret /= num;

  return(ret);

}  /* end of calc_list_mean() */


/************************************************************/
/* this function calculates the stddev and mean of a list, and
 * returns both using the passed in pointers
 */
void
calc_list_mean_stddev(
  int list[],
  int num,
  float *mean,
  float *stddev)
{

  int loop;
  float sum1, sum2, dnum;
  float temp;

  sum1 = 0.0;
  sum2 = 0.0;
  dnum = (float) num;

  /* do the calculations */
  for (loop = 0; loop < num; loop++)
  {
    sum1 += (list[loop] * list[loop]);
    sum2 += list[loop];
  }

  temp = ( (sum1) - ((sum2/dnum) * sum2) ) / dnum;
  if (temp < 0.0)
    temp = 0.0;
  (*stddev) = (float) sqrt( (double) temp );

  *mean = sum2 / dnum;

}  /* end of calc_list_mean_stddev() */


/************************************************************/
/* this function calculates a bunch of statistics on a list of floats,
 * and returns these statistics using the passed-in pointers
 */
void
calc_flist_stats(
  float list[],
  int num,
  float *mean,
  float *stddev,
  float *min,
  float *max)
{

  int loop;
  float sum1, sum2, fnum;
  float temp;

  sum1 = 0.0;
  sum2 = 0.0;
  fnum = (float) num;
  *min = list[0];
  *max = list[0];

  /* do the calculations */
  for (loop = 0; loop < num; loop++)
  {
    sum1 += (list[loop] * list[loop]);
    sum2 += list[loop];

    if (list[loop] < *min)
      *min = list[loop];
 
    if (list[loop] > *max)
      *max = list[loop];
  }

  temp = ( (sum1) - ((sum2/fnum) * sum2) ) / fnum;
  if (temp < 0.0)
    temp = 0.0;
  (*stddev) = (float) sqrt( (double) temp );

  *mean = sum2 / fnum;

}  /* end of calc_flist_stats() */


/************************************************************/
/* this function calculates the statistics involving backtrack nodes
 
assumptions:
  # of vertices, # of graph tests, # of trials per graph stored in g_options

  stats->graph[graph_index].trial[trial_index].nodes contains number of nodes

stat statistics set:
  nodes_tot
  nodes_ham
  nodes_noham
  nodes_nofound
  plus corresponding noderatio statistics 

  note that nodes_tot includes ham and noham cases only
    
*/ 
void
calc_node_stats(
  stat_exp_type *stats)
{
  int graph_index, trial_index;

  int total_tests = g_options.num_graph_tests * g_options.num_instance_tests;

  /* want to count total nodes for ham, noham and nofound instances */
  int ham_count     = 0;
  int noham_count   = 0;
  int nofound_count = 0;
  int tot_count     = 0;

  float ham_list[total_tests+1];
  float noham_list[total_tests+1];
  float nofound_list[total_tests+1];
  float tot_list[total_tests+1];

  float min, max;	/* junk variables */
  
  for (graph_index = 0; graph_index < g_options.num_graph_tests; graph_index++)
  {
    for (trial_index = 0; trial_index < g_options.num_instance_tests; 
	trial_index ++)
    {
      if (stats->graph[graph_index].graphham == HC_FOUND)
      {
        ham_list[ham_count] = stats->graph[graph_index].trial[trial_index].nodes;
        ham_count++;
      }
      else if (stats->graph[graph_index].graphham == HC_NOT_EXIST)
      {
        noham_list[noham_count] = 
	  stats->graph[graph_index].trial[trial_index].nodes;
        noham_count++;
      }
      else
      {
        nofound_list[nofound_count] = 
	  stats->graph[graph_index].trial[trial_index].nodes;
        nofound_count++;
      }

      if (stats->graph[graph_index].graphham != HC_NOT_FOUND)
      {
        tot_list[tot_count] = 
          stats->graph[graph_index].trial[trial_index].nodes;
        tot_count++;
      }

    }  /* end of trial_index loop */
  }  /* end of graph_index loop */

  /* calculate averages, stddevs */
  calc_flist_stats(ham_list, ham_count, &(stats->nodes_ham.ave), 
 	&(stats->nodes_ham.stddev), &min, &max);
  calc_flist_stats(noham_list, noham_count, &(stats->nodes_noham.ave), 
 	&(stats->nodes_noham.stddev), &min, &max);
  calc_flist_stats(nofound_list, nofound_count, &(stats->nodes_nofound.ave), 
 	&(stats->nodes_nofound.stddev), &min, &max);
  calc_flist_stats(tot_list, tot_count, &(stats->nodes_tot.ave), 
        &(stats->nodes_tot.stddev), &min, &max);

  /* calculate node ratios */
  for (trial_index = 0; trial_index < total_tests; trial_index++)
  {
    if (trial_index < ham_count)
      ham_list[trial_index] /= ((float) g_options.graphgen.nvertex);

    if (trial_index < noham_count)
      noham_list[trial_index] /= ((float) g_options.graphgen.nvertex);

    if (trial_index < nofound_count)
      nofound_list[trial_index] /= ((float) g_options.graphgen.nvertex);

    if (trial_index < tot_count)
      tot_list[trial_index] /= ((float) g_options.graphgen.nvertex);
  }
    
  /* calcluate node ratio statistics */
  calc_flist_stats(ham_list, ham_count, &(stats->noderatio_ham.ave), 
 	&(stats->noderatio_ham.stddev), &min, &max);
  calc_flist_stats(noham_list, noham_count, &(stats->noderatio_noham.ave), 
 	&(stats->noderatio_noham.stddev), &min, &max);
  calc_flist_stats(nofound_list, nofound_count, &(stats->noderatio_nofound.ave), 
 	&(stats->noderatio_nofound.stddev), &min, &max);
  calc_flist_stats(tot_list, tot_count, &(stats->noderatio_tot.ave), 
        &(stats->noderatio_tot.stddev), &min, &max);

}  /* end of calc_node_stats() */
  

/************************************************************/
/* this function calculates the statistics after all the experiments are 
 * complete
 */
void
calc_exp_stats(
  stat_exp_type *stats)
{
  int gloop, tloop;
  int numtests = g_options.num_graph_tests * g_options.num_instance_tests;
  float max, min;
  float ratio;
  float varsuccess, varfail;

  /* general stats */
  int hamcount = 0;

  float tmsuccess[numtests];
  int tmsuccessnum = 0;
  float tmfail[numtests];
  int tmfailnum = 0;
  int successcount = 0;

  /* backtrack algorithm stats */

  float tmham[numtests];
  int tmhamnum = 0;
  float tmnoham[numtests];
  int tmnohamnum = 0;
  float tmtotal[numtests];
  int testcount = 0;

  int biconnect = 0;
  int mindeg2 = 0;

  int numhamgraphs = g_options.num_graph_tests;

  /* heuristic algorithm */
  if (g_options.algorithm == ALG_POSA_HEUR)
  {
    /* note that HC_NOT_EXIST results are completely ignored */
    for (gloop = 0; gloop < g_options.num_graph_tests; gloop++)
    {
      for (tloop = 0; tloop < g_options.num_instance_tests; tloop++)
      {
        if (stats->graph[gloop].trial[tloop].result == HC_FOUND)
        {
          successcount++;
          tmsuccess[tmsuccessnum] = stats->graph[gloop].trial[tloop].time;
          tmsuccessnum++;
        }
        else if (stats->graph[gloop].trial[tloop].result == HC_NOT_FOUND)
        {
          tmfail[tmfailnum] = stats->graph[gloop].trial[tloop].time;
          tmfailnum++;
        }
        else
          numtests--;
      }
      if (stats->graph[gloop].graphham == HC_FOUND)
        hamcount++;
    }
          
    /* calculate averages, stddevs */
    calc_flist_stats(tmsuccess, tmsuccessnum, &(stats->tmsuccess.ave), 
 	&(stats->tmsuccess.stddev), &min, &max);
    calc_flist_stats(tmfail, tmfailnum, &(stats->tmfail.ave), 
	&(stats->tmfail.stddev), &min, &max);

    /* calculate alg success rate */
    stats->algsuccess.ave = ( (float) successcount / (float) numtests );
    stats->algsuccess.stddev = (float) sqrt( (double) 
 	(numtests * stats->algsuccess.ave * (1 - stats->algsuccess.ave) ) );
 
    /* calculate expected time */
    ratio = (1 - stats->algsuccess.ave) / stats->algsuccess.ave;
    if (tmfailnum == 0)
      stats->tmexpect.ave = stats->tmsuccess.ave; 
    else if (tmsuccessnum == 0)
      stats->tmexpect.ave = stats->tmfail.ave; 
    else 
      stats->tmexpect.ave = stats->tmsuccess.ave + stats->tmfail.ave * ratio;
 
    varsuccess = stats->tmsuccess.stddev * stats->tmsuccess.stddev;
    varfail = stats->tmfail.stddev * stats->tmfail.stddev;
    if (tmfailnum == 0)
      stats->tmexpect.stddev = stats->tmsuccess.stddev;
    else if (tmsuccessnum == 0)
      stats->tmexpect.stddev = stats->tmfail.stddev;
    else
    {
      stats->tmexpect.stddev = (float) sqrt( (double) 
   		(varsuccess + varfail * ratio * ratio) );
    }
 
    /* calculate percent ham graphs rate */
    stats->perham.ave = ( (float) hamcount / (float)g_options.num_graph_tests);
    stats->perham.stddev = (float) sqrt( (double) 
   (g_options.num_graph_tests * stats->perham.ave * (1 - stats->perham.ave) ) );
 
  }  /* end of heuristic algorithm stats */

  /* backtrack algorithm */
  else if ( (g_options.algorithm == ALG_NOPRUNE_BT) ||
        (g_options.algorithm == ALG_BACKTRACK) )
  {
    /* only ignore HC_NOT_FOUND if graph is HC_NOT_FOUND */
    /* always include HC_NOT_FOUND results for tottime stats */
    /* ignore HC_NOT_EXIST for alg success times */
    for (gloop = 0; gloop < g_options.num_graph_tests; gloop++)
    {
      for (tloop = 0; tloop < g_options.num_instance_tests; tloop++)
      {
	if (stats->graph[gloop].trial[tloop].result == HC_FOUND)
        {
          successcount++;
          tmsuccess[tmsuccessnum] = stats->graph[gloop].trial[tloop].time;
          tmsuccessnum++;
        }
        else if (stats->graph[gloop].trial[tloop].result == HC_NOT_FOUND)
        {
          tmfail[tmfailnum] = stats->graph[gloop].trial[tloop].time;
          tmfailnum++;
        }
        else
          numtests--;

        if (stats->graph[gloop].graphham == HC_FOUND) 
        {
          tmham[tmhamnum] = stats->graph[gloop].trial[tloop].time;
          tmhamnum++;
        }
        else if (stats->graph[gloop].graphham == HC_NOT_EXIST)
        {
          tmnoham[tmnohamnum] = stats->graph[gloop].trial[tloop].time;
          tmnohamnum++;
        }
        tmtotal[testcount] = stats->graph[gloop].trial[tloop].time;
        testcount++;
      }  /* end of trial for loop */

      if (stats->graph[gloop].graphham == HC_FOUND)
        hamcount++;
      if (stats->graph[gloop].graphham == HC_NOT_FOUND)
        numhamgraphs--;

      if (stats->graph[gloop].biconnected == 1)
        biconnect ++;
      if (stats->graph[gloop].mindeg2 == 1)
        mindeg2 ++;

    }  /* end of graph for loop */

    /* calculate biconnect, mindeg2 */
    stats->perbiconnect.ave = 
	((float) biconnect / (float) g_options.num_graph_tests);
    stats->perbiconnect.stddev = 0;  /* not calculated */
    stats->permindeg2.ave = 
	((float) mindeg2 / (float) g_options.num_graph_tests);
    stats->permindeg2.stddev = 0;  /* not calculated */
          
    /* calculate averages, stddevs */
    calc_flist_stats(tmsuccess, tmsuccessnum, &(stats->tmsuccess.ave), 
 	&(stats->tmsuccess.stddev), &min, &max);
    calc_flist_stats(tmfail, tmfailnum, &(stats->tmfail.ave), 
	&(stats->tmfail.stddev), &min, &max);

    calc_flist_stats(tmham, tmhamnum, &(stats->tmham.ave), 
 	&(stats->tmham.stddev), &min, &max);
    calc_flist_stats(tmnoham, tmnohamnum, &(stats->tmnoham.ave), 
	&(stats->tmnoham.stddev), &min, &max);
    calc_flist_stats(tmtotal, testcount, &(stats->tmtotal.ave), 
	&(stats->tmtotal.stddev), &min, &max);

    /* calculate percent ham graphs rate */
    stats->perham.ave = ( (float) hamcount / (float) numhamgraphs );
    stats->perham.stddev = (float) sqrt( (double) 
   (numhamgraphs * stats->perham.ave * (1 - stats->perham.ave) ) );
 
    /* calculate alg success rate */
    stats->algsuccess.ave = ( (float)successcount / (float)numtests );

    /* if numtests == 0, then all results were HC_NOT_EXIST so set
     * algsuccess to 1.00 -> this is mostly a fix for knighttour results
     */
    if (numtests == 0)
      stats->algsuccess.ave = 1.0;

    stats->algsuccess.stddev = (float) sqrt( (double) 
 	(numtests * stats->algsuccess.ave * (1 - stats->algsuccess.ave) ) );
 
    /* calculate expected time */
    ratio = (1 - stats->algsuccess.ave) / stats->algsuccess.ave;
    if (tmfailnum == 0)
      stats->tmexpect.ave = stats->tmtotal.ave;  /* not success */
    else if (tmsuccessnum == 0)
      stats->tmexpect.ave = stats->tmfail.ave; 
    else 
      stats->tmexpect.ave = stats->tmsuccess.ave + stats->tmfail.ave * ratio;
 
    varsuccess = stats->tmsuccess.stddev * stats->tmsuccess.stddev;
    varfail = stats->tmfail.stddev * stats->tmfail.stddev;
    if (tmfailnum == 0)
      stats->tmexpect.stddev = stats->tmtotal.stddev;  /* not success */
    else if (tmsuccessnum == 0)
      stats->tmexpect.stddev = stats->tmfail.stddev;
    else
    {
      stats->tmexpect.stddev = (float) sqrt( (double) 
   		(varsuccess + varfail * ratio * ratio) );
    }
  }  /* end of backtrack algorithm stats */

  calc_node_stats(stats);

}  /* end of calc_exp_stats() */


/************************************************************/
/* this function initializes the list of statistics
 */
void
init_exp_stats(
  stat_exp_type *stats)
{
  int gloop, tloop;

  for (gloop = 0; gloop < g_options.num_graph_tests; gloop++)
  {
    for (tloop = 0; tloop < g_options.num_instance_tests; tloop++)
    {
      stats->graph[gloop].trial[tloop].result = HC_NOT_FOUND;
      stats->graph[gloop].trial[tloop].time = 0;
      stats->graph[gloop].trial[tloop].nodes = 0;
      stats->graph[gloop].trial[tloop].edgeprune = 0;
      stats->graph[gloop].trial[tloop].initprune = 0;
      stats->graph[gloop].trial[tloop].retries = 0;
    }
    stats->graph[gloop].graphham = HC_NOT_FOUND;
  }

}  /* end of init_exp_stats() */

