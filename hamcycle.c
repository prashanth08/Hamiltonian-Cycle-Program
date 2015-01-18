

/* project includes */

#include "main.h"

/* local global variable: starting time of algorithm */

struct timeval g_algstart;
int g_hit_timelimit = RUN_NORMAL;


/************************************************************/
/* function to check the time limit for a \hc\ algorithm 
 * returns HC_QUIT if out of time
 * returns HC_CONTINUE otherwise
 */
int
hc_check_timelimit()
{
  long timeret;
  struct rusage curtime;
  struct timeval nowtime;
  float difftime;

  /* make sure time limit is being used */
  if (g_options.alg_timelimit < 0)
    return(HC_CONTINUE);

  /* calculate current time */
  timeret = getrusage(RUSAGE_SELF,&curtime);
  nowtime.tv_sec = curtime.ru_utime.tv_sec - g_algstart.tv_sec;
  nowtime.tv_usec = curtime.ru_utime.tv_usec - g_algstart.tv_usec;
  difftime = nowtime.tv_sec + ( (float) nowtime.tv_usec / 1000000.0);

  /* check if difference is more than time limit */
  if (difftime >= g_options.alg_timelimit)
  {
    g_hit_timelimit = RUN_TIMELIMIT;
    return(HC_QUIT);
  }
  else
    return(HC_CONTINUE);

}  /* end of hc_check_timelimit() */


/************************************************************/
/* function to verify that a specified solution is indeed a hamiltonian
 * cycle of the graph
 * solution is represented by an array of vertices
 *
 * returns HC_VERIFY if it is a solution
 * returns HC_NOT_VERIFY if it is not
 */
int
hc_verify_solution(
  graph_type *graph,
  int solution[])
{
  int loop;
  int vertcount[MAXVERT];
  
  /* first verify that the solution is indeed a path of length = numvert */
  for (loop = 0; loop < (graph->numvert)-1; loop++)
  {
    if ( check_if_edge(graph, solution[loop], solution[loop+1]) != EDGE_EXIST)
      return(HC_NOT_VERIFY);
  }

  /* check if endpoints are connected (have a cycle) */
  if ( check_if_edge(graph, solution[graph->numvert-1], solution[0]) 
	!= EDGE_EXIST)
    return(HC_NOT_VERIFY);
 
  /* check to see that there are no repeats of vertices */

  /* initialize array */
  for (loop = 0; loop < graph->numvert; loop++)
  { 
    vertcount[loop] = 0;
  }

  /* count # of times each vertex appears */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    vertcount[solution[loop]] ++;
  }

  /* check that each vertex only appeared once */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    if (vertcount[loop] != 1)
      return(HC_NOT_VERIFY);
  }

  return(HC_VERIFY);

}  /* end of hc_verify_solution() */


/************************************************************/
/* A set of functions to handle an edgestack
 * (a stack which stores edges)
 */
/************************************************************/

/************************************************************/
/* function to add a (deleted) edge to an edge stack 
 * v1,v2 specifies the vertex endpoints of the edge
 *
 * if edgestack is NULL then just return
 * (NULL indicates that don't want to save stack information) 
 */
void
push_edge_to_stack(
  int v1, 
  int v2, 
  edgestack_type *edges)
{
  if (edges == NULL) return;

  edges->stack[edges->pointer].v1 = v1;
  edges->stack[edges->pointer].v2 = v2;
  (edges->pointer)++;

}  /* end of push_edge_to_stack() */


/************************************************************/
/* function to remove a (deleted) edge from an edge stack 
 * the edge is stored in v1,v2 
 */
void
pop_edge_from_stack(
  int *v1, 
  int *v2, 
  edgestack_type *edges)
{
  (edges->pointer)--;

  *v1 = edges->stack[edges->pointer].v1;
  *v2 = edges->stack[edges->pointer].v2;

}  /* end of pop_edge_from_stack() */


/************************************************************/
/* function to return current stack pointer location
 */
int
get_curpnt_edgestack(
  edgestack_type *edges)
{
  return (edges->pointer);

}  /* end of init_edgestack() */


/************************************************************/
/* function to copy an edge stack
 */
void
copy_edgestack(
  edgestack_type *fromstack,
  edgestack_type *tostack)
{
  int loop;

  for (loop = 0; loop < fromstack->pointer; loop++)
  {
    tostack->stack[loop].v1 = fromstack->stack[loop].v1;
    tostack->stack[loop].v2 = fromstack->stack[loop].v2;
  }
  tostack->pointer = fromstack->pointer;

}  /* end of copy_edgestack() */


/************************************************************/
/* function to initialize an edge stack
 */
void
init_edgestack(
  edgestack_type *edges)
{
  edges->pointer = 0;

}  /* end of init_edgestack() */


/************************************************************/
/* this function selects an initial vertex from the specified graph
 *
 * the method of selection is specified by the selectflag parameter:
 *   INITVERT_RANDOM - select at random
 *   INITVERT_MAXDEG - select vertex of maximum degree
 *   INITVERT_RANDEG - select a random vertex with probability proportional
 *                     to degree
 *   INITVERT_FIRST  - select first vertex in graph
 *   
 * the vertex selected is returned
 */
int
select_initvertex(
  graph_type *graph,
  int selectflag) 
{
  int selvert = 0;

  int vlist[MAXVERT];
  int numvert;
  int maxdeg;
  int degsum;
  int loop;

  switch(selectflag)
  {
    case INITVERT_RANDOM:
      /* select a vertex at random */
      selvert = lrand48() % graph->numvert;
      break;

    case INITVERT_MAXDEG:
      /* select a random vertex from those vertices of maximum degree */

      /* first calculate the maximum degree */
      for ( maxdeg = graph->deg[0], loop = 1; loop < graph->numvert; loop++)
      {
        if (graph->deg[loop] > maxdeg)
        {
          maxdeg = graph->deg[loop];
        }
      }

      /* create list of vertices of maximum degree */
      for (numvert = 0, loop = 0; loop < graph->numvert; loop++)
      {
        if (graph->deg[loop] == maxdeg)
        {
          vlist[numvert++] = loop;
        }
      }

      /* randomly select a vertex from this list */
      selvert = vlist[ (lrand48() % numvert) ];
          
      break;

    case INITVERT_RANDEG:
      /* select a vertex randomly, with selection probability weighted
       * according to vertex degree
       */

      /* first calculate summation of vertex degrees */
      for (degsum = 0, loop = 0; loop < graph->numvert; loop++)
      {
        degsum += graph->deg[loop];
      }

      /* do weighted random selection */
      degsum = lrand48() % degsum;
      selvert = -1;
      do
      {
        selvert++;
        degsum -= graph->deg[selvert];
      } while (degsum > 0);
      
      break;

    case INITVERT_FIRST:
      selvert = 0;
      break;

  }  /* end of switch */

  return (selvert);

}  /* end of select_initvertex() */


/************************************************************/
/* function that starts with 1 degree 2 vertex, and forms a forced
 * path.  Any edge between the endpoints of this forced path is removed
 * (and further path-extending is then done).  
 * also returns HC_NOT_EXIST if a forced short cycle is found
 *	(start and end vertices are the same)
 * returns HC_FOUND if a forced hamiltonian cycle is found
 * otherwise returns HC_NOT_FOUND
 *
 * prune is incremented for each edge removed
 * each deg 2 vertex in the forced path is marked as used 
 *   (using the used[] array)
 *
 * deleted edges are added to the edgestack structure
 *
 * assumptions:  curvert == degree 2
 */
int
extend_forced_path(
  int curvert,
  graph_type *graph,
  int used[],
  int *prune,
  edgestack_type *edgestack)
{
  int length;

  int done;
  int startvert;	/* starting vertex of path */
  int endvert;		/* ending vertex of path */
  int oldsv;		/* previous starting vertex */
  int oldev;		/* previous ending vertex */

  int tmp;


  oldsv = oldev = curvert;

  startvert = graph->nbr[curvert][0];
  endvert = graph->nbr[curvert][1];
  length = 3;
  used[curvert] = used[startvert] = used[endvert] = 1;

  /* main extend-path loop */
  do
  {
    done = 1;

    /* extend the start vertex */
    while (graph->deg[startvert] == 2)
    {
      /* get next edge along forced path */
      if (graph->nbr[startvert][0] != oldsv)
        tmp = graph->nbr[startvert][0];
      else
        tmp = graph->nbr[startvert][1];
  
      /* if have a forced cycle, check its length */
      if (tmp == endvert)
      {
        if (length < graph->numvert)	/* cycle is short */
          return(HC_NOT_EXIST);
        else
          return(HC_FOUND);
      }
  
      oldsv = startvert;
      startvert = tmp;
      length++;
      used[startvert] = 1;
    }
      
    /* extend the end vertex */
    while (graph->deg[endvert] == 2)
    {
      /* get next edge along forced path */
      if (graph->nbr[endvert][0] != oldev)
        tmp = graph->nbr[endvert][0];
      else
        tmp = graph->nbr[endvert][1];
  
      /* if have a forced cycle, check its length */
      if (tmp == startvert)
      {
        if (length < graph->numvert)	/* cycle is short */
          return(HC_NOT_EXIST);
        else
          return(HC_FOUND);
      }
  
      oldev = endvert;
      endvert = tmp;
      length++;
      used[endvert] = 1;
    }
    
    /* have forced path, so try to remove an edge inbetween the endpoints 
     * but do this only if current path length is less than total number of
     * vertices*/
    if (length < graph->numvert)
    {
      if (rm_edge_graph(graph, startvert, endvert) == EDGE_REMOVE)
      {
        done = 0;	/* keep trying to extend path */
        (*prune)++;
        push_edge_to_stack(startvert, endvert, edgestack);
      }
    }

  } while (!done); /* end of main while loop */

  /* unmark start and end vertices, since they are not deg 2 */
  used[startvert] = used[endvert] = 0;

  return(HC_NOT_FOUND);

}  /* end of extend_forced_path() */


/************************************************************/
/* function to do pruning on the graph, including
 * simple checks to see if a HC can exist. 
 * returns HC_NOT_EXIST if a HC cannot exist
 * returns HC_FOUND if a HC was found  (unlikely to happen)
 * returns HC_NOT_FOUND otherwise
 *
 * the number of edges pruned (removed) from the graph is returned in the
 * prune argument
 *
 * the edges removed are added to the edgestack structure, in case they
 * need to be undeleted in the future.  if this feature is not desired,
 * just pass NULL to the edgestack structure.
 *
 * the prunelevel argument specifies the amount of pruning to do
 * currently, the options are:
 *   HC_PRUNE_BASIC : check for vertices of degree >= 2 
 *                    check for vertices with a max of 2 deg 2 neighbours
 *                      and remove extra edges if have 2 deg 2 neighbours
 *   HC_PRUNE_CYC   : for any path of forced edges, remove edges that make
 *                      a cycle
 *   HC_PRUNE_CONNECT:  check for graph being connected
 *   HC_PRUNE_CUTPOINT: check for existance of a cutpoint.
 *   HC_PRUNE_ALL   : do all pruning
 *
 */
int 
hc_do_pruning(
  graph_type *graph,
  int *prune,
  int prunelevel,
  edgestack_type *edgestack)
{
  int done;

  int ret;
  int degmrk[MAXVERT];
  int loop, eloop;
  int curprune;
  int used[MAXVERT];  
  int newvert;

  /************************************************************/
  /* checks to see if H.C. can not exist, and prunes (removes) edges 
   * that cannot be part of the HC:
   *
   *   to see if HC exists:
   *     - are all graph degrees >= 2?
   *     - do vertices have a max of 2 neighbours of degree 2?
   *     - is the graph connected?
   *     - do articulation (cut) points exist?
   *   to prune edges:
   *     - if a vertex has 2 degree neighbours, remove all other edges
   *     - for any path of forced edges (ie vertices have degree 2) remove
   *     any edges that lead to a cycle (assuming length of path < number
   *     of vertices in graph.
   *
   *   note that each time an edge is removed, the pruning steps are all
   *   redone, since pruning one edge could lead to other edges being
   *   pruned
   */

  *prune = 0;
  do
  {
    done = 1;

    if (prunelevel & HC_PRUNE_BASIC)
    {
      /* check if vertex degrees are >= 2 */
      for (loop = 0; loop < graph->numvert; loop++)
      {
        if (graph->deg[loop] < 2)
        {
          if (g_options.report_flags & REPORT_ALG)
          {
            fprintf(g_options.log_fp, "Vertex %d has degree %d, ", loop, 
  					graph->deg[loop]);
            fprintf(g_options.log_fp, "which implies no hamiltonian cycle.\n");
          }
          return(HC_NOT_EXIST);
        }
      }

      /* check if # of deg 2 neighbours is <= 2 */
      for (loop = 0; loop < graph->numvert; loop++)
      {
        degmrk[loop] = 0;
  
        for(eloop = 0; eloop < graph->deg[loop]; eloop++)
        {
          if (graph->deg[graph->nbr[loop][eloop]] == 2)
            degmrk[loop]++;
        }
  
        if (degmrk[loop] > 2)
        {
          if (g_options.report_flags & REPORT_ALG)
          {
            fprintf(g_options.log_fp,
              "Vertex %d has %d neighbours of degree 2, ", loop, degmrk[loop]);
           fprintf(g_options.log_fp, "which implies no hamiltonian cycle.\n");
          }
          return(HC_NOT_EXIST);
        }
      }
    }

    /* initialize used[] variable */
    for (loop = 0; loop < graph->numvert; loop++)
    {
      used[loop] = 0;
    }

    /************************************************************/
    /* prune edges that cannot be traversed in H.C. 
     */
    for (loop = 0; loop < graph->numvert; loop++)
    {

      if (prunelevel & HC_PRUNE_BASIC)
      {
        /* prune extra edges of vertices with 2 degree-2 neighbours */
        /* note that this vertex also becomes degree 2 */
        if ( (degmrk[loop] == 2) && (graph->deg[loop] > 2) )
        {
          eloop = 0;
          while(eloop < graph->deg[loop])
          {
            newvert = graph->nbr[loop][eloop];
            if (graph->deg[newvert] != 2 )
            {
              rm_edge_graph(graph, loop, newvert);
              (*prune)++;
              push_edge_to_stack(loop, newvert, edgestack);
              done = 0;		/* redo entire pruning check */
            }
            else
              eloop++;
          }
        }
      }

      if (prunelevel & HC_PRUNE_CYC) 
      {
        /* find longest path of forced edges, and remove any cycle-creating
         * edge from this path.
         * only check each forced path once, by using the used[] array
         */
        if ( (graph->deg[loop] == 2) && (used[loop] == 0) )
        {
          curprune = *prune;
          ret = extend_forced_path(loop, graph, used, prune, edgestack);
          if (ret == HC_NOT_EXIST)
          {
            if (g_options.report_flags & REPORT_ALG)
            {
              fprintf(g_options.log_fp, "Graph has a forced short cycle, ");
              fprintf(g_options.log_fp, 
                "which implies no hamiltonian cycle.\n");
            }
            return(HC_NOT_EXIST);
          }
    
          /* if HC found, indicate so and return */
          if (ret == HC_FOUND)
          {
            if (g_options.report_flags & REPORT_ALG)
            {
              fprintf(g_options.log_fp,
                "Graph has a forced hamiltonian cycle.\n");
            }
            return(HC_FOUND);
          }

          if (*prune > curprune)  /* pruning took place, redo pruning check */
            done = 0;
        }

      }  /* end of HC_PRUNE_CYC if statement */

    }  /* end of loop through graph vertices */

  }  while (!done); /* end of prune-graphcheck loop */

  /* check for components */
  if (prunelevel & HC_PRUNE_CONNECT)
  {
    if (calc_graph_components(graph) > 1) 
    {
      if (g_options.report_flags & REPORT_ALG)
      {
        fprintf(g_options.log_fp, "The graph has multiple components, ");
        fprintf(g_options.log_fp, "which implies no hamiltonian cycle.\n");
      }
      return(HC_NOT_EXIST);
    } 
  }
    
  /* check for articulation points */
  if (prunelevel & HC_PRUNE_CUTPOINT)
  {
    if (check_graph_cutpoints(graph) == CUTPNT_EXIST)
    {
      if (g_options.report_flags & REPORT_ALG)
      {
        fprintf(g_options.log_fp, "The graph has at least one cutpoint, ");
        fprintf(g_options.log_fp, "which implies no hamiltonian cycle.\n");
      }
      return(HC_NOT_EXIST);
    }
  }

  return(HC_NOT_FOUND);

}  /* end of hc_do_pruning() */


/************************************************************/
/* function to add a vertex to the current path
 *   vert = new vertex
 *   assumes vertices stored in path[] from 0 to *plength-1
 */
void
add_vert_to_path(
  path_type path[],
  graphpath_type graphpath[],
  int *pstart,
  int *pend,
  int *plength,
  int vert)
{
  path[*pend].next = *plength;
  path[*plength].gvert = vert;
  path[*plength].next = -1;
  graphpath[vert].pathpos = *plength;
  *pend = *plength;
  (*plength)++;

}  /* end of add_vert_to_path() */

  
/************************************************************/
/* function to remove the end vertex of the current path
 *   must specify second-from-the-end vertex (oldend)
 *   assumes vertices stored in path[] from 0 to *plength-1
 *
 * @@ this will break if the path order is messed with.  to fix, must
 * move vertex on the end (at location *plength-1) to the location of the
 * deleted endpoint.
 */
void
remove_endvert_from_path(
  path_type path[],
  graphpath_type graphpath[],
  int *pstart,
  int *pend,
  int *plength,
  int oldend)
{
  graphpath[path[*pend].gvert].pathpos = -1;
  path[*pend].gvert = -1;
  path[*pend].next = -1;
  *pend = oldend;
  (*plength)--;
  path[*pend].next = -1;

}  /* end of remove_endvert_from_path() */


/************************************************************/
/* rotational transformation to reverse path in cycle.
 * ie: A-B-C-D, edge from D-A
 * new path: A-D-C-B  (B is new end of path)
 *
 * endpathv = path index of end-of-path, which is changed
 * revpathv = path index of start of reverse path (A)
 * plength = length of path
 *
 * included in hamcycle.c because it is used in the generic
 *   path_to_cycle() function
 */
void
hc_reverse_path(
  path_type path[],
  graphpath_type graphpath[],
  int *endpathv,
  int revpathv,
  int plength)
{
  int i, j;
  int tempnum;
  int newend;

  newend = path[revpathv].next;
  path[revpathv].next = *endpathv;

  i = newend;
  j = path[i].next;

  do
  {    
    /* make j point to i, but save what j points to first */
    tempnum = path[j].next;
    path[j].next = i;

    /* advance i and j down path */
    i = j;
    j = tempnum;

  } while (j != -1);  /* stop when i = oldend, j points to nothing */

  /* set up new end of path */
  path[newend].next = -1;
  *endpathv = newend;
  graphpath[path[newend].gvert].ended = plength;   /* for posa's only */

}  /* end of hc_reverse_path() */


/************************************************************/
/* this function trys to transform a given (hamiltonian) path to 
 * a (hamiltonian) cycle
 * note that the path does not have to be hamiltonian for the algorithm
 * to work.  it will just try to form a cycle from the given path
 *
 * returns HC_FOUND if successfull, HC_NOT_EXIST if unsuccessfull
 *
 * if HC_FOUND, the given path is modified so that the end vertex has an
 *   edge to the start vertex.
 *
 * note that in modifying the path, an edge will be 'removed' from the path
 * (between new end and predecessor of old end in new path)
 * this edge _cannot_ be forced, since both vertices must be at least
 * degree 3 in order for this transformation to work
 */
int
hc_path_to_cycle(
  graph_type *graph,
  path_type path[],
  graphpath_type graphpath[],
  int *pstart, 
  int *pend,
  int plength)
{
  int loop;
  int tempvert, curvert;
  int tempnum;
 
  /* check if have a simple hamiltonian path */
  if ( check_if_edge(graph, path[*pend].gvert, path[*pstart].gvert) 
       == EDGE_EXIST) 
  {
    return(HC_FOUND);
  }

  /* for each neighbour of end vertex, check to see if next vertex to it in
   * path has edge with start.  if so, then can construct a cycle
   */
  for (loop = 0; loop < graph->deg[path[*pend].gvert]; loop++)
  {
    tempvert = graph->nbr[path[*pend].gvert][loop];

    /* get next vertex in path */
    tempnum = graphpath[tempvert].pathpos;
    tempnum = path[tempnum].next;
    curvert = path[tempnum].gvert;

    /* check to see if this vertex has an edge with the start */
    if (check_if_edge(graph, curvert, path[*pstart].gvert) == EDGE_EXIST)
    {
      /* do reversal of path */
      hc_reverse_path(path, graphpath, pend, graphpath[tempvert].pathpos, 
			plength);

      return(HC_FOUND);
    }

  }  /* end of loop through neighbours of end-vertex on path */

  return(HC_NOT_EXIST);

}  /* end of hc_path_to_cycle() */
  

/************************************************************/
/* testing wrapper for hamiltonian cycle algorithms
 *
 * this executes a single trial
 *
 * pass in graph to test, and stats to update
 * graph should not be changed at end of function
 * return RUN_TIMELIMIT if hit timelimit, otherwise return RUN_NORMAL
 */
int 
test_hc_alg(
  graph_type *graph,
  stat_pertrial_type *trialstats)
{
  int retval = RUN_NORMAL;

  int hcret = 0;
  long timeret;
  struct rusage curtime;
  int solution[MAXVERT];

  int loop;

  if (g_options.report_flags & REPORT_ALG)
  {
    switch(g_options.algorithm)
    {
      case ALG_NOPRUNE_BT:
        fprintf(g_options.log_fp, 
	"Starting basic backtrack algorithm to find hamiltonian cycle...\n");
        break;
      case ALG_POSA_HEUR:
        fprintf(g_options.log_fp, 
	"Starting posa-like algorithm to find hamiltonian cycle...\n");
        break;
      case ALG_BACKTRACK:
        fprintf(g_options.log_fp, 
	"Starting pruning backtrack algorithm to find hamiltonian cycle...\n");
        break;
      default:
        EXIT_ERROR("Invalid algorithm in test_hc_alg().\n");
        break;
    }
  }

  /* start stat timer */
  g_hit_timelimit = RUN_NORMAL;
  timeret = getrusage(RUSAGE_SELF,&curtime);
  g_algstart.tv_sec = curtime.ru_utime.tv_sec;
  g_algstart.tv_usec = curtime.ru_utime.tv_usec;

  /* run algorithm */
  switch(g_options.algorithm)
  {
    case ALG_NOPRUNE_BT:
    case ALG_BACKTRACK:
      hcret = master_backtrack_alg(graph, trialstats, solution);
      break;

    case ALG_POSA_HEUR:
      hcret = master_heuristic_alg(graph, trialstats, solution);
      break;
  }

  /* stop timing, and calculate elapsed time in seconds */
  timeret = getrusage(RUSAGE_SELF,&curtime);
  g_algstart.tv_sec = curtime.ru_utime.tv_sec - g_algstart.tv_sec;
  g_algstart.tv_usec = curtime.ru_utime.tv_usec - g_algstart.tv_usec;
  trialstats->time = g_algstart.tv_sec + ((float)g_algstart.tv_usec/1000000.0);

  trialstats->result = hcret;

  /* report if hit timelimit */
  if (g_hit_timelimit == RUN_TIMELIMIT) 
  {
    retval = RUN_TIMELIMIT;

    WARN_ERROR("Warning: algorithm reached time limit.\n");

    if (g_options.report_flags & REPORT_ALG)
    {
      fprintf(g_options.log_fp, "Algorithm reached time limit.\n");
    }
  }

  /* verify solution */
  if (hcret == HC_FOUND)
  {
      if (hc_verify_solution(graph, solution) == HC_NOT_VERIFY)
      {
        EXIT_ERROR("Error:  algorithm calculated bad solution.\n");
      }
  }

  /* print solution status */
  if (g_options.report_flags & REPORT_ALG) 
  {
    if (hcret == HC_FOUND)
      fprintf(g_options.log_fp, "Hamiltonian cycle found.\n");
    else if (hcret == HC_NOT_EXIST)
      fprintf(g_options.log_fp, "Hamiltonian cycle does not exist.\n");
    else if (hcret == HC_NOT_FOUND)
      fprintf(g_options.log_fp, "Hamiltonian cycle not found.\n");
  }

  /* print solution if found */
  if (g_options.report_flags & REPORT_SOLUTION)
  {
    if (hcret == HC_FOUND)
    {
      /* print solution */
      fprintf(g_options.sol_fp, "Cycle \n");
      for (loop = 0; loop < graph->numvert; loop++)
      {
        fprintf(g_options.sol_fp,"%4d ", solution[loop]);
        if ( (loop+1)%10 == 0)
          fprintf(g_options.sol_fp, "\n");
      }
      if (loop % 10 != 0) 
        fprintf(g_options.sol_fp, "\n");
    }
    else
      fprintf(g_options.sol_fp, "NoCycle \n");
  }

  return(retval);
    
}  /* end of test_hc_alg() */


