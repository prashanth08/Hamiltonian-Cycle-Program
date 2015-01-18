
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: heuristic.c
 * Contents: heuristic Hamiltonian Cycle algorithms
 *
 */
/*
	Copyright (c) 1998 Basil Vandegriend and Joseph Culberson. 
	All rights reserved.

     ``THIS SOURCE CODE IS SUPPLIED  ``AS IS'' WITHOUT WARRANTY OF ANY KIND,
     AND ITS AUTHOR AND THE JOURNAL OF ARTIFICIAL INTELLIGENCE RESEARCH
     (JAIR) AND JAIR'S PUBLISHERS AND DISTRIBUTORS, DISCLAIM ANY AND ALL
     WARRANTIES, INCLUDING BUT NOT LIMITED TO ANY IMPLIED
     WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, AND
     ANY WARRANTIES OR NON INFRINGEMENT.  THE USER ASSUMES ALL LIABILITY AND
     RESPONSIBILITY FOR USE OF THIS SOURCE CODE, AND NEITHER THE AUTHOR NOR
     JAIR, NOR JAIR'S PUBLISHERS AND DISTRIBUTORS, WILL BE LIABLE FOR
     DAMAGES OF ANY KIND RESULTING FROM ITS USE.  Without limiting the
     generality of the foregoing, neither the author, nor JAIR, nor JAIR's
     publishers and distributors, warrant that the Source Code will be
     error-free, will operate without interruption, or will meet the needs
     of the user.''

        Redistribution and use in source and binary forms, with or without
        modification, are permitted provided that the following conditions
        are met:
        1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        3. All advertising materials mentioning features or use of this
        software must display the following acknowledgement:
        "This product includes software developed by the University of
        Alberta, Edmonton."
        4. Neither the name of the University nor the names of its
	contributors may be used to endorse or promote products derived 
	from this software without specific prior written permission.

        THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS ``AS IS'' AND ANY
        EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
        THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
        PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
        CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
        NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
        LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
        HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
        CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
        OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
        EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

        THIS SOFTWARE IS SUPPLIED WITHOUT ANY SUPPORT SERVICES.
*/
/************************************************************/

/* project includes */

#include "main.h"

/* for debugging only 
 * set to 1 or higher and set -report +ALG to get printing information
 */
#define VERBOSE_MODE 0


/************************************************************/
/* heuristic algorithm debugging function to print and verify current path
 * startpathv = path index of start-of-path
 * endpathv = path index of end-of-path
 * pathlength = length of path
 */ 
void
debug_print_path(
  graph_type *graph,
  path_type path[],
  graphpath_type graphpath[],
  int startpathv,
  int endpathv,
  int pathlength)
{
  int curpath;
  int curvert;
  int count;

  if ( !( (g_options.report_flags & REPORT_ALG) && (VERBOSE_MODE) ) )
  {
    /* not in debugging mode, return */
    return;
  }

  /* require a higher debugging mode */
  if (VERBOSE_MODE < 2)
    return;

  fprintf(g_options.log_fp, 
	"Verifying current path, length = %d, start = %d, end = %d\n", 
	pathlength, startpathv, endpathv);

  curpath = startpathv;
  count = 0;

  do
  {
    count++;
    curvert = path[curpath].gvert;
  
    fprintf(g_options.log_fp, "  path position %d = graph vertex %d\n",
      curpath, curvert);
    
    if (graphpath[curvert].pathpos != curpath)
    {
      fprintf(g_options.log_fp, 
	"    Error: invalid graphpath.pathpos entry = %d\n",
           graphpath[curvert].pathpos);
    }

    if ( (count == pathlength) && (curpath != endpathv) )
    {
      fprintf(g_options.log_fp, "    Error: bad end variable.\n");
    }

    curpath = path[curpath].next;
  
    if ( (curpath != -1) &&
         (check_if_edge(graph, curvert, path[curpath].gvert) != EDGE_EXIST) )
    {
      fprintf(g_options.log_fp, 
	"    Error: no edge between this vertex and next.\n");
    }

  }  while (count < pathlength);

}  /* end of debug_print_path() */


/************************************************************/
/* this function executes the cycle extension technique
 *
 * basic idea:  have an incomplete path, and from endpoint cannot visit
 *              unvisited vertex
 *              search to see if path can be converted into a cycle
 *              if so, search for a random edge to break the cycle, and
 *              form a path with new endpoints, with the new _end_ point
 *              having an edge to an unvisited vertex
 *              (note that the edge broken to go from cycle to path must
 *               not be forced)
 * uses arguments to return changed path
 * returns # of new vertex to visit
 * if no such vertex, and cycle was formed, return -2
 * if couldn't form cycle, return -1
 *
 * note: it is assumed that current end point of graph has all its
 *       neighbours in the current path
 */ 
int
do_cycle_extend(
  graph_type *graph,
  path_type path[],
  graphpath_type graphpath[],
  int *pathstart,
  int *pathend,
  int pathlength)
{
  int searchstart;
  int foundvert;
  
  int curvert, curnum, nextvert, nextnum;
  int newvert = 0;

  int loop;

  /* indicates which edge to break (actual position in terms of # of
   * vertices down the path.  if = -1, then want to break edge between 
   * endpoint and startpoint, so no need to change path around
   */
  int breakposition = 0;

  path_type tmppath[MAXVERT];
  graphpath_type tmpgraphpath[MAXVERT];
  int tmpcur;

  /* first try to find a cycle */
  if (hc_path_to_cycle(graph, path, graphpath, pathstart, pathend, pathlength)
       == HC_FOUND)
  {
    if ( (g_options.report_flags & REPORT_ALG) && (VERBOSE_MODE) )
    {
      fprintf(g_options.log_fp, "  cycle extend: have a cycle.\n");
      debug_print_path(graph,path,graphpath,*pathstart,*pathend,pathlength);
      fflush(g_options.log_fp);
    }
  }  /* end of construct-cycle section */  
  else
  {
    /* cannot form a cycle, so return */
    return (-1);
  }

  /* search for point to break the cycle 
   * start at random point in path, check to see if edge to next vertex
   * can be broken or not.
   * criteria for breaking an edge:
   *   1.  not forced (neither endpoint is deg 2)
   *   2.  some neighbour of _next_ vertex (not current) is unvisited
   */
  searchstart = lrand48() % pathlength;
  curnum = searchstart;
  foundvert = 0;

  do
  {
    /* determine which two vertices are the endpoints of the edge in question */
    nextnum = (curnum +1) % pathlength;
    curvert = path[curnum].gvert;
    nextvert = path[nextnum].gvert;

    /* check that the edge is not forced */
    if ( (graph->deg[curvert] != 2) && (graph->deg[nextvert] != 2) )
    {
      /* check if nextvert has unvisited neighbour */
      for (loop = 0; loop < graph->deg[nextvert]; loop++) 
      {
        newvert = graph->nbr[nextvert][loop];
        if (graphpath[newvert].pathpos == -1)
        {
          foundvert = 1;
          break;
        }
      }
    } 
  
    /* if found unvisited vertex neighbour, then get out of this loop */
    if (foundvert == 1)
      break;

    /* advance search to next edge in path */
    curnum = nextnum;

  }  while (curnum != searchstart); 

  if (foundvert == 0)  /* no unvisited vertex neighbour found, so give up */
    return (-2);

  if ( (g_options.report_flags & REPORT_ALG) && (VERBOSE_MODE) )
  {
    fprintf(g_options.log_fp, 
	"  cycle extend: have cycle, new end vertex = %d \n", nextvert);
    fflush(g_options.log_fp);
  }

  /* rearrange path so that curvert = start of path and nextvert = end 
   * new vertex to visit is newvert
   * to do this, reverse path segment before edge to be broken, and after
   * ie:  from   A----B <-> C-----D  (break edge B,C)  (have edge D,A)
   *        to   B----A - D-----C  (C is new endpoint)
   * A = *pathstart, B = curnum, C = nextnum, D = *pathend
   *
   * easiest way to do this is to traverse entire path, and copy path into
   * temporary path variables, then copy back into original variables 
   *
   * why we don't need to worry about .ended parameter:  path is going to 
   * be extended by at least 1 vertex, so all current .ended parameters
   * will be invalid anyways.
   */

  /* @@ more efficent way to rearrange path, without recopying, by changing
   * start, end vertices
   */

  /* calculate actual breakposition:  # of vertices to position before
   * nextnum position, including start 
   */
  curnum = *pathstart;
  breakposition = 0;
 
  if (curnum == nextnum)
    breakposition = pathlength;
  else
  {
    do
    {
      breakposition++;
      curnum = path[curnum].next;
    } while (curnum != nextnum);
  }

  /* initialize tmpgraphpath structure */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    tmppath[loop].gvert = -1;
    tmppath[loop].next = -1;
    tmpgraphpath[loop].pathpos = -1;
    tmpgraphpath[loop].ended = -1;
  }

  /* copy section of old path from index 0 to nextnum-1 
   * (from middle to start of new path) 
   */
  for (curnum = *pathstart, tmpcur = breakposition-1; tmpcur >= 0; tmpcur --)
  {
    tmppath[tmpcur].gvert = path[curnum].gvert;
    tmppath[tmpcur].next = tmpcur+1;
    tmpgraphpath[tmppath[tmpcur].gvert].pathpos = tmpcur;
    tmpgraphpath[tmppath[tmpcur].gvert].ended = 0;

    /* advance index for old path */
    curnum = path[curnum].next;
  }

  /* copy section of old path from index nextnum to pathlength
   * (from end to middle of new path )
   */
  /* no need to reset curnum, should be at nextnum
   */
  for (curnum = nextnum, tmpcur = pathlength-1; 
		tmpcur >= breakposition; tmpcur--)
  {
    tmppath[tmpcur].gvert = path[curnum].gvert;
    tmppath[tmpcur].next = tmpcur+1;
    tmpgraphpath[tmppath[tmpcur].gvert].pathpos = tmpcur;
    tmpgraphpath[tmppath[tmpcur].gvert].ended = 0;

    /* advance index for old path */
    curnum = path[curnum].next;
  }

  /* setup endpoint in path properly */
  tmppath[pathlength-1].next = -1;
  tmpgraphpath[tmppath[pathlength-1].gvert].ended = pathlength;
  *pathend = pathlength-1;
  *pathstart = 0;

  /* now copy entire path back over original */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    path[loop].gvert = tmppath[loop].gvert;
    path[loop].next = tmppath[loop].next;
    graphpath[loop].pathpos = tmpgraphpath[loop].pathpos;
    graphpath[loop].ended = tmpgraphpath[loop].ended;
  }

  debug_print_path(graph, path, graphpath, *pathstart, *pathend, pathlength);
  return(newvert);

}  /* end of do_cycle_extend() */


/************************************************************/
/* POSA-like algorithm:  probabilistic algorithm for finding hamiltonian cycle
 * uses rotational transformation
 * 
 * function returns HC_FOUND if HC was found, HC_NOT_FOUND otherwise
 *
 * passed in to this function is the starting graph vertex (startv) from which 
 * to start constructing the cycle.  Since the algorithm is probabilistic,
 * specifying different starting vertices may change the results obtained
 * by this algorithm.
 *
 * the nodecount parameter is incremented by the # of nodes (vertices) 
 *     processed in finding the cycle. 
 *
 * if a solution is found, it is placed in the solution[] array
 *   (as an ordered list of graph vertices forming the cycle)
 */
int 
calc_posa_heur_alg(
  graph_type *graph,
  int startv,
  int solution[],
  int *nodecount)
{	
  path_type path[MAXVERT];
  graphpath_type graphpath[MAXVERT];

  int pstart, pend, plength;	/* path start, path end, path length */

  int found = 0;
  int nextvert;

  /* temporary calculation variables */
  int loop;
  int curvert, tempvert, tempnum;
  int i;

  /* initialize variables */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    path[loop].gvert = -1;
    path[loop].next = -1;
    graphpath[loop].pathpos = -1;
    graphpath[loop].ended = -1;
  }

  pstart = 0;
  pend = 0;
  plength = 1;
  path[pstart].gvert = startv;
  path[pstart].next = -1;
  graphpath[startv].pathpos = pstart;
  graphpath[startv].ended = plength;

  /* start loop of adding vertices to the path */
  while (!found)
  {
    /* select a neighbour of the end vertex on path to add to path 
     * select based upon options
     */
    curvert = path[pend].gvert;
    nextvert = -1;
    (*nodecount)++;	/* count iterations */

    if (g_options.heur_alg.visitflag == VISIT_SMART)
    {
      /* use several selection criteria:
       * 1.  select a deg 2 vertex (unvisited).  
       *    note: should not be more than one, otherwise current vertex
       *         would have been forced, and would have been pruned down 
       *         to degree 2 as well
       * 2.  select a (random) unvisited vertex
       * 3.  try cycle extend, if set.  (search for a cycle, if found break it)
       * 4.  select a (random) visited vertex (same as normal VISIT_RANDOM)
       */

      /* 1. select a degree 2 neighbour (unvisited) */
      for (loop = 0; loop < graph->deg[curvert]; loop++)
      {
        tempvert = graph->nbr[curvert][loop];
 
        if ( (graph->deg[tempvert] == 2) && 
		(graphpath[tempvert].pathpos == -1) )
        {
          nextvert = tempvert;
          break;
        }
      }

      if (nextvert == -1)	/* no vertex selected */
      {
        /* 2. select an unvisited vertex */
        /* start at random point in list of neighbours, to make selection
         * random 
         */
        tempnum = lrand48() % graph->deg[curvert];
        loop = tempnum;
        do 
        {
          tempvert = graph->nbr[curvert][loop];
          if (graphpath[tempvert].pathpos == -1)
          {
            nextvert = tempvert;
            break;
          }
          else
          {
           loop = (loop+1) % graph->deg[curvert];
          }
        }  while (loop != tempnum);
      }

      /* 3.  try cycle extend option, if set */
      if ((g_options.heur_alg.cycleextendflag == CYCLEEXTEND) && 
		(nextvert == -1))
      {
        /* search for a cycle */
        nextvert = do_cycle_extend(graph, path, graphpath, &pstart, &pend,
			plength);

        /* if returned -1, then couldn't form circle, so continue as normal 
         * (might actually be stuck @@) */
        /* if returned -2, then formed circle, and got stuck, so no
         * chance of expanding this path */
        if (nextvert == -2)
          return(HC_NOT_FOUND);

      }
    }  /* end of smartvisit choose-vertex section */
  

    /* random visit option, or if smart visit never found a better vertex
     * to visit
     */
    if ( (g_options.heur_alg.visitflag == VISIT_RAND) || (nextvert == -1) )
    {
      /* select a vertex, but note that if it was previously visited,
       * we must ensure that do not repeat previous path, so check that
       * length of path and new ending vertex do not correspond to
       * previous setup 
       */
      tempnum = lrand48() % graph->deg[curvert];
      loop = tempnum;
      do 
      {
        tempvert = graph->nbr[curvert][loop];
        if (graphpath[tempvert].pathpos == -1)  /* check if unvisited */
        {
          nextvert = tempvert;
          break;
        }
        
        /* visited vertex - check ended stat:  figure out new path
         * endpoint, based on posa's transformation
         * x = tempvert 
         * graphpath[x].pathpos -> find location in path
         * path[x].next         -> find next element of path
         * path[x].gvert        -> find vertex # of next element in path
         * graphpath[x].ended   -> this is the new end vertex: check ended
         *                          parameter
         */
        /* visited vertex: check that we will not be breaking a forced edge.
         * if next vertex in path is deg 2, then will be breaking a forced
         * edge - don't do that
         * make this check only if smartvisit flag is set
         */
        i = graphpath[tempvert].pathpos;
        i = path[i].next;
        i = path[i].gvert;
        if ( (plength != graphpath[i].ended) &&
             ( ( (g_options.heur_alg.visitflag == VISIT_SMART) &&
	 	 (graph->deg[i] != 2) ) ||
	       (g_options.heur_alg.visitflag == VISIT_RAND) ) )
        {
          nextvert = tempvert;
          break;
        }
        else
        {
         loop = (loop+1) % graph->deg[curvert];
        }
      }  while (loop != tempnum);

    }  /* end of search for next vertex */

    if (nextvert == -1)
    {
      /* cannot seem to add a vertex to the path, so must be stuck -> quit */
      return(HC_NOT_FOUND);
    }

    /* add vertex to path, if it is not in it yet */ 
    if ( (g_options.report_flags & REPORT_ALG) && (VERBOSE_MODE) )
    {
      fprintf(g_options.log_fp, "vertex %d on path = %d\n", plength, nextvert);
      fflush(g_options.log_fp);
    }

    if (graphpath[nextvert].pathpos == -1)
    {
      plength++;
      graphpath[nextvert].pathpos = plength-1;
      graphpath[nextvert].ended = plength;
      path[pend].next = plength-1;
      pend = plength-1;
      path[plength-1].gvert = nextvert;
      path[plength-1].next = -1;
    }
    else
    {
      /* vertex is already in path, so do posa's transformation */
      hc_reverse_path(path, graphpath, &pend,
			graphpath[nextvert].pathpos, plength);
    }

    /* check if have hamilitonian path, and if so, try to find a cycle */
    if (plength == graph->numvert)
    {
      if ( (g_options.report_flags & REPORT_ALG) && (VERBOSE_MODE) )
      {
        fprintf(g_options.log_fp, "  have path, trying for cycle.\n");
        fflush(g_options.log_fp);
      }

      /* try to find a cycle from the path */
      /* normal cycle completion:  just look for an edge between start
       * and end 
       */
      if (g_options.heur_alg.completeflag != COMPLETE_SMART)
      {
        if (check_if_edge(graph, path[pend].gvert, path[pstart].gvert) 
  		== EDGE_EXIST)
        {
          found = 1;
          break;
        }
      }
      else
      {
        /* smart cycle completion: check also if can use posa's
         * transformation to create a cycle.
         */
        if (hc_path_to_cycle(graph, path, graphpath, &pstart, &pend, plength)
             == HC_FOUND)
        {
          found = 1;
        }
      }
    }  /* end of hamiltonian path check */
        
  }  /* end of add-vertex-to-path loop */

  /* have a solution, so record it */
  for (tempnum = pstart, loop = 0; loop < graph->numvert; loop++)
  {
    solution[loop] = path[tempnum].gvert;
    tempnum = path[tempnum].next;
  }

  return(HC_FOUND);

}  /* end of calc_posa_heur_alg() */


/************************************************************/
/* main body of posa algorithm
 *   does initial degree checking/component checking using pruning
 *   runs posa algorithm, 
 *     returns solution (ordering of vertices making up the hamiltonian cycle)
 *     (if solution was found)
 *
 *   returns HC_FOUND if HC found, HC_NOT_EXIST if a cycle cannot possibly
 *     exist on this graph, and HC_NOT_FOUND if a cycle was not found 
 */
int 
master_heuristic_alg(
  graph_type *graph,
  stat_pertrial_type *trialstats,
  int solution[])
{
  int ret;
  int loop;
  int nodecount = 0;
  int prune = 0;
  graph_type testgraph;

  /* update statistics */
  trialstats->edgeprune = 0;
  trialstats->nodes = 0;
  trialstats->retries = 0;

  /* copy passed in graph to second graph data structure 
   * so that original graph isn't overwritten when pruning, etc.
   */
  copy_graph(graph, &testgraph);

  /* do initial H.C. pruning, and run tests to see if H.C. can exist */
  ret = hc_do_pruning(&testgraph, &prune, HC_PRUNE_ALL, NULL);

  /* update reduce statistic */
  trialstats->edgeprune = prune;

  if (ret == HC_NOT_EXIST)
  {
    return(HC_NOT_EXIST);
  }
  /* if forced HC was found, then just let posa's find it quickly (to
   * construct the solution)
   */

  /* loop through each vertex in graph, using each one in turn as the
   * starting vertex
   */
  for (loop = 0; loop < testgraph.numvert; loop++) 
  {
    if ( (g_options.report_flags & REPORT_ALG) && (VERBOSE_MODE) )
    {
      fprintf(g_options.log_fp, "Running Posa's on graph using starting "
				"vertex = %d \n", loop);
      fflush(g_options.log_fp);
    }
    
    if (g_options.algorithm == ALG_POSA_HEUR)
    {
      ret = calc_posa_heur_alg(&testgraph, loop, solution, &nodecount);
    }

    if (ret == HC_FOUND)
      break;
    if (ret == HC_NOT_EXIST)
      break;
  }

  /* update statistics */
  trialstats->retries = loop;
  trialstats->nodes = nodecount;

  return(ret);

}  /* end of master_heuristic_alg() */


/************************************************************/
/* heuristic algorithm option routines, for handling the options
 * associated with the posa algorithm
 */
/************************************************************/

/************************************************************/
/* this function parses the option file for parameters for using
 * posa's algorithm.
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
 *  +smartvisit  : use smarts to decide upon next vertex to visit
 *  +smartcomplete : use smarts to convert hamiltonian line to a cycle.
 *  +cycleextend  : use cycleextend technique
 *     this requires and thus sets the  +smartvisit and +smartcomplete options
 *
 */
int 
parse_heuristic_alg_options(
  FILE *fp,
  char word[])
{
  int status;

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

      /* visitflag parameter */
      if (strcasecmp(parmstr, "+smartvisit")==0) 
      {
        g_options.heur_alg.visitflag = VISIT_SMART;
      }

      /* completeflag parameter */
      else if (strcasecmp(parmstr, "+smartcomplete")==0) 
      {
        g_options.heur_alg.completeflag = COMPLETE_SMART;
      }

      /* cycleextend parameter */
      else if (strcasecmp(parmstr, "+cycleextend")==0) 
      {
        g_options.heur_alg.cycleextendflag = CYCLEEXTEND;
        /* set other options required by cycleextend */
        g_options.heur_alg.completeflag = COMPLETE_SMART;
        g_options.heur_alg.visitflag = VISIT_SMART;
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

}  /* end of parse_heuristic_alg_options() */


/************************************************************/
/* this function initializes the posa alg options
 * default values are set to those of basic posa.
 * @@ could implement more efficient/elegent option system
 *    store in 1 int variable using 1-bit flags.
 */
void
init_heuristic_alg_options()
{
  g_options.heur_alg.completeflag = COMPLETE_NORM;
  g_options.heur_alg.visitflag = VISIT_RAND;
  g_options.heur_alg.cycleextendflag = NOCYCLEEXTEND;

}  /* end of init_heuristic_alg_options() */



/************************************************************/
/* this function initializes the posa alg options
 * default values are set
 */
void
print_heuristic_alg_options(
  FILE *fp)
{
  fprintf(fp, "    visit next vertex flag = ");
  if (g_options.heur_alg.visitflag == VISIT_RAND)
    fprintf(fp, "randomly\n");
  else if (g_options.heur_alg.visitflag == VISIT_SMART)
    fprintf(fp, "intelligently\n");

  fprintf(fp, "    complete cycle from line flag = ");
  if (g_options.heur_alg.completeflag == COMPLETE_NORM)
    fprintf(fp, "simply\n");
  else if (g_options.heur_alg.completeflag == COMPLETE_SMART)
    fprintf(fp, "intelligently\n");

  fprintf(fp, "    cycle extension technique = ");
  if (g_options.heur_alg.cycleextendflag == CYCLEEXTEND)
    fprintf(fp, "yes\n");
  else if (g_options.heur_alg.cycleextendflag == NOCYCLEEXTEND)
    fprintf(fp, "no\n");

}  /* end of print_heuristic_alg_options() */

