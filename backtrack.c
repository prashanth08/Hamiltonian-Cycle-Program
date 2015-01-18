

/* project includes */

#include "main.h"


/* local visitlist structure
 * necessary so visitlist can be sorted
 */
typedef struct {
  int vert;
  int deg;
  }  visitlist_type;

#define HIT_YES 1
#define HIT_NO  0

int g_hit_nodelimit = HIT_NO;

/************************************************************/
/* function to check node limit for backtrack HC algorithm
 * returns HC_QUIT if hit node limit
 * returns HC_CONTINUE otherwise
 */
int
bt_check_nodelimit(
  int cur_nodes)  /* current nodes searched */
{
  if (g_options.bt_alg.max_nodes == 0)
  {
    return(HC_CONTINUE);
  }

  if (cur_nodes >= g_options.bt_alg.max_nodes)
  {
    return(HC_QUIT);
  }
  else
  {
    return(HC_CONTINUE);
  }
 
}  /* end of bt_check_nodelimit() */
  

/************************************************************/
/* comparison function for visitlist data structure
 * (for sorting)
 * results returned depend on the degsortflag
 */
static int
compare_visitlist(
  void *vert1,
  void *vert2)
{
  visitlist_type *v1;
  visitlist_type *v2;

  v1 = (visitlist_type *) vert1;
  v2 = (visitlist_type *) vert2;

  if (g_options.bt_alg.degsortflag == DEGSORT_MIN)
  {
    /* sort by increasing degree */
    if (v1->deg < v2->deg)
      return (-1);
    else if (v1->deg > v2->deg)
      return (1);
    else
      return (0);
  }
  else
  { 
    /* sort by decreasing degree (so largest is first) */
    if (v1->deg > v2->deg)
      return (-1);
    else if (v1->deg < v2->deg)
      return (1);
    else
      return (0);
  }

}  /* end of compare_visitlist() */


/************************************************************/
/* this function takes a list (array) of vertices and sorts
 * them according to the degsortflag (used in the comparision function)
 *
 * note that preferably, vertices with equal degrees should be ordered
 * randomly.  qsort orders equal elements unpredictablely, which is good
 * enough
 */
void
sort_visitlist(
  graph_type *graph, 
  visitlist_type visitlist[], 
  int numvisit)
{
  int loop;

  /* set .deg field for visitlist array */
  for (loop = 0; loop < numvisit; loop++)
  {
    visitlist[loop].deg = graph->deg[visitlist[loop].vert];
  }

  /* call qsort() function */
  qsort(visitlist, numvisit, sizeof(visitlist_type), (void *)compare_visitlist);

}  /* end of sort_visitlist() */


/************************************************************/
/* backtrack algorithm to find hamiltonian cycle
 *
 * function returns HC_FOUND if HC was found, HC_NOT_EXIST if no HC
 *   was found (final return of HC_NOT_EXIST means no HC exists)
 *
 * parameters:
 *   graph -> graph to solve
 *   pstart, pend, plenth, path, graphpath -> specify the hamiltonian path 
 *     being constructed
 *   nodecount -> # of 'nodes' (vertices) expanded
 *      = # of calls to the recursive routine
 *
 * This is the NON-PRUNING VERSION
 */
int 
calc_noprune_bt_alg(
  graph_type *graph,
  int *pstart,
  int *pend,
  int *plength,
  path_type *path,
  graphpath_type *graphpath,
  int *nodecount)
{
  int loop;
  int oldpend;
  int newvert;
  
  (*nodecount)++;

  /* if have hamiltonian path, then try to convert into a cycle */
  if (*plength == graph->numvert) 
  {
    return(hc_path_to_cycle(graph, path, graphpath, pstart, pend, *plength) );
  }

  /* loop through neighbours of current endpoint, trying each in turn */
  for (loop = 0; loop < graph->deg[path[*pend].gvert]; loop++)
  {
    /* if neigbhour is not in path, then add to path and recurse */
    newvert = graph->nbr[path[*pend].gvert][loop];

    if (graphpath[newvert].pathpos == -1)
    {
      /* add current neighbour to path and recurse */
      oldpend = *pend;
      add_vert_to_path(path, graphpath, pstart, pend, plength, newvert);

      if (calc_noprune_bt_alg(graph, pstart, pend, plength, path, graphpath,
             nodecount) == HC_FOUND)
      {
        return(HC_FOUND);
      }
      
      /* current try was bad, so backup (remove current neighbour from path) */
      remove_endvert_from_path(path, graphpath, pstart, pend, plength, oldpend);
    }
  }  /* end of loop through neighbours of endpoint */

  return (HC_NOT_EXIST);

}  /* end of calc_noprune_bt_alg() */


/************************************************************/
/* pruning backtrack algorithm to find hamiltonian cycle
 *
 * function returns HC_FOUND if HC was found, HC_NOT_EXIST if no HC
 *   was found (final return of HC_NOT_EXIST means no HC exists)
 *
 * parameters:
 *   orggraph -> original graph.  used for hc_path_to_cycle
 *   graph -> graph to solve, which has edges pruned
 *   pstart, pend, plenth, path, graphpath -> specify the hamiltonian path 
 *     being constructed
 *   nodecount -> # of 'nodes' (vertices) expanded
 *      = # of calls to the recursive routine
 *
 * note: this algorithm might fail if it receives a non-simple graph
 *  i.e.: cannot have edges from a vertex $a$ back to vertex $a$
 *  i.e.: avoid duplicate edges 
 */
int 
calc_backtrack_alg(
  graph_type *orggraph,
  graph_type *graph,
  int *pstart,
  int *pend,
  int *plength,
  path_type *path,
  graphpath_type *graphpath,
  int *nodecount,
  int *prune,
  edgestack_type *edgestack)
{
  int loop, vloop;
  int oldpend;
  int newvert;

  int ret;
  int numprune;

  visitlist_type visitlist[MAXDEGREE];
  int numvisit;

  int numforced;
  int forcedvert = 0;

  int localstackpnt;

  if (g_hit_nodelimit == HIT_YES)
    return(HC_NOT_FOUND);

  /* increment node counter, check if reached limit */
  (*nodecount)++;
  if (bt_check_nodelimit(*nodecount) == HC_QUIT)
  {
    g_hit_nodelimit = HIT_YES;
    return(HC_NOT_FOUND);
  }

  /* if have hamiltonian path, then try to convert into a cycle */
  if (*plength == graph->numvert) 
  {
    return(hc_path_to_cycle(orggraph, path, graphpath, pstart, pend, *plength));
  }

  /* construct a prioritized visit list */
  numvisit = 0;

  /* count number of unvisited neighbors (of current endpoint) of degree 2 */
  for (numforced = 0, loop = 0; loop < graph->deg[path[*pend].gvert]; loop++)
  {
    newvert = graph->nbr[path[*pend].gvert][loop];
    if ( (graph->deg[newvert] == 2) && (graphpath[newvert].pathpos == -1) )
    {
      numforced++;
      forcedvert = newvert;
    }
  }

  /* if more than 1 forced edge (and not first vertex in path), 
   * then no HC possible 
   */
  if ( (numforced == 2) && (*plength != 1) )
  {
    return(HC_NOT_EXIST);
  }
    
  /* if 1 forced edge, then must take it */
  if (numforced == 1)
  {
    visitlist[numvisit++].vert = forcedvert;
  }
  else
  {
    /* load unvisited neighbors into visit list */
    for (loop = 0; loop < graph->deg[path[*pend].gvert]; loop++)
    {
      newvert = graph->nbr[path[*pend].gvert][loop];
      if (graphpath[newvert].pathpos == -1)
      {
        visitlist[numvisit++].vert = newvert;
      }
    }
    
    if (g_options.bt_alg.degsortflag != DEGSORT_RAND)
    {
      /* order unvisited neighbors by degree */
      sort_visitlist(graph, visitlist, numvisit);
    }

    /* if first vertex, then can skip last edge of visitlist, since
     * if haven't found cycle by then, then no cycle is possible
     */
    if (*plength == 1)
      numvisit--;
 
  }

  /* loop through visit list, trying each in turn */
  for (vloop = 0; vloop < numvisit; vloop++)
  {
    /* add vertex to path */
    oldpend = *pend;
    add_vert_to_path(path, graphpath, pstart, pend, plength, 
      visitlist[vloop].vert);

    /* save current position in edgestack */
    localstackpnt = get_curpnt_edgestack(edgestack);

    /* remove other edges from old end vertex to unvisited vertices 
     * don't do this to initial vertex (need edges to return)
     */
    numprune = 0;
    if (*plength > 2)
    {
      loop = 0;
      while (loop < graph->deg[path[oldpend].gvert])
      {
        newvert = graph->nbr[path[oldpend].gvert][loop];
        if (graphpath[newvert].pathpos == -1)  /* neighbour not in path */
        {
          rm_edge_graph(graph, path[oldpend].gvert, newvert);
          numprune++;
          push_edge_to_stack(path[oldpend].gvert, newvert, edgestack);
          /* don't increment loop variable since edge was deleted */
        }
        else 
          loop++;
      }
    }

    /* if vertices were removed, then try pruning 
     */
    if (numprune > 0)
    {
      *prune += numprune;
      ret = hc_do_pruning(graph, &numprune, g_options.bt_alg.pruneoptflag,
		edgestack);
      *prune += numprune;
    }
    else
      ret = HC_NOT_FOUND;

    /* recursively look for HC, if HC still possible */
    if (ret != HC_NOT_EXIST) 
    {
      if (calc_backtrack_alg(orggraph, graph, pstart, pend, plength, 
	    path, graphpath, nodecount, prune, edgestack) == HC_FOUND)
      {
        return(HC_FOUND);
      }
      if (g_hit_nodelimit == HIT_YES)
        return(HC_NOT_FOUND);
    }

    /* check if exceeded allowed time */
    if ( hc_check_timelimit() == HC_QUIT)
    {
      return(HC_NOT_FOUND);
    }

    /* undelete pruned edges */
    while (get_curpnt_edgestack(edgestack) != localstackpnt)
    {
      int v1, v2;
      pop_edge_from_stack(&v1, &v2, edgestack);

      add_edge_graph(graph, v1, v2);
    }

    /* current try was bad, so backup (remove vertex from path) */
    remove_endvert_from_path(path, graphpath, pstart, pend, plength, oldpend);

    /* we don't remove the edge we just tried to traverse because it will
     * be deleted anyways when we select the next edge
     */

  }  /* end of loop through visit list */

  /* check if exceeded allowed time */
  if ( hc_check_timelimit() == HC_QUIT)
  {
    return(HC_NOT_FOUND);
  }
  return (HC_NOT_EXIST);

}  /* end of calc_backtrack_alg() */


/************************************************************/
/* Hamilitonian Cycle Backtrack Algorithm Wrapper Function
 *   does initial degree checking/component checking using pruning
 *   runs backtrack algorithm, 
 *     returns solution (ordering of vertices making up the hamiltonian cycle)
 *     (if solution was found)
 *
 *   returns HC_FOUND if HC found, HC_NOT_EXIST if HC does not exist 
 *
 *  graph should NOT be modified (ensureham uses this function)
 */
int 
master_backtrack_alg(
  graph_type *graph,
  stat_pertrial_type *trialstats,
  int solution[])
{
  int ret;
  int loop;
  int nodecount = 0;
  int prune = 0;
  graph_type testgraph;

  edgestack_type edgestack;   /* for saving deleted edges */

  path_type path[MAXVERT];
  graphpath_type graphpath[MAXVERT];
  int pstart, pend, plength;
  int initvert;
  int tempnum;

  /* initialize variables */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    graphpath[loop].pathpos = -1;
    path[loop].gvert = -1;
    path[loop].next = -1;
  }

  init_edgestack(&edgestack);

  initvert = select_initvertex(graph, g_options.bt_alg.initvertflag);

  pstart = pend = 0;
  plength = 1;
  path[pstart].gvert = initvert;
  path[pstart].next = -1;
  graphpath[initvert].pathpos = pstart;

  /* update statistics */
  trialstats->nodes = 0;
  trialstats->edgeprune = 0;

  if (g_options.algorithm == ALG_BACKTRACK)
  {
    trialstats->initprune = 0;
  }

  /* copy passed in graph to second graph data structure 
   * so that original graph isn't overwritten when pruning, etc.
   */
  copy_graph(graph, &testgraph);

  /* do initial pruning */
  ret = hc_do_pruning(&testgraph, &prune, HC_PRUNE_ALL, &edgestack);

  /* update initial prune statistic */
  trialstats->initprune = prune;

  if (ret == HC_NOT_EXIST)
  {
    return(HC_NOT_EXIST);
  }
  /* if have forced HC, then just let backtrack quickly find it to get
   * the actual solution 
   */
  prune = 0;

  /* call recursive hc-backtrack algorithm */
  if (g_options.algorithm == ALG_NOPRUNE_BT)
  {  
    ret = calc_noprune_bt_alg(&testgraph, &pstart, &pend, &plength, path, 
		graphpath, &nodecount);

    trialstats->nodes = nodecount;
  }
  else if ( (g_options.algorithm == ALG_BACKTRACK) &&
	    (g_options.bt_alg.restart_increment == 0) )
  {
    ret = calc_backtrack_alg(graph, &testgraph, &pstart, &pend, &plength, 
      path, graphpath, &nodecount, &prune, &edgestack);

    trialstats->edgeprune = prune;
    trialstats->nodes     = nodecount;
  }
  else
  { /* pruning backtrack with iterated restart */
    int totnodes = 0;
    int totprune = 0;
    graph_type usegraph;
    edgestack_type prunestack;

    copy_edgestack(&edgestack, &prunestack);

    g_options.bt_alg.max_nodes = g_options.bt_alg.restart_increment *
				graph->numvert;

    /* this loop won't terminate until a solution is found
       use timelimit option to terminate earlier
     */
    do {

      /* initialize variables for each time around
         initialize to after initial pruning
       */
      prune = 0;
      nodecount = 0;
      copy_graph(&testgraph, &usegraph);
      copy_edgestack(&prunestack, &edgestack);

      for (loop = 0; loop < graph->numvert; loop++)
      {
        graphpath[loop].pathpos = -1;
        path[loop].gvert = -1;
        path[loop].next = -1;
      }
  
      initvert = select_initvertex(&usegraph, g_options.bt_alg.initvertflag);

      pstart = pend = 0;
      plength = 1;
      path[pstart].gvert = initvert;
      path[pstart].next = -1;
      graphpath[initvert].pathpos = pstart;

      g_hit_nodelimit = HIT_NO;

      ret = calc_backtrack_alg(graph, &usegraph, &pstart, &pend, &plength,
        path, graphpath, &nodecount, &prune, &edgestack);

      totprune += prune;
      totnodes += nodecount;      

      /* backtrack failed so increase node count for next iteration */
      if (ret == HC_NOT_FOUND)
      {
        g_options.bt_alg.max_nodes *= g_options.bt_alg.restart_increment;

        /* @@ eventually print to log file */
        fprintf(stderr, "Increment & Restart:  maximum nodes limit = %d\n",
          g_options.bt_alg.max_nodes);
      }

    } while   ( (ret == HC_NOT_FOUND) &&
		(hc_check_timelimit() != HC_QUIT) ) ;  

    trialstats->edgeprune = totprune;
    trialstats->nodes     = totnodes;

  }  /* end of pruning backtrack with iterated restart */

  if (ret == HC_FOUND)
  {
    /* convert path to solution */
    for (tempnum = pstart, loop = 0; loop < graph->numvert; loop++)
    {
      solution[loop] = path[tempnum].gvert;
      tempnum = path[tempnum].next;
    }
  }

  return(ret);

}  /* end of master_backtrack_alg() */


/************************************************************/
/* backtrack algorithm option routines, for handling the options
 * associated with the backtrack algorithm
 */
/************************************************************/

/************************************************************/
/* this function parses the option file for parameters for using
 * the backtrack and pruning backtrack algorithms.
 *
 * input parameters:  word : the argument
 *                      fp : file pointer to file, set to the parameters of 
 *                           the argument
 * output parameters: word : the next argument/comment (non-parameter) word
 *                  return : the status of the most recent read
 *
 * option file parameters are:
 *  (defaults are normal or random flag settings )
 * 
 *  Parameters for both basic and pruning backtrack:
 *  +initvert= <see below> : how to select initial vertex
 *       random : 
 *       maxdeg : maximum degree vertex is selected
 *       randeg : probability of selection proportional to degree
 *       first  : first vertex in graph is selected
 *
 *  Pruning backtrack parameters:
 *  +degsort = <see below> : sort visit list by degree, specifying sort order
 *       rand = random order (default) (uses order in graph, not really random)
 *       min  = min degree first, increasing order
 *       max  = max degree first, decreasing order
 *  +pruneopt = <see below> : specify level of pruning to do (hc_do_pruning)
 *       each character adds another type of pruning to be done
 *       'N' = no pruning done (default)
 *       'B' = basic pruning only (deg 2)
 *       'C' = cycle (forced path) pruning
 *       'O' = connect components pruning (checking)
 *       'A' = articulation (cut) point checking
 *  +restart = <N> : specify iterated restart mode & increment size
 *
 *  Other Possible Pruning backtrack options not implemented (yet)
 *  +forcecheck : check and follow forced edges
 *  +edgeprune  : remove other edges from old end vertex to unvisited verts
 *
 */
int 
parse_backtrack_alg_options(
  FILE *fp,
  char word[])
{
  int status;
  int loop;

  char parmstr[STRLEN];
  char valstr[STRLEN];

  /* read next word in options file */
  status = read_next_word(fp, word);

  /* read next parameter, till read a non-parameter word, then return */
  while ( !(status & READ_EOF) )
  {
    if (check_word(word) == WORD_PARM)
    {
      /* parse the parameter */
      parse_parameter_str(word, parmstr, valstr);

      /* check parameters for both basic backtrack and pruning backtrack */

      /* initvertflag parameter */ 
      if (strcasecmp(parmstr, "+initvert")==0) 
      {
        if (strcasecmp(valstr, "random")==0)
        {
          g_options.bt_alg.initvertflag = INITVERT_RANDOM;
        }
        else if (strcasecmp(valstr, "maxdeg")==0)
        {
          g_options.bt_alg.initvertflag = INITVERT_MAXDEG;
        }
        else if (strcasecmp(valstr, "randeg")==0)
        {
          g_options.bt_alg.initvertflag = INITVERT_RANDEG;
        }
        else if (strcasecmp(valstr, "first")==0)
        {
          g_options.bt_alg.initvertflag = INITVERT_FIRST;
        }
        else
        {
          EXIT_ERROR1("Error: invalid +initvert value `%s'.\n", valstr);
        }
      }

      /* pruning backtrack parameters only */
      else if (g_options.algorithm == ALG_NOPRUNE_BT)
      {
        WARN_ERROR1("Warning: Unidentified parameter `%s'.\n", parmstr);
      }
     
      /* pruneoptflag parameter */
      else if (strcasecmp(parmstr, "+pruneopt")==0)
      {
        /* scan valstr for letters indicating which pruning methods to use */
        for (loop = 0; loop < strlen(valstr); loop++)
        {
          switch(valstr[loop])
          {
            case 'N':
            case 'n':
              g_options.bt_alg.pruneoptflag = HC_PRUNE_NONE;
              break;
            case 'B':
            case 'b':
              g_options.bt_alg.pruneoptflag |= HC_PRUNE_BASIC;
              break;
            case 'C':
            case 'c':
              g_options.bt_alg.pruneoptflag |= HC_PRUNE_CYC;
              break;
            case 'O':
            case 'o':
              g_options.bt_alg.pruneoptflag |= HC_PRUNE_CONNECT;
              break;
            case 'A':
            case 'a':
              g_options.bt_alg.pruneoptflag |= HC_PRUNE_CUTPOINT;
              break;
            default:
          EXIT_ERROR1("Error: invalid +pruneopt value `%c'.\n", valstr[loop]);
              break;
          }  /* end of switch */
        }  /* end of for loop */
      }

      /* degsortflag parameter */
      else if (strcasecmp(parmstr, "+degsort")==0)
      {
        if (strcasecmp(valstr, "rand") == 0)
        {
          g_options.bt_alg.degsortflag = DEGSORT_RAND;
        }
        else if (strcasecmp(valstr, "min") == 0)
        {
          g_options.bt_alg.degsortflag = DEGSORT_MIN;
        }
        else if (strcasecmp(valstr, "max") == 0)
        {
          g_options.bt_alg.degsortflag = DEGSORT_MAX;
        }
        else
        {
          EXIT_ERROR1("Error: invalid +degsort value `%s'.\n", valstr);
        }
      }

      /* parse restart parameter */
      else if (strcasecmp(parmstr, "+restart")==0)
      {
        g_options.bt_alg.restart_increment = atoi(valstr);
       
        if (g_options.bt_alg.restart_increment < 2)
        {
          EXIT_ERROR("Restart increment size invalid.\n");
        }
      }

      else
      {
        WARN_ERROR1("Warning: Unidentified parameter `%s'.\n", parmstr);
      }
    }
    else
    { 
      return (status);
    }

    status = read_next_word(fp, word);

  }  /* end of while loop */  

  return(status);

}  /* end of parse_backtrack_alg_options() */


/************************************************************/
/* this function initializes the bt alg options
 * default values are set to those of basic backtrack.
 */
void
init_backtrack_alg_options()
{
  g_options.bt_alg.initvertflag = INITVERT_RANDOM;
  g_options.bt_alg.degsortflag  = DEGSORT_RAND;
  g_options.bt_alg.pruneoptflag = HC_PRUNE_NONE;

  g_options.bt_alg.restart_increment = 0;
  g_options.bt_alg.max_nodes	     = 0;

}  /* end of init_backtrack_alg_options() */


/************************************************************/
/* this function initializes the bt alg options
 * default values are set
 */
void
print_backtrack_alg_options(
  FILE *fp)
{
  /* options for both basic backtrack and pruning backtrack */
  fprintf(fp, "    initial vertex selection flag = ");
  if (g_options.bt_alg.initvertflag == INITVERT_RANDOM)
    fprintf(fp, "random\n");
  else if (g_options.bt_alg.initvertflag == INITVERT_MAXDEG)
    fprintf(fp, "max degree\n");
  else if (g_options.bt_alg.initvertflag == INITVERT_RANDEG)
    fprintf(fp, "random weighted by degree\n");
  else if (g_options.bt_alg.initvertflag == INITVERT_FIRST)
    fprintf(fp, "first vertex in graph\n");

  /* pruning backtrack options only */
  if (g_options.algorithm == ALG_BACKTRACK)
  {
    fprintf(fp, "    sort visit list by method = ");
    if (g_options.bt_alg.degsortflag == DEGSORT_RAND)
      fprintf(fp, "random\n");
    else if (g_options.bt_alg.degsortflag == DEGSORT_MIN)
      fprintf(fp, "min degree (increasing degree)\n");
    else if (g_options.bt_alg.degsortflag == DEGSORT_MAX)
      fprintf(fp, "max degree (decreasing degree)\n");
  }

  /* iterated restart, if specified */
  if (g_options.bt_alg.restart_increment > 0)
  {
    fprintf(fp, "    iterated restart increment = %d\n",
      g_options.bt_alg.restart_increment);
  }

  fprintf(fp, "    graph pruning/checking options = ");
  if (g_options.bt_alg.pruneoptflag == HC_PRUNE_NONE)
  {
    fprintf(fp, "none\n");
  }
  else
  {
    if (g_options.bt_alg.pruneoptflag & HC_PRUNE_BASIC) fprintf(fp, "basic ");
    if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CYC) fprintf(fp, "cycle ");
    if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CONNECT) 
      fprintf(fp, "connected ");
    if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CUTPOINT) 
      fprintf(fp, "cutpoints ");
    fprintf(fp, "\n");
  }
  
}  /* end of print_backtrack_alg_options() */

