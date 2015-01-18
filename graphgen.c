
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: graphgen.c
 * Contents: graph generation code (for different graphs)
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


/************************************************************/
/* utility functions */
/************************************************************/

/************************************************************/
/* this function puts the given list into a random ordering
 * (by randomly swapping elements)
 */
void
random_order_list(
  int list[],
  int length)
{
  int loop;
  int switchindex, switchval;

  for (loop = 0; loop < length - 1; loop ++)
  {
    switchindex = loop +  ( (int) ( lrand48() % (length - loop) ) );
    switchval = list[loop];
    list[loop] = list[switchindex];
    list[switchindex] = switchval;
  }

}  /* end of random_order_list() */


/************************************************************/
/* this function randomly permutes a graph (scrambles the vertices)
 * assumes graph is properly set up
 * assumes graph is undirected
 */
void
random_permute_graph(
  graph_type *graph)
{
  graph_type new_graph;

  int vloop;
  int eloop;
  int new_vertices[MAXVERT];
  int new_first;
  int new_second;

  /* copy old graph into new to preserve other information like # of vertices */
  copy_graph(graph, &new_graph);

  /* initialize the new graph's vertex and edge data */
  for (vloop = 0; vloop < new_graph.numvert; vloop++)
  {
    for (eloop = 0; eloop < new_graph.deg[vloop]; eloop++)
    {
      new_graph.nbr[vloop][eloop] = 0;
    }
    new_graph.deg[vloop] = 0;
  }
  
  /* create the new vertex ordering */
  for (vloop = 0; vloop < new_graph.numvert; vloop++)
  {
    new_vertices[vloop] = vloop;
  }
  random_order_list(new_vertices, new_graph.numvert);

  /* for each edge in the old graph, add the equivalent edge in the new graph */
  for (vloop = 0; vloop < graph->numvert; vloop++)
  {
    new_first = new_vertices[vloop]; 
    for (eloop = 0; eloop < graph->deg[vloop]; eloop++)
    {
      new_second = new_vertices[graph->nbr[vloop][eloop]];
      if (check_if_edge(&new_graph, new_first, new_second) == EDGE_NOTEXIST)
      {
        add_edge_graph(&new_graph, new_first, new_second);
      }
    }  /* end of loop through neighbours of current vertex */
  }  /* end of loop through vertices */

  /* copy new graph back into old */
  copy_graph(&new_graph, graph);

}  /* end of random_permute_graph() */


/************************************************************/
/* ICCS graph generator */
/************************************************************/

/************************************************************/
/* this function calculates the vertex # (in a graph data structure)
 * of the first vertex of the specified subgraph
 * note that first subgraph is #0
 */
int
calc_iccs_subgraph_index(
  int subgraph,
  int isize)
{
  return ( (isize * 2 + 2) * subgraph );

}  /* end of calc_iccs_subgraph_index() */


/************************************************************/
/* this function generates an ICCS subgraph
 *
 * sv = start vertex in graph to locate subgraph
 * isize = # of vertices in independent set
 *
 * labelling of vertices in subgraph
 * sv = degree 2 forced vertex (connects to sv+1, the one S_T vertex)
 * sv+1, sv+2 = S_T vertices (connecting vertices for cycle)
 * sv+3 = special cutset vertex for minimum degree heuristic
 *        has edges to sv+1, sv+2, sv+isize+3, sv+isize+4
 * sv+3 to sv+isize+1 = S_C vertices (cutset vertices)
 * sv+isize+2 to sv+2*isize+1 = S_I vertices (independent set vertices)
 */
void
gen_subgraph_iccs(
  graph_type *graph,
  int sv,
  int isize)
{
  int iloop, cloop;

  /* connect edge from degree 2 vertex to S_T vertex */
  add_edge_graph(graph, sv+0, sv+1);
  
  /* connect edges from independent set to cutset */
  for (iloop = 0; iloop < isize; iloop++)
  {
    for (cloop = 1; cloop < isize-1; cloop++)
    {
      add_edge_graph(graph, sv+isize+2+iloop, sv+3+cloop); 
    }
  }

  /* connect sv+3 vertex to independent set */
  add_edge_graph(graph, sv+3, sv+isize+3);
  add_edge_graph(graph, sv+3, sv+isize+4);

  /* connect edges from S_T vertices to independent set */
  add_edge_graph(graph, sv+1, sv+isize+2);
  add_edge_graph(graph, sv+2, sv+2*isize+1);
   
  /* connect edges from S_T vertices to cutset */
  add_edge_graph(graph, sv+1, sv+3);
  add_edge_graph(graph, sv+2, sv+3);

}  /* end of gen_subgraph_iccs() */


/************************************************************/
/* this function connects the subgraphs of the ICCS graph
 * with non-Hamiltonian edges by connecting vertices
 * in the cutset
 *
 * note that the first vertex in the cutset (offset+3) is not connected
 * to any other subgraphs
 */
void
gen_iccs_nonham_connect(
  graph_type *graph,
  int subgraphs, 
  int indsetsize)
{
  int sloop, cloop;
  int vert1;
  int sg2, cs2, vert2;

  if (subgraphs == 1)
    return;

  /* loop through the subgraphs */
  for (sloop = 0; sloop < subgraphs; sloop++)
  {
    /* loop through cutset vertices */
    for (cloop = 1; cloop < indsetsize-1; cloop++)
    {
      vert1 = calc_iccs_subgraph_index(sloop,indsetsize) + 3 + cloop;

      /* get a random vertex in a different subgraph */

      /* get random subgraph different from current one */
      do
      {
        sg2 = (int) (lrand48() % subgraphs);
      } while (sg2 == sloop);

      /* get random vertex in cutset, excluding sv+3 vertex */
      cs2 = (int) (lrand48() % (indsetsize-2) );

      vert2 = calc_iccs_subgraph_index(sg2,indsetsize) + 4 + cs2;

      /* don't add the edge if it already exists */
      if (check_if_edge(graph, vert1, vert2) == EDGE_NOTEXIST)
      {
        add_edge_graph(graph, vert1, vert2);
      }
 
    }  /* end of cutset for loop */
  }  /* end of subgraph for loop */

}  /* end of gen_iccs_nonham_connect() */


/************************************************************/
/* this function generates an interconnected-cutset (ICCS) graph
 * which has a number of subgraphs equal to <subgraphs> and the size of
 * the independent set in each subgraph is <indsetsize>
 *
 * the graph is designed to be hard for any possible variation of the 
 * backtrack algorithm (any degree sorting heuristic) and any level
 * of component checking or cutpoint checking
 *
 */
void
gen_iccs_graph(
  graph_type *graph,
  int subgraphs, 
  int indsetsize)
{
  int sloop;
  int sgindex;

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, "Generating an ICCS graph with");
    fprintf(g_options.log_fp, " %d subgraphs ", subgraphs); 
    fprintf(g_options.log_fp, "and independent sets of size %d.\n", indsetsize);
  }

  /* init graph */
  init_graph(graph);

  /* calculate # of vertices */
  graph->numvert = 2 * subgraphs * (indsetsize + 1);
  g_options.graphgen.nvertex = graph->numvert;

  /* create subgraphs, and connect primary edges */
  for (sloop = 0; sloop < subgraphs; sloop++)
  {
    sgindex = calc_iccs_subgraph_index(sloop, indsetsize);
    gen_subgraph_iccs(graph, sgindex, indsetsize);

    if (sloop != 0)
    {
      add_edge_graph(graph, sgindex, 
	calc_iccs_subgraph_index(sloop-1,indsetsize)+2);
    }
    else
    {
      add_edge_graph(graph, sgindex, 
	calc_iccs_subgraph_index(subgraphs-1,indsetsize)+2);
    }
  }
 
  /* connect nonHamiltonian edges between cutsets of different subgraphs */
  gen_iccs_nonham_connect(graph, subgraphs, indsetsize);

  random_permute_graph(graph);

}  /* end of gen_iccs_graph() */


/************************************************************/
/* addpath graph generator */
/************************************************************/

/************************************************************/
/* this function generates an addpath graph with <nvertex> vertices,
 * and paths of length <pathlen1> and <pathlen2>
 * note that a length of 1.0 specifies a Hamiltonian path, not a
 * Hamiltonian cycle
 */
void 
gen_addpath_graph(
  graph_type *graph,
  int nvertex,
  int numpaths,
  float pathlen[])
{
  int pathloop;
  int pathsize;
  int pathvert[MAXVERT];
  int pathindex;

  int lowdeglist[MAXVERT], deg2list[MAXVERT], highdeglist[MAXVERT];
  int lowdegcount, deg2count, highdegcount;
  int lowdeguse, deg2use, highdeguse;

  int loop;

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, "Generating an addpath graph of %d vertices ", 
	nvertex);
    fprintf(g_options.log_fp, "and paths of length: ");
    for (pathloop = 0; pathloop < numpaths; pathloop++)
    {  
      fprintf(g_options.log_fp, "%.2f ", pathlen[pathloop]);
    }
    fprintf(g_options.log_fp,"\n");
  }

  /* init graph */
  init_graph(graph);

  graph->numvert = nvertex;

  /* loop through the number of paths */
  for (pathloop = 0; pathloop < numpaths; pathloop++)
  {
    pathsize = pathlen[pathloop] * graph->numvert;

    /* degree sort:  divide vertices into three lists
     * lowdeglist = degree 0 and 1 vertices
     * deg2list   = degree 2 vertices
     * highdeglist = degree 3 or more vertices
     */
    lowdegcount = deg2count = highdegcount = 0;
    for (loop = 0; loop < graph->numvert; loop++)
    {
      if (graph->deg[loop] < 2)
        lowdeglist[lowdegcount++] = loop;
      else if (graph->deg[loop] == 2)
        deg2list[deg2count++] = loop;
      else
        highdeglist[highdegcount++] = loop;
    }

    /* randomly order each of the lists */
    random_order_list(lowdeglist, lowdegcount);
    random_order_list(deg2list, deg2count);
    random_order_list(highdeglist, highdegcount);

    /* determine how many deg2 vertices or highdeg vertices will 
     * be used in the path
     */
    lowdeguse = deg2use = highdeguse = 0;
    lowdeguse = lowdegcount;
    if (pathsize <= lowdegcount)
      lowdeguse = pathsize;
    else
    {
      if (deg2count >= pathsize - lowdeguse)
      {
        deg2use = pathsize - lowdeguse;
      }
      else
      {
        deg2use = deg2count;
        highdeguse = pathsize - lowdeguse - deg2use;
      }
    }

    /* build collection of vertices in path */
    pathindex = 0;
    for (loop = 0; loop < lowdeguse; loop++)
      pathvert[pathindex++] = lowdeglist[loop];

    for (loop = 0; loop < deg2use; loop++)
      pathvert[pathindex++] = deg2list[loop];
    
    for (loop = 0; loop < highdeguse; loop++)
      pathvert[pathindex++] = highdeglist[loop];

    if (pathindex != pathsize)
    {
      EXIT_ERROR("Error in path length in gen_addpath_graph().");
    }
    
    /* get new random ordering of the vertices */
    random_order_list(pathvert, pathsize);

    /* add the corresponding edges to the graph */
    for (loop = 0; loop < pathsize-1; loop++)
    {
      /* only add edge if it doesn't already exist */
      if (check_if_edge(graph, pathvert[loop], pathvert[loop+1])
		== EDGE_NOTEXIST)
      {
        add_edge_graph(graph, pathvert[loop], pathvert[loop+1]);
      }
    }
      
  }  /* end of for loop for adding paths */

  random_permute_graph(graph);

}  /* end of gen_addpath_graph() */


/************************************************************/
/* addcycle graph generator */
/************************************************************/

/************************************************************/
/* this function adds <ncycle> cycles to the specified graph 
 * while used for the add-cycle graph generator, it can also
 * be used to add a Hamiltonian Cycle to an existing graph
 */
void
add_cycles_to_graph(
  graph_type *graph,
  float ncycles)
{
  int vloop;
  int cloop;
  int maxcycles;
  int fractcycle;
  int tempcycle[MAXVERT];

  maxcycles = (int) (ncycles);
  fractcycle = 0;
  if (ncycles - maxcycles > 0.0)
  {
    ncycles += 0.00001;
    /* calculate # of vertices remaining in fractional component */
    fractcycle = (int) ( (ncycles - maxcycles) * (float) (graph->numvert) );
    if (fractcycle < 2)  /* need at least 2 vertices to add an edge */
    {
      maxcycles--;
      fractcycle = 0;
    }

    /* need to calculate last, incomplete cycle */
    maxcycles++;
  }
  
  /* loop through the number of cycles to add (round up) */
  for (cloop = 1; cloop <= maxcycles; cloop++)
  {
    /* get a random permutation of the vertices */
    for (vloop = 0; vloop < graph->numvert; vloop++)
      tempcycle[vloop] = vloop;

    random_order_list(tempcycle, graph->numvert);

    /* now add an edge for each vertex of the cycle
     * if last cycle then
     *   if fractcycle > 0, then only add edges up to vertex fractcycle 
     *   (if fractcycle = numvert, then add a hamiltonian path)
     *   if fractcycle == 0, then add the entire cycle
     */
    for (vloop = 1; vloop < graph->numvert; vloop++)
    {
      /* only add edge if it doesn't already exist */
      if (check_if_edge(graph, tempcycle[vloop-1], tempcycle[vloop]) 
		== EDGE_NOTEXIST)
      {
        add_edge_graph(graph, tempcycle[vloop-1], tempcycle[vloop]);
      }
      
      if ( (cloop == maxcycles) && (fractcycle == vloop) )
        break;
    }
    
    /* add last edge forming the cycle. */
    if ( (fractcycle == 0) || (cloop < maxcycles) )
    {
      if (check_if_edge(graph, tempcycle[0], tempcycle[(graph->numvert)-1]) 
		== EDGE_NOTEXIST)
        add_edge_graph(graph, tempcycle[0], tempcycle[(graph->numvert)-1]);
    }
   
  }  /* end of loop through cycles */

}  /* end of add_cycles_to_graph() */


/************************************************************/
/* this function generates an add-cycle graph with <nvertex> vertices
 * and <ncycle> cycles placed in the graph
 *
 * note that this function actually adds <ncycle> cycles to an existing
 * graph, so it can be used to build a cycle into a pre-existing graph.
 */
void 
gen_addcycle_graph(
  graph_type *graph,
  int nvertex,
  float ncycles)
{
  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, 
	"Generating an addcycle graph of %d vertices and %.2f cycles.\n", 
	nvertex, ncycles);
  }

  /* init graph */
  init_graph(graph);

  graph->numvert = nvertex;

  add_cycles_to_graph(graph, ncycles);

  random_permute_graph(graph);

}  /* end of gen_addcycle_graph() */


/************************************************************/
/* random graph generator */
/************************************************************/

/************************************************************/
/* this function generates a random graph with <nvertex> vertices
 * and <nedges> edges
 */
void 
gen_random_graph(
  graph_type *graph,
  int nvertex,
  int nedges)
{
  int loop;
  int v1, v2;

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, 
	"Generating a random graph of %d vertices and %d edges.\n", 
	nvertex, nedges);
  }

  /* init graph */
  init_graph(graph);

  graph->numvert = nvertex;

  for (loop = 0; loop < nedges; loop++)
  {
    do  /* select a random edge that does not exist yet */
    {
      do  /* generate a different endpoint */
      {
        v1 = (lrand48() % nvertex);  /* generate one endpoint */
        v2 = (lrand48() % nvertex);
      } while (v2 == v1);
    } while (check_if_edge(graph, v1, v2) == EDGE_EXIST);

    /* add the edge */
    add_edge_graph(graph, v1, v2); 
  }

}  /* end of gen_random_graph() */


/************************************************************/
/* crossroads graph generator */
/************************************************************/

/************************************************************/
/* this function generates a crossroads subgraph starting
 * with vertex # <startvertex> in the graph data structure
 * sv = startvertex
 */
void 
gen_subgraph_crossroads(
  graph_type *graph,
  int sv)
{
  add_edge_graph(graph, sv+0, sv+1);
  add_edge_graph(graph, sv+0, sv+2);
  add_edge_graph(graph, sv+1, sv+3);
  add_edge_graph(graph, sv+1, sv+6);
  add_edge_graph(graph, sv+2, sv+3);
  add_edge_graph(graph, sv+2, sv+4);
  add_edge_graph(graph, sv+2, sv+5);
  add_edge_graph(graph, sv+3, sv+4);
  add_edge_graph(graph, sv+3, sv+5);
  add_edge_graph(graph, sv+4, sv+5);

}  /* end of gen_subgraph_crossroads() */


/************************************************************/
/* this function generates a crossroads graph 
 * with the specified number of subgraphs
 */
void 
gen_crossroads_graph(
  graph_type *graph,
  int subgraphs)
{
  int loop;

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, "Generating a %d subgraph crossroads graph.\n",
	subgraphs);
  }

  /* init graph */
  init_graph(graph);

  /* calculate # of vertices */
  graph->numvert = subgraphs * 7;
  g_options.graphgen.nvertex = graph->numvert;
 
  /* loop through generation of subgraphs */
  for (loop = 0; loop < 7*subgraphs; loop+=7)
  {
    /* generate subgraph */
    gen_subgraph_crossroads(graph, loop);

    /* connect subgraphs */
    if (loop != 0)
      add_edge_graph(graph, loop-1, loop);
  } 

  /* connect last subgraph to first */
  add_edge_graph(graph, subgraphs*7-1, 0); 

  random_permute_graph(graph);
    
}  /* end of gen_crossroads_graph() */


/************************************************************/
/* generalized knight's circuit graph generator */
/************************************************************/

/************************************************************/
/* this function adds an edge from the source cell to the destination
 * cell if they lie within the board 
 *
 * assume 2-dimensional board, with a 2-dimension knights move
 *
 */
void 
add_knighttour_edge(
  graph_type *graph,
  int boardx, 
  int boardy,
  int sourcex,
  int sourcey,
  int destx,
  int desty)
{
  int sourcevert, destvert;

  /* check to see if source and destination cells lie within the board */
  if ( (sourcex < 0) || (sourcex >= boardx) ||
       (sourcey < 0) || (sourcey >= boardy) ||
       (destx < 0) || (destx >= boardx) ||
       (desty < 0) || (desty >= boardy) )
    return;

  sourcevert = sourcex + sourcey * boardx;  
  destvert = destx + desty * boardx;
  
  /* create directed edge */
  add_direct_edge_graph(graph, sourcevert, destvert);

}  /* end of add_knighttour_edge() */
    

/************************************************************/
/* this function is the interface function for generating and 
 * returning a knighttour graph 
 *
 * assume a 2-d board, with a 2-d knights move
 *
 */
void 
gen_knighttour_graph(
  graph_type *graph,
  int boardx, 
  int boardy,
  int move1, 
  int move2)
{
  int loop;
  int curx, cury;

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, "Generating a knighttour graph on a "
	"%d, %d board.\n", boardx, boardy);
  }

  /* init graph */
  init_graph(graph);

  /* calculate # of cells on board = # of vertices in graph */
  graph->numvert = boardx * boardy;
  g_options.graphgen.nvertex = graph->numvert;
 
  if (graph->numvert > MAXVERT)
  {
    EXIT_ERROR("knighttour graph generation exceeded max # of vertices.\n");
  }

  /* loop through cells of board, add edges going away from current cell
   * only, to avoid duplicate edges
   */
  curx = cury = 0;
  
  for (curx = -1, loop = 0; loop < graph->numvert; loop++)
  {
    /* increment board position */
    curx ++;
    if (curx >= boardx)
    {
      curx = 0;
      cury++;
    }

    /* add edges based on where knight can move to from current cell 
     * (don't worry about illegal moves)
     */

      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx+move1, cury+move2);
      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx+move2, cury+move1);

      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx+move1, cury-move2);
      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx+move2, cury-move1);

      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx-move1, cury+move2);
      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx-move2, cury+move1);

      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx-move1, cury-move2);
      add_knighttour_edge(graph, boardx, boardy, curx, cury, 
	curx-move2, cury-move1);
    
  }  /* end of main loop through board positions */

  random_permute_graph(graph);

}  /* end of gen_knighttour_graph() */


/************************************************************/
/* Degreebound graph generator */
/************************************************************/

/************************************************************/
/* this function swaps the values of the 2 entries in the specified
 * (integer) stack
 */
void
stackswap(
  int p1,
  int p2,
  int stack[])
{
  int temp;

  temp = stack[p1];
  stack[p1] = stack[p2];
  stack[p2] = temp;

}  /* end of stackswap() */


/************************************************************/
/* this function generates a random graph, with the degree of each
 * vertex set to the specified degreee (if possible)
 *
 * maxdeg[] -> array of desired degrees for each vertex
 *             note: summation of desired degrees must be even
 *             
 * rarely, the function will be unable to achieve the desired degree
 *   sequence.  rather than coding the few exception cases in (which 
 *   would be very difficult, perhaps impossible), the generator detects
 *   when it has taken too long to completely generate a graph, and returns
 *   GEN_FAILURE.  Otherwise, normally, it will return GEN_OKAY
 */
#define GEN_OKAY 1
#define GEN_FAILURE 0
int
generate_fixed_degree_graph(
  graph_type *graph,
  int numvert,
  int maxdeg[])
{

#define DEGREEVERSION 1 
#if DEGREEVERSION == 1
   /* array to hold the vertices which still have free valence */
   int vfreeval[MAXVERT];

   /* number of vertices with free valence remaining */
   int numfreeval;

   /* freevalence remaining */
   int remvalence[MAXVERT];

   /* when we switch to a list of possible vertex pairs, store pair set here */
   struct vpair_struct {
	int x,y;
   };

   int numpairrem;
   struct vpair_struct *pairset;

  int tempv,tempw;
  int v,w;




  init_graph(graph);

  graph->numvert = numvert;

  numpairrem = 0;
  for(v=0;v< graph->numvert; v++)
  {
	vfreeval[v] = v;
	remvalence[v] = maxdeg[v];
	if (maxdeg[v] > numpairrem) numpairrem = maxdeg[v];
  }
  /* when numfreeval is reduced to twice maxdegree, then use a brute force
     completion to detect failure  easily */
  numpairrem *= 2;

  numfreeval = graph->numvert;

  while (numfreeval > numpairrem ) 
  {
	tempv = ( lrand48() % numfreeval);
	tempw = ( lrand48() % numfreeval);
	if (tempv != tempw) 
	{
	   v = vfreeval[tempv];
	   w = vfreeval[tempw];
	   if (EDGE_NOTEXIST == check_if_edge(graph,v,w) ) 
	   {
		add_edge_graph(graph,v,w);
		remvalence[v]--;
		if (0 == remvalence[v]) 
		{
			numfreeval--;
			vfreeval[tempv] = vfreeval[numfreeval];
			if (vfreeval[tempv] == w) tempw = tempv;
		}
		remvalence[w]--;
		if (0 == remvalence[w]) 
		{
			numfreeval--;
			vfreeval[tempw] = vfreeval[numfreeval];
		}
		if ((remvalence[v]<0) || (remvalence[w] <0))
		 fprintf(stderr,"freevalences are %d,%d rem=%d\n",
			remvalence[v],remvalence[w], numfreeval);
      	   }
	}
   }

   /* complete the rest by setting up all possible pairs of remaining vertices */
   pairset = (struct vpair_struct *) 
	calloc(numpairrem*(numpairrem-1), sizeof(struct vpair_struct) ); 

   numpairrem = 0;
   for(v=0;v<numfreeval-1;v++)
    for(w=v+1;w<numfreeval;w++)
    {
	pairset[numpairrem].x = vfreeval[v];
	pairset[numpairrem].y = vfreeval[w];
        numpairrem++;
    }
        
   while ( (numpairrem >0 ) && (numfreeval > 0) )
   {
	tempv = (lrand48() % numpairrem );
	v = pairset[tempv].x;
	w = pairset[tempv].y;
        if (  (remvalence[v] >0) 
	   && (remvalence[w] >0) 
	   && (EDGE_NOTEXIST == check_if_edge(graph,v,w) )
	   )
        {
             add_edge_graph(graph,v,w);
             remvalence[v]--;
             if (0 == remvalence[v])
             {
                     numfreeval--;
             }
             remvalence[w]--;
             if (0 == remvalence[w])
             {
                     numfreeval--;
             }
        }

	numpairrem--;
	pairset[tempv] = pairset[numpairrem];
   }


   free(pairset);
   

   if (numfreeval > 0) 
   {
	/* fprintf(stderr,"RETURNING FAILURE\n"); */
	return(GEN_FAILURE);
   }
   else
   {
	/* fprintf(stderr,"RETURNING OKAY\n"); */
	return(GEN_OKAY);
   }

} /* end DEGREEVERSION 1 generate_fixed_degree */
   
   
#endif

#if DEGREEVERSION == 2
   /* 
	This version creates a vector containg each vertex v
	d[v] times. It randomly selects a pair from the vector,
	and if not equal and not already an edge, then adds the
	edge to the graph and deletes the pair from the vector.
	The process fails and returns GEN_FAILURE if more than MAXFAILS
	failures occurs while trying to select a pair.
	If completed then returns GEN_OKAY.
   */
#define MAXFAILS 100

	int i,j,k; /* loop indices */
	int totaldegree, numremaining;
	int n1,n2;

	int *vvector;

	int numfails;

	int v,w; 
   

	totaldegree = 0;
	for(i=0;i<numvert;i++)
		totaldegree += maxdeg[i];
	
	vvector = (int *) calloc(totaldegree,sizeof(int));

	k = 0;
	for (i=0;i<numvert;i++) {
		for(j=0;j<maxdeg[i];j++) {
			vvector[k] = i;
			k++;
		}
	}

  	init_graph(graph);
  	graph->numvert = numvert;
		
	numremaining = k;
	numfails = 0;
	while (numremaining > 2) 
	{
		n1 = numremaining -1;
		n2 =n1 -1;
		v = lrand48() % numremaining;
		stackswap(v, n1, vvector);
		w = lrand48() % n1;
                stackswap(w, n2, vvector);
		v = vvector[n1];
		w = vvector[n2];
		if (v == w)
			numfails++;
		else if (EDGE_NOTEXIST == check_if_edge(graph,v,w) ) 
		{
			numfails = 0;
			add_edge_graph(graph,v,w);
			numremaining = n2;
		}
		else
			numfails++;
		if (numfails >= MAXFAILS)
		{
			free(vvector);
			/* fprintf(stderr,"FAILURE\n"); */
			return GEN_FAILURE;
		}
	}
	v = vvector[0];
	w = vvector[1];
	if ( (v==w) || ( EDGE_EXIST == check_if_edge(graph,v,w) ) )
	{
		free(vvector);
		/* fprintf(stderr,"FAILURE\n"); */
		return GEN_FAILURE;
	}
		
	add_edge_graph(graph,v,w);
	free(vvector);
	/* fprintf(stderr,"OKAY\n"); */
	return GEN_OKAY;
}

	

#endif /* DEGREEVERSION 2 */

#if DEGREEVERSION == 3
  /*
	This version uses random edge selection, as in version 1,
	but attempts to do fixups when failure is first detected. 
	Likely a bad idea for ur purposes after all.
  */
	
  /* array/stack to hold list of vertices in the graph
   * different stack pointers (_sp) point to bottom of their portion
   * of the stack
   */
  int vstack[MAXVERT];		

  /* points to bottom full vertex (degree is at specified amount)
   * or if no such vertices, points to 1 above the top of the stack
   */
  int donevert_sp;	

  /* points to vertex currently having edges added to it */
  int curvert_sp;

  /* points to bottom of the nbrs of current vertex, or points to 
   * curvert_sp if no such nbrs
   */
  int nbrvert_sp;

  int numtrys;
  int maxtrys;

  int curvert;
  int temp;
  int nbrvert;
  int dloop, nloop, loop;
  
  int v1_sp, v2_sp;
  int v1, v2;

  /* initialize graph and stack */
  init_graph(graph);

  graph->numvert = numvert;
  for (loop = 0; loop < graph->numvert; loop++)
  {
    vstack[loop] = loop;
  }
  donevert_sp = graph->numvert;


  /* start of main loop:  add edges, one by one, to vertices whose
   * degrees are less than their desired degrees
   */
  while (donevert_sp > 0)	/* while unfinished vertices remain */
  {
    /* select random, none-full vertex, swap to top of not-done stack
     * portion
     */
    temp = (lrand48() % donevert_sp);	/* between 0 and donevert_sp-1 */
    curvert = vstack[temp];
    curvert_sp = donevert_sp - 1;
    stackswap(temp, curvert_sp, vstack);
  
    /* make next section of stack hold neighbours of curvert */
    nbrvert_sp = curvert_sp;
    for (dloop = 0; dloop < graph->deg[curvert]; dloop++)
    {
      /* stop checking for neighbours if all the remaining vertices are 
       * neighbours of curvert
       */
      if (nbrvert_sp > 0)	
      {
        for (nloop = 0; nloop < nbrvert_sp; nloop++)
        {
          if (graph->nbr[curvert][dloop] == vstack[nloop])
          {
            nbrvert_sp --;
            stackswap(nloop, nbrvert_sp, vstack);
          }
        }
      }
      else
        break;
    }
     
    /* if there are no vertices to add an edge to from curvert, because
     * all the vertices are either neighbours, full, or both, then use
     * edge transformation trick 
     */
    if (nbrvert_sp <= 0) 
    {
      if ( (maxdeg[curvert] - graph->deg[curvert]) >= 2 )
      {
        /* need to find 2 non-neighbours of curvert that are joined by an
         * edge.  Cut that edge, and instead add an edge from each to
         * curvert.  The other vertice's degrees don't change, while
         * curvert's increases by 2.
         */
        
        /* randomly select 2 vertices from full-degree portion of stack
         * (donevert_sp to top of stack), and ensure that they are not 
         * neighbours of curvert, and are also joined by an edge.  
         * first calculate maximum # of times this random search should be
         * attempted - should be high enough to pick out a single pair in a
         * graph
         */

        /* approximate # of possible pairs of vertices */
        maxtrys = (graph->numvert - donevert_sp) * 
			(graph->numvert - donevert_sp);
        maxtrys = maxtrys * 5 + 10;
        numtrys = 0;

        do
        { 
          numtrys++;

          v1_sp = ( lrand48() % (graph->numvert - donevert_sp) );
          v1_sp += donevert_sp;
          v1 = vstack[v1_sp];

          v2_sp = ( lrand48() % (graph->numvert - donevert_sp) );
          v2_sp += donevert_sp;
          v2 = vstack[v2_sp];

        } while ( (numtrys < maxtrys) && ( (v2 == v1) || 
                  (check_if_edge(graph, v1, curvert) == EDGE_EXIST) ||
                  (check_if_edge(graph, v2, curvert) == EDGE_EXIST) ||
                  (check_if_edge(graph, v1, v2) == EDGE_NOTEXIST) ) );

        if (numtrys >= maxtrys)
        {
          return(GEN_FAILURE);  /* give up trying to generate last edges */
        }

        /* now do the transformation */
        rm_edge_graph(graph, v1, v2);
        add_edge_graph(graph, curvert, v1);
        add_edge_graph(graph, curvert, v2);

        /* check to see if curvert has reached desired degree */
        if (graph->deg[curvert] == maxdeg[curvert])
        {
          donevert_sp --;
        }
      }
      else
      {
        /* want to increase degree of curvert by only 1, so use different
         * transformation:
         * find non-full neighbour of curvert (NV) that has an edge to a full
         * non-neighbour (FV).  curvert = CV.  
         * edges before: (CV, NV); (NV, FV)
         * edges after: (CV, NV); (CV, FV)
         * note that deg[CV] += 1, deg[FV] = nochange, deg[NV] += -1
         * first calculate maximum # of times this random search should be
         * attempted - should be high enough to pick out a single pair in a
         * graph
         */ 

        /* approximate # of possible pairs of vertices */
        maxtrys = (curvert_sp - nbrvert_sp) * (graph->numvert - donevert_sp);
        maxtrys = maxtrys * 5 + 10;
        numtrys = 0;

        /* randomly select a non-full neighbour, and a full non-neighbour, 
         * connected by an edge
         */
        do
        { 
          numtrys++;

          /* select neighbour */
          v1_sp = ( lrand48() % (curvert_sp - nbrvert_sp) );
          v1_sp += nbrvert_sp;
          v1 = vstack[v1_sp];

          /* select full vertex */
          v2_sp = ( lrand48() % (graph->numvert - donevert_sp) );
          v2_sp += donevert_sp;
          v2 = vstack[v2_sp];

        } while ( (numtrys < maxtrys) && ( (v2 == v1) || 
                  (check_if_edge(graph, v2, curvert) == EDGE_EXIST) ||
                  (check_if_edge(graph, v1, v2) == EDGE_NOTEXIST) ) );

        if (numtrys >= maxtrys)
        {
          return(GEN_FAILURE);  /* give up trying to generate last edges */
        }

        /* do the transformation */
        rm_edge_graph(graph, v1, v2);
        add_edge_graph(graph, curvert, v2);

        /* v1 was not full, and it lost an edge, so it remains none full 
         * curvert however should have been filled up, so check for this
         */
        if (graph->deg[curvert] == maxdeg[curvert])
        {
          donevert_sp --;
        }
      }
    }
    else
    {
      /* randomly select a non-neighbour of curvert, and add an edge 
       * between the two 
       */
      temp = (lrand48() % nbrvert_sp);
      nbrvert = vstack[temp];
      add_edge_graph(graph, curvert, nbrvert);

      /* check to see if either vertex has now reached desired degree */
      if (graph->deg[curvert] == maxdeg[curvert])
      {
        donevert_sp --;
      }

      if (graph->deg[nbrvert] == maxdeg[nbrvert])
      {
        donevert_sp --;
        stackswap(temp, donevert_sp, vstack); 
      }
    }
 
  }  /* end of main add-edge while loop */

  return(GEN_OKAY);

}  /* end of generate_fixed_degree_graph() */

#endif /* DEGREEVERSION 3 */

/************************************************************/
/* this function is the interface function for generating and 
 * returning a degreebound graph
 *
 * degpercent[] - array of percentages for each degree
 *                (range is 0 to 1)
 * degsize     - largest non-zero degree
 */
void 
gen_degbound_graph(
  graph_type *graph,
  int numvert,
  int degsize,
  float degpercent[])
{
  int loop;
  int perloop;
  int maxdeg[MAXVERT];
  int degcount[MAXDEGREE];

  int sumdeg;
  int mindeg;
  int ret;
  int totvert;

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, 
	"Generating a degreebound graph on %d vertices.\n", numvert);
  }

  /* calculate number of vertices for each degree */
  degcount[0] = degcount[1] = 0;
  for (totvert = 0, loop = 2; loop < degsize; loop++)
  {
    degcount[loop] = (int) rint((numvert * degpercent[loop]));
    /* debug fprintf(stderr, "degcount[%d] =%d\n",loop,degcount[loop]); */
    totvert += degcount[loop];
  }

  degcount[degsize] = numvert - totvert;
  /* debug fprintf(stderr, "degcount[%d] =%d\n",degsize,degcount[degsize]); */


  /* create the degree sequence */
  perloop = 0;
  degcount[perloop] = 0;
  for (loop = 0; loop < numvert; loop++)
  {
    while (degcount[perloop] == 0)
    {
      perloop++;
      if (perloop > degsize)
      {
        EXIT_ERROR("Error in degree sequence in gen_degbound_graph().");
      }
    }
    maxdeg[loop] = perloop;
    degcount[perloop] --;
  } /* end of for loop */

  /* confirm the sum of degrees is even, otherwise add 1 to the lowest degree
   * also check that maxdeg for each vertex <= numvert -1 */
  for (sumdeg = 0, mindeg = 0, loop = 0; loop < numvert; loop++)
  {
/* unnecessary code */
#ifdef NOTNEEDED
    if (maxdeg[loop] > numvert-1)
      maxdeg[loop] = numvert-1;

    if (maxdeg[loop] < g_options.graphgen.mindeg)
      maxdeg[loop] = g_options.graphgen.mindeg;
#endif

    sumdeg += maxdeg[loop];

    if (maxdeg[loop] < maxdeg[mindeg] )
      mindeg = loop;
  }

  if (sumdeg % 2 == 1)
    maxdeg[mindeg]++;

  /* generate the graph: if problem in generation, retry till success */
  do
  {
    ret = generate_fixed_degree_graph(graph, numvert, maxdeg);
    if ( (g_options.report_flags == REPORT_ALG) && (ret == GEN_FAILURE) )
    {
      fprintf(g_options.log_fp, "  Regenerating degreebound graph.\n");
    }
  }  while (ret == GEN_FAILURE);

  random_permute_graph(graph);

}  /* end of gen_degbound_graph() */


/************************************************************/
/* geometric graph generator */
/************************************************************/

/* This was taken from Joseph Culberson's Coloring Page 
 * and modified by Basil Vandegriend
 */
/*
        Title:  Geometric Graph Generator (Improved versions - March, May 1995)
	file:	geomgraph.c
        Does:   Generates quasi-random k-colorable graphs that may be used
                to test coloring algorithms.  Randomly places the vertices
		in a dim dimensioned hypercube.  Connects vertex pairs with
		an edge if they are different colors and satisfy a distance
		requirement. 
        Source: Joseph Culberson's Coloring Page
                http://web.cs.ualberta.ca/~joe/Coloring/index.html
        Author: initial version:  Jonathan Baldwin
		Modified by J. Culberson, March 1995
		Modified by Denis Papp, May 1995
		Modified by Adam Beacham, May 1995
        email:  joe@cs.ualberta.ca
		abeacham@cs.ualberta.ca
		dpapp@cs.ualberta.ca

        Input Parameters:
                size : number of vertices
                dist2 : distance (for adding edges), squared
                dflag: == CONNECT_NEAR if adding edges when distance <= dist
                       == CONNECT_FAR if adding edges when distance <= dist
                dim  : number of dimensions (2 = planar)
                wrap : == GRAPH_WRAP if 2 vertices on opposite edges are near
                       == GRAPH_NOWRAP if not

        Output Parameters:
                graph : pointer to allocated graph structure

		part - number of color partitions (isn't needed?)
		size - # of edges put in graph
		file - the file stream to the gnuplot file.

*/

/************************************************************/
/* this function calculates the distance squared between two points
 * (used by geometric graph generator)
 */
double 
geo_distance(
  double vert[], 
  int dim,
  int i, 
  int j, 
  int depth, 
  int wrap) 
{
  
  double d1, d2, d=0.0;

  if (depth > 0) 
  {
    d = geo_distance(vert,dim,i,j,depth-1,wrap);
  }

  d1 = (vert[i*dim+depth] - vert[j*dim+depth]);
  if (wrap) 
  {
    if (d1 > 0.0) 
    {
	d2 = 1.0 - d1;
    }
    else 
    {
	d2 = 1.0 + d1;
    }
    d1 *= d1;
    d2 *= d2;

    if (d2 < d1) 
      d1 = d2;  /* do wrapping */

  } 
  else 
  {
    d1 *= d1;
  }

  return(d+d1);

}  /* end of geo_distance() */
  

/************************************************************/
/* this function generates the geometric graph 
 * 
 * size : number of vertices
 * dist2: distance (for adding edges)
 * dflag: == CONNECT_NEAR if adding edges when distance <= dist
 *        == CONNECT_FAR if adding edges when distance <= dist
 * dim  : number of dimensions (2 = planar)
 * wrap : == GRAPH_WRAP if 2 vertices on opposite edges are near
 */
void 
gen_geo_graph(
  int size,
  double dist2,
  int dflag,
  int dim,
  int wrap,
  graph_type *graph)
{
  double *vert;
  int loop, eloop;
  double d2;
  double mindist = 0.0;
  int minindex;

  /* print generation information/options */

  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, 
	"Generating a geometric graph on %d vertices.\n", size);
  }

  /* allocate memory for vertices of graph */
  vert = (double *) malloc(sizeof(double)*dim*size);
  if (vert == NULL)
  {
    EXIT_ERROR("Error allocating memory for vert[] array.\n");
  }

  /* prepare graph data structure */
  init_graph(graph);
  if ( (size < 1) || (size > MAXVERT) )
  {
    EXIT_ERROR("Error: invalid number of vertices specified.\n");
  }
  graph->numvert = size;

  /* assign random coordinate (0.0 <= x < 1.0) to each dimension 
   * of each vertex 
   */
  for (loop = 0; loop < dim*size; loop++)
    vert[loop] = ( (double) (lrand48() % LARGEVAL ) ) / (double) LARGEVAL;

  /* for each vertex, check if it has an edge with each other vertex */
  for (loop = 0; loop < size-1; loop++)
  {
    for (eloop = loop+1; eloop < size; eloop++)
    {
      /* calculate distance (squared) between the two vertices */
      d2 = geo_distance(vert, dim, loop, eloop, dim-1, wrap);

      if ( ( (dflag == CONNECT_NEAR) && (d2 <= dist2) ) ||
           ( (dflag == CONNECT_FAR) && (d2 > dist2) ) )
      {
        add_edge_graph(graph, loop, eloop);
      }
    }

  }  /* end of loop through vertices */

  /* check if each vertex has a sufficiently large degree.  if vertex
   * degree is below the minimum degree, then add edges to nearest non-edge
   * vertices.
   * note: add edges based on distance w.r.t CONNECT_NEAR or CONNECT_FAR
   */
  if (g_options.graphgen.mindeg > 0)
  {
    for (loop = 0; loop < size; loop++)
    {
      /* if a vertex's degree is below the min deg, look for the min (or max)
       * distance vertex and add that edge.
       */
      while (graph->deg[loop] < g_options.graphgen.mindeg)
      {
        if (dflag == CONNECT_NEAR)
          mindist = 1.0;
        else if (dflag == CONNECT_FAR)
          mindist = 0.0;

        for (minindex = -1, eloop = 0; eloop < size; eloop++)
        {
          if (eloop == loop)
            continue;
  
          d2 = geo_distance(vert, dim, loop, eloop, dim-1, wrap);
          if ( ( (d2 < mindist) && (d2 > dist2) && (dflag == CONNECT_NEAR) ) ||
               ( (d2 > mindist) && (d2 < dist2) && (dflag == CONNECT_FAR) ) )
          {
            mindist = d2;
            minindex = eloop;
          }
        }

        if (minindex == -1)
        {
          WARN_ERROR("Warning:  cannot fulfill min. degree requirements in ");
          WARN_ERROR("gen_geo_graph().\n");
          break;
        }
        else	/* add edge to graph */
        {
          add_edge_graph(graph, loop, minindex);
        }
      }  /* end of while statement */
    }  /* end of for loop */
  }

}  /* end of gen_geo_graph() */


/************************************************************/
/* this function parses the option file for parameters for generating 
 * a particular graph type
 *
 * input parameters:  word : the argument
 *                    fp   : file pointer to file, set to the parameters of
 *                           the argument
 *             graphgentype: the type of graph to generate
 * output parameters: word : the next argument/comment (non-parameter) word
 *                   return: the status of the most recent read
 * 
 * see manual.txt for option file parameters
 *
 */
int
parse_graphgen_options(
  FILE *fp,
  char word[],
  int graphgentype)
{
  int status;
  float temp;

  char parmstr[STRLEN];
  char valstr[STRLEN];

  /* read next word in options file */
  status = read_next_word(fp, word);

  /* if not generating a graph, then exit */
  if (graphgentype == GEN_NOGRAPH)
    return(status);

  /* read next parameter, till read a non-parameter word, then return */
  while ( !(status & READ_EOF) )
  {
    if (check_word(word) == WORD_PARM)
    {
      /* parse the parameter */
      parse_parameter_str(word, parmstr, valstr);

      /* number of vertices parameter */
      if ( (strcasecmp(parmstr, "+n")==0) || 
		(strcasecmp(parmstr, "+nvertex") == 0) )
      {
        g_options.graphgen.nvertex = (int) atoi(valstr);        
        if ( (g_options.graphgen.nvertex < 1) || 
		(g_options.graphgen.nvertex > MAXVERT) )
        {
          EXIT_ERROR("Invalid number of vertices specified for `+nvertex'.\n");
        }  

        if ( (graphgentype == GEN_KNIGHTTOUR) || 
		(graphgentype == GEN_CROSSROADS) )
        {
          fprintf(stderr,"Warning:  `+nvertex' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* ensure hamiltonicity parameter */
      else if (strcasecmp(parmstr, "+ensureham") == 0)
      {
        g_options.graphgen.makeham = HAM_ENSURE;
        if (graphgentype == GEN_KNIGHTTOUR)
        {
          EXIT_ERROR("Warning: cannot ensure hamiltonicity of knight's tour graphs.\n");
        }
      }
      
      /* distance */
      else if (strcasecmp(parmstr, "+dist") == 0) 
      {
        sscanf(valstr, "%f", &(g_options.graphgen.dist) );
        if ( (g_options.graphgen.dist <= 0.0) || 
		(g_options.graphgen.dist > 1.0 ) )
        {
          EXIT_ERROR("Invalid distance specified for `+dist'.\n");
        }  

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+dist' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* distance squared */
      else if (strcasecmp(parmstr, "+dist2") == 0) 
      {
        sscanf(valstr, "%f", &temp);
        if ( (temp <= 0.0) || (temp > 1.0 ) )
        {
          EXIT_ERROR("Invalid distance specified for `+dist2'.\n");
        }  

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+dist2' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }

        g_options.graphgen.dist = (float) sqrt( (double) temp);
      }

      /* # of dimensions */
      else if (strcasecmp(parmstr, "+dim") == 0) 
      {
        g_options.graphgen.dim = (int) atoi(valstr);

        if ( (g_options.graphgen.dim < 1 ) )
        {
          EXIT_ERROR("Invalid number of dimensions specified for `+dim'.\n");
        }  

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+dim' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* near flag */
      else if (strcasecmp(parmstr, "+near") == 0) 
      {
        g_options.graphgen.dflag = CONNECT_NEAR;

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+near' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* far flag */
      else if (strcasecmp(parmstr, "+far") == 0) 
      {
        g_options.graphgen.dflag = CONNECT_FAR;

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+far' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* wrap flag */
      else if (strcasecmp(parmstr, "+wrap") == 0) 
      {
        g_options.graphgen.dflag = GRAPH_WRAP;

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+wrap' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* no-wrap flag */
      else if (strcasecmp(parmstr, "+nowrap") == 0) 
      {
        g_options.graphgen.dflag = GRAPH_NOWRAP;

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+wrap' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* minimum degree graph is forced to have */
      else if (strcasecmp(parmstr, "+mindeg") == 0) 
      {
        g_options.graphgen.mindeg = (int) atoi(valstr); 

        if ( (g_options.graphgen.mindeg < 0 ) || 
		(g_options.graphgen.mindeg > MAXDEGREE) )
        {
          EXIT_ERROR("Invalid minimum degree specified for `+mindeg'.\n");
        }  

        if (graphgentype != GEN_GEOMETRIC)
        {
          fprintf(stderr,"Warning:  `+mindeg' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* mean vertex degree */ 
      else if (strcasecmp(parmstr, "+meandeg") == 0) 
      {
        sscanf(valstr, "%f", &(g_options.graphgen.meandeg) );

        if ( (g_options.graphgen.meandeg <= 0.0 ) || 
		(g_options.graphgen.meandeg > g_options.graphgen.nvertex) )
        {
          EXIT_ERROR("Invalid value specified for `+meandeg'.\n");
        }  

        if (graphgentype != GEN_RANDOM)
        {
          fprintf(stderr,"Warning:  `+meandeg' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }
    
      /* degree percentage */
      else if (strcasecmp(parmstr, "+d2") == 0)
      {
        sscanf(valstr, "%g", &(g_options.graphgen.degpercent[2]) );
        if (g_options.graphgen.degsize < 2)
        {
          g_options.graphgen.degsize = 2;
        }

        if ( (g_options.graphgen.degpercent[2] < 0.0 ) ||
                (g_options.graphgen.degpercent[2] > 1.0 ) )
        {
          EXIT_ERROR("Invalid value specified for `+d2'.\n");
        }

        if (graphgentype != GEN_DEGREEBOUND)
        {
          fprintf(stderr,"Warning:  `+d2' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* degree percentage */
      else if (strcasecmp(parmstr, "+d3") == 0)
      {
        sscanf(valstr, "%g", &(g_options.graphgen.degpercent[3]) );
        if (g_options.graphgen.degsize < 3)
        {
          g_options.graphgen.degsize = 3;
        }

        if ( (g_options.graphgen.degpercent[3] < 0.0 ) ||
                (g_options.graphgen.degpercent[3] > 1.0 ) )
        {
          EXIT_ERROR("Invalid value specified for `+d3'.\n");
        }

        if (graphgentype != GEN_DEGREEBOUND)
        {
          fprintf(stderr,"Warning:  `+d3' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* degree percentage */
      else if (strcasecmp(parmstr, "+d4") == 0)
      {
        sscanf(valstr, "%g", &(g_options.graphgen.degpercent[4]) );
        if (g_options.graphgen.degsize < 4)
        {
          g_options.graphgen.degsize = 4;
        }

        if ( (g_options.graphgen.degpercent[4] < 0.0 ) ||
                (g_options.graphgen.degpercent[4] > 1.0 ) )
        {
          EXIT_ERROR("Invalid value specified for `+d4'.\n");
        }

        if (graphgentype != GEN_DEGREEBOUND)
        {
          fprintf(stderr,"Warning:  `+d4' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* degconst parameter */ 
      else if (strcasecmp(parmstr, "+degconst") == 0) 
      {
        sscanf(valstr, "%f", &(g_options.graphgen.degconst) );

        if (g_options.graphgen.degconst <= 0.0 )
        {
          EXIT_ERROR("Invalid value specified for `+degconst'.\n");
        }  

        if ( graphgentype != GEN_RANDOM)
        {
          fprintf(stderr,"Warning:  `+degconst' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* board dimension #1 */
      else if (strcasecmp(parmstr, "+board1") == 0) 
      {
        g_options.graphgen.board1 = (int) atoi(valstr); 

        if ( (g_options.graphgen.board1 <= 0 ) || 
		(g_options.graphgen.board1 > MAXBOARDSIZE) )
        {
          EXIT_ERROR("Invalid size specified for `+board1'.\n");
        }  

        if ( (graphgentype != GEN_KNIGHTTOUR) )
        {
          fprintf(stderr,"Warning:  `+board1' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* board dimension #2 */
      else if (strcasecmp(parmstr, "+board2") == 0) 
      {
        g_options.graphgen.board2 = (int) atoi(valstr); 

        if ( (g_options.graphgen.board2 <= 0 ) || 
		(g_options.graphgen.board2 > MAXBOARDSIZE) )
        {
          EXIT_ERROR("Invalid size specified for `+board2'.\n");
        }  

        if ( (graphgentype != GEN_KNIGHTTOUR) )
        {
          fprintf(stderr,"Warning:  `+board2' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* move distance #1 */
      else if (strcasecmp(parmstr, "+move1") == 0) 
      {
        g_options.graphgen.move1 = (int) atoi(valstr); 

        if ( (g_options.graphgen.move1 < 1 ) || 
		(g_options.graphgen.move1 > MAXBOARDSIZE-1) )
        {
          EXIT_ERROR("Invalid size specified for `+move1'.\n");
        }  

        if ( (graphgentype != GEN_KNIGHTTOUR) )
        {
          fprintf(stderr,"Warning:  `+move1' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* move distance #2 */
      else if (strcasecmp(parmstr, "+move2") == 0) 
      {
        g_options.graphgen.move2 = (int) atoi(valstr); 

        if ( (g_options.graphgen.move2 < 1 ) || 
		(g_options.graphgen.move2 > MAXBOARDSIZE-1) )
        {
          EXIT_ERROR("Invalid size specified for `+move2'.\n");
        }  

        if ( (graphgentype != GEN_KNIGHTTOUR) )
        {
          fprintf(stderr,"Warning:  `+move2' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* number of crossroads or ICCS subgraphs */
      else if (strcasecmp(parmstr, "+subgraphs") == 0) 
      {
        g_options.graphgen.numsubgraphs = (int) atoi(valstr); 

        if ( (g_options.graphgen.numsubgraphs < 1 ) ||
		(g_options.graphgen.numsubgraphs*7 >= MAXVERT) )
        {
            EXIT_ERROR("Invalid amount specified for `+subgraphs'.\n");
        }  

        if ( (graphgentype != GEN_CROSSROADS) && 
		(graphgentype != GEN_ICCS) )
        {
          fprintf(stderr,"Warning:  `+subgraphs' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* number of cycles to add */
      else if (strcasecmp(parmstr, "+numcycles") == 0) 
      {
        sscanf(valstr, "%f", &(g_options.graphgen.numcycles) );

        if (g_options.graphgen.numcycles < 1 ) 
        {
          EXIT_ERROR("Invalid amount specified for `+numcycles'.\n");
        }  

        if ( (graphgentype != GEN_ADDCYCLE) )
        {
          fprintf(stderr,"Warning:  `+numcycles' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* length of path to add */
      else if (strcasecmp(parmstr, "+path1") == 0) 
      {
        sscanf(valstr, "%f", &(g_options.graphgen.pathlengths[0]) );

        if ( (g_options.graphgen.pathlengths[0] < 0) ||
		(g_options.graphgen.pathlengths[0] > 1) ) 
        {
          EXIT_ERROR("Invalid amount specified for `+path1'.\n");
        }  

        if ( (graphgentype != GEN_ADDPATH) )
        {
          fprintf(stderr,"Warning:  `+path1' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* length of path to add */
      else if (strcasecmp(parmstr, "+path2") == 0) 
      {
        sscanf(valstr, "%f", &(g_options.graphgen.pathlengths[1]) );

        if ( (g_options.graphgen.pathlengths[1] < 0) ||
		(g_options.graphgen.pathlengths[1] > 1) ) 
        {
          EXIT_ERROR("Invalid amount specified for `+path2'.\n");
        }  

        if ( (graphgentype != GEN_ADDPATH) )
        {
          fprintf(stderr,"Warning:  `+path2' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
        }
      }

      /* size of independent sets (ICCS) */
      else if (strcasecmp(parmstr, "+indsetsize") == 0) 
      {
        sscanf(valstr, "%d", &(g_options.graphgen.indsetsize) );

        if (g_options.graphgen.indsetsize < 2) 
        {
          EXIT_ERROR("Invalid amount specified for `+indsetsize'.\n");
        }  

        if ( (graphgentype != GEN_ICCS) )
        {
          fprintf(stderr,"Warning:  `+indsetsize' parameter not relevant for "
                  "%s graph generation.\n", opt_gen_str[graphgentype]);
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

}  /* end of parse_geometric_graph_opt() */


/************************************************************/
/* this function initializes the graphgen options 
 * default values are set
 */
void
init_graphgen_options()
{
  int loop;

  /* set variables to `reasonable' values */

  g_options.graphgen.nvertex = 50;

  g_options.graphgen.makeham = HAM_DONTCARE;

  g_options.graphgen.dist = 0.255;
  g_options.graphgen.dim = 2;
  g_options.graphgen.dflag = CONNECT_NEAR;
  g_options.graphgen.wrapflag = GRAPH_WRAP;
  g_options.graphgen.mindeg = 0;

  g_options.graphgen.board1 = 8;
  g_options.graphgen.board2 = 8;
  g_options.graphgen.move1 = 1;
  g_options.graphgen.move2 = 2;

  g_options.graphgen.numsubgraphs = 0;

  g_options.graphgen.degconst = 0.0;
  g_options.graphgen.meandeg = 3.0;

  g_options.graphgen.numcycles = 2.0;

  for (loop = 0; loop < MAXNUMADDPATHS; loop++)
    g_options.graphgen.pathlengths[loop] = 0.0;

  g_options.graphgen.indsetsize = 0;

  /* leave these at 0 to make parse code work properly */
  g_options.graphgen.degsize = 2;
  g_options.graphgen.degpercent[2] = 0.0;
  g_options.graphgen.degpercent[3] = 0.0;
  g_options.graphgen.degpercent[4] = 0.0;
  g_options.graphgen.degpercent[5] = 0.0;

}  /* end of init_graphgen_options() */


/************************************************************/
/* this function prints the graphgen options 
 * only relevant options are printed (depends upon current gengraph type)
 */
void
print_graphgen_options(
  FILE *fp)
{
  int loop;

  /* print each parameter if relevant to type of graph being generated */
  if ( (g_options.graphgentype != GEN_KNIGHTTOUR) &&
	(g_options.graphgentype != GEN_CROSSROADS) &&
	(g_options.graphgentype != GEN_ICCS) )
  {
    fprintf(fp, "    # of vertices = %d\n", g_options.graphgen.nvertex); 
    if (g_options.graphgen.makeham == HAM_ENSURE)
    {
      fprintf(fp, "    ensure that graph is hamiltonian\n");
    }
  }

  /* ICCS graph */
  if (g_options.graphgentype == GEN_ICCS)
  {
    fprintf(fp, "    # of subgraphs = %d\n", g_options.graphgen.numsubgraphs);
    fprintf(fp, "    size of independent set = %d\n", 
	g_options.graphgen.indsetsize);
  }

  /* addpath graph */
  if (g_options.graphgentype == GEN_ADDPATH)
  {
    int loop = 0;
    while ( (g_options.graphgen.pathlengths[loop] > 0.0) && 
		(loop < MAXNUMADDPATHS) )
    {
      fprintf(fp, "    path length = %.2f\n",
	g_options.graphgen.pathlengths[loop]);
      loop++;
    } 
  }

  /* addcycle graph */
  if (g_options.graphgentype == GEN_ADDCYCLE)
  {
    fprintf(fp, "    # of cycles to add = %.2f\n",
	g_options.graphgen.numcycles);
  }

  /* random graph */
  if (g_options.graphgentype == GEN_RANDOM)
  {
    if (g_options.graphgen.degconst > 0.0)
      fprintf(fp, "    degree constant = %.2f\n", g_options.graphgen.degconst); 
    else
      fprintf(fp, "    mean degree = %.1f\n", g_options.graphgen.meandeg); 
  }

  /* geometric graph */
  if (g_options.graphgentype == GEN_GEOMETRIC)
  {
    fprintf(fp, "    distance = %.5f\t", g_options.graphgen.dist); 
    fprintf(fp, "    dist squared = %.3f\n", 
	g_options.graphgen.dist * g_options.graphgen.dist); 
    fprintf(fp, "    # of dimensions = %d\t", g_options.graphgen.dim); 
    fprintf(fp, "    geometric graph flags: ");
    if ( g_options.graphgen.dflag == CONNECT_NEAR)
      fprintf(fp, "CONNECT_NEAR, ");
    else
      fprintf(fp, "CONNECT_FAR, ");

    if ( g_options.graphgen.wrapflag == GRAPH_WRAP)
      fprintf(fp, "WRAP\n");
    else
      fprintf(fp, "NO WRAP\n");
  }

  /* degree bound graph */
  if (g_options.graphgentype == GEN_DEGREEBOUND)
  {
    for (loop = 2; loop <= g_options.graphgen.degsize; loop++)
    {
      if (g_options.graphgen.degpercent[loop] == 0) 
      {
        continue;
      }
   
      fprintf(fp, "    percentage of vertices of degree %d = %.3f\n", loop,
	g_options.graphgen.degpercent[loop]);
    }
  }

  /* geometric graph */
  if (g_options.graphgentype == GEN_GEOMETRIC)
  {
    if ( g_options.graphgen.mindeg > 0)
      fprintf(fp, "    forced minimum degree = %d\n", 
	g_options.graphgen.mindeg);
  }

  /* knighttour graph */
  if (g_options.graphgentype == GEN_KNIGHTTOUR)
  {
    fprintf(fp, "    board dimensions = %d, %d\n", 
	g_options.graphgen.board1, g_options.graphgen.board2);
    fprintf(fp, "    knight movement = %d, %d\n", 
	g_options.graphgen.move1, g_options.graphgen.move2);
  }
  
  /* crossroads graph */
  if (g_options.graphgentype == GEN_CROSSROADS)
  {
    fprintf(fp, "    # of crossroads subgraphs = %d\n", 
	g_options.graphgen.numsubgraphs);
  }

}  /* end of print_graphgen_options() */

