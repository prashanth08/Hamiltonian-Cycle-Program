
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend 
 * Email: basil@cs.ualberta.ca
 * File: graphdata.c
 * Contents: data structure to store graphs
 *	     access functions for this data structure
 *
 */
/*
	Copyright (c) 1998 Basil Vandegriend. 
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
/* graph I/O functions 
 *
 * specification of graph file: 
 *  filename extension = .graph 
 *  first 'word' of line either comment symbol or a number
 *  comment symbols:  "# "  (GFILE_COMMENT)
 *  ignore all lines starting with comment symbol
 *  **** IMPORTANT:  lines with a comment must have other text on them ****
 *  ie:   valid:  # here is a line
 *      invalid:  #
 *
 * format of graph file:
 *  first line :  	# of vertices in graph (n)
 *  lines 2 to n+1:  	vertex label in range 0 ... n-1
 *			degree of vertex (v)
 *			the v neighbors of the vertex
 *  note each number is separated by whitespace
 */
/************************************************************/

/************************************************************/
/* output graph
 * it is assumed that the file pointer is opened, and can be written to
 * if the comments[] argument is non-empty, it will be added as a line of
 *   comments to the top of the graph file 
 */
int
output_graph(
  FILE *fp,
  graph_type *graph,
  char comments[])
{
  int loop;
  int nloop;

  /* print header statement */
  fprintf(fp, "# graph file\n");
 
  /* print comments if non-empty */
  if (strcmp( comments, "") != 0)
  {
    fprintf(fp, "# %s \n", comments);
  }

  /* print # of vertices */
  fprintf(fp, "%d\n", graph->numvert);

  fprintf(fp, "# Vert\tDegree\tNeighbors\n");

  /* print each line of graph */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    fprintf(fp, "%3d\t%3d\t", loop, graph->deg[loop]);
   
    for (nloop = 0; nloop < graph->deg[loop]; nloop++)
    {
      fprintf(fp, "%3d ", graph->nbr[loop][nloop]);
    }
    fprintf(fp, "\n");
  }
    
  fprintf(fp, "\n# end of graph file\n");
 
  return(RET_OK);

}  /* end of output_graph() */


/************************************************************/
/* output graph in fortran format (alg595)
 * it is assumed that the file pointer is opened, and can be written to
 */
void
output_fortran_graph(
  FILE *fp,
  graph_type *graph)
{
  int loop;
  int nloop;
  int degsum;
  int count;

  /* print # of vertices */
  fprintf(fp, "%5d\n", graph->numvert);

  /* print vertex degree summations */
  fprintf(fp, "%5d", 0);
  for (count = 1, degsum = 0, loop = 1; loop <= graph->numvert; loop++)
  {
    degsum += graph->deg[loop-1];
    fprintf(fp, "%5d", degsum);
    count++;
    if (count == 10)
    {
      fprintf(fp, "\n");
      count = 0;
    }
  }
  fprintf(fp, "\n");

  /* print adjacency list:  note that vertex numbers start with 1 */
  for (count = 0, loop = 0; loop < graph->numvert; loop++)
  {
    for (nloop = 0; nloop < graph->deg[loop]; nloop++)
    {
      fprintf(fp, "%5d", graph->nbr[loop][nloop]+1);
      count++;
      if (count == 10)
      {
        fprintf(fp, "\n");
        count = 0;
      }
    }
  }
  fprintf(fp, "\n");
    
}  /* end of output_fortran_graph() */


/************************************************************/
/* get next word
 * this function reads the next word from the specified file pointer, and
 * junks the line if it corresponds to a comment
 */
int
get_next_word(
  FILE *fp,
  char *str)
{
  char instr[256];
  int done = 0;

  do
  {
    fscanf(fp, "%s ", instr);
    if ( strcmp(instr, GFILE_COMMENT) == 0 )
    {
      /* have a comment, so skip the rest of the line */
      fgets(instr, 256, fp);
    }
    else
    {
      done = 1;
    }
  } while (done == 0);

  strcpy(str, instr);

  return(RET_OK);

}  /* end of get_next_word() */


/************************************************************/
/* input graph
 * it is assumed that the file pointer is opened, and can be read from
 * it is assumed that the graph pointer points to an allocated data 
 *   structure
 */
int
input_graph(
  FILE *fp,
  graph_type *graph)
{
  char instr[256];  
  int loop;
  int nloop;

  /* read # of vertices (skip over comments) */
  get_next_word(fp, instr);

  graph->numvert = atoi(instr);
  if (graph->numvert < 1 || graph->numvert > MAXVERT)
  {
    EXIT_ERROR("Invalid number of vertices in graph file.\n");
  } 
  
  /* start reading each line describing each vertex */
  for (loop = 0; loop < graph->numvert; loop++)
  {
 
    /* get vertex number */
    get_next_word(fp, instr);
    if (atoi(instr) != loop)
    {
      EXIT_ERROR("Vertex is out of order in graph file.\n");
    }

    /* get degree of vertex */
    fscanf(fp, " %d ", &(graph->deg[loop]) );
    if ( (graph->deg[loop] < 0) || (graph->deg[loop] > MAXDEGREE) )
    {
      EXIT_ERROR("Invalid vertex degree # in graph file.\n");
    }

    /* get neighbors of vertex */
    for (nloop = 0; nloop < graph->deg[loop]; nloop++)
    {
      fscanf(fp, " %d ", &(graph->nbr[loop][nloop]) );
      if ( (graph->nbr[loop][nloop] < 0) || 
		(graph->nbr[loop][nloop] > MAXVERT) )
      {
        EXIT_ERROR("Invalid vertex neighbor in graph file.\n");
      }
    }  /* end of neighbor loop */
  }  /* end of vertex loop */
      
  return(RET_OK);

}  /* end of input_graph() */


/************************************************************/
/* graph statistics
 *
 * the following routine(s) calculate and print out statistics on
 * graphs 
 */
/************************************************************/


/************************************************************/
/* this function calculates all possible graph statistic
 * information, including a histogram of vertex degrees
 * the information is stored in the graph data structure
 */
void 
calc_graph_stats(
  graph_type *graph)
{
  int loop;

  /* initialize variables */
  graph->mindeg = MAXDEGREE;
  graph->maxdeg = 0;
  graph->numedges = 0;

  for (loop = 0; loop < MAXDEGREE; loop++)
    graph->deghistogram[loop] = 0;

  /* calculate mean, stddev of vertex degree */
  calc_list_mean_stddev(graph->deg, graph->numvert, &(graph->meandeg), 
	&(graph->stddevdeg));
 
  /* calculate min, max degrees and total number of edges 
   * create vertex degree histogram
   */
  for (loop = 0; loop < graph->numvert; loop++)
  {
    graph->deghistogram[graph->deg[loop]] ++;
    if (graph->deg[loop] < graph->mindeg)
      graph->mindeg = graph->deg[loop];
    if (graph->deg[loop] > graph->maxdeg)
      graph->maxdeg = graph->deg[loop];

    graph->numedges += graph->deg[loop];
  }

  graph->numedges /= 2;   /* each edge is counted twice */
  
}  /* end of calc_graph_stats() */


/************************************************************/
/* this function prints out all possible graph statistic
 * information, including a histogram of vertex degrees
 * it does NOT print out graph solve information
 */
void 
print_graph_stats(
  FILE *fp,
  graph_type *graph)
{
  int loop;

  fprintf(fp,"Graph Information:  \n");
  fprintf(fp,"  # of vertices = %d\t", graph->numvert);
  fprintf(fp,"  # of edges = %d\n", graph->numedges);
  fprintf(fp,"  mean vertex degree (stddev) = %.2f (%.2f) \n", 
		graph->meandeg, graph->stddevdeg);
  fprintf(fp,"  min degree = %d\t", graph->mindeg);
  fprintf(fp,"  max degree = %d\n", graph->maxdeg);

  fprintf(fp,"  Vertex Degree Histogram:\n");
  for (loop = graph->mindeg; loop <= graph->maxdeg; loop++)
  {
    if (graph->deghistogram[loop] != 0)
    {
      fprintf(fp,"    degree %d: # of vertices = %d\n", 
		loop, graph->deghistogram[loop]);
    }
  }

}  /* end of print_graph_stats() */


/************************************************************/
/* graph access functions
 *
 * the following routine(s) perform basic operations on the graph
 * 
 */
/************************************************************/


/************************************************************/
/* this function initializes/emptys an allocated graph structure
 * (creates a completely unconnected graph) 
 */
void 
init_graph(
  graph_type *graph)
{
  int loop, dloop;

  for (loop = 0; loop < MAXVERT; loop++)
  {
    graph->deg[loop] = 0;
    for (dloop = 0; dloop < MAXDEGREE; dloop++)
      graph->nbr[loop][dloop] = 0;
  }

  graph->numvert = 0;
  graph->numedges = 0;
  graph->meandeg = 0.0;
  graph->stddevdeg = 0.0;
  graph->mindeg = 0;
  graph->maxdeg = 0;

  graph->solve = HC_NOT_FOUND;

  for (loop = 0; loop < MAXDEGREE; loop++)
    graph->deghistogram[loop] = 0;


}  /* end of init_graph() */


/************************************************************/
/* this function copys a graph into a second graph.
 * (the second graph is initialized first)
 * graph is copied from source into dest
 */
void 
copy_graph(
  graph_type *source,
  graph_type *dest)
{
  int loop, dloop;

  /* initialize destination graph */
  init_graph(dest);

  /* degree and neighbour information */
  dest->numvert = source->numvert;
  for (loop = 0; loop < source->numvert; loop++)
  {
    dest->deg[loop] = source->deg[loop];

    for (dloop = 0; dloop < source->deg[loop]; dloop++)
      dest->nbr[loop][dloop] = source->nbr[loop][dloop];
  }

  /* copy statistics information */
  dest->numedges = source->numedges;
  dest->meandeg = source->meandeg;
  dest->stddevdeg = source->stddevdeg;
  dest->mindeg = source->mindeg;
  dest->maxdeg = source->maxdeg;
 
  for (loop = 0; loop < MAXDEGREE; loop++)
  {
    dest->deghistogram[loop] = source->deghistogram[loop];
  }

  dest->solve = source->solve;
 
}  /* end of copy_graph() */


/************************************************************/
/* this function adds a directed edge to a current graph.
 * it calls EXIT_ERROR if this would exceed the MAXDEGREE of a
 * particular vertex
 *
 * the edge (v1, v2) is added to the graph.
 *
 * note: the function does not check to see if the edge already exists!
 */
void 
add_direct_edge_graph(
  graph_type *graph,
  int v1,
  int v2)
{

  if ( (v1 < 0) || (v1 >= MAXVERT ) || (v2 < 0) || (v2 >= MAXVERT) )
  {
    EXIT_ERROR("Invalid vertex specified in add_direct_edge_graph().\n");
  }

  graph->nbr[v1][graph->deg[v1]] = v2;
  graph->deg[v1]++;

  if ( (graph->deg[v1] >= MAXDEGREE) || (graph->deg[v2] >= MAXDEGREE) )
  {
    EXIT_ERROR("Maximum vertex degree exceeded in add_direct_edge_graph().\n");
  }

}  /* end of add_direct_edge_graph() */


/************************************************************/
/* this function adds an undirected  edge to a current graph.
 * it calls EXIT_ERROR if this would exceed the MAXDEGREE of a
 * particular vertex
 * v1 and v2 are the 2 vertices joined by the edge
 *
 * note: does not check to see if the edge already exists!
 */
void 
add_edge_graph(
  graph_type *graph,
  int v1,
  int v2)
{

  add_direct_edge_graph(graph, v1, v2);
  add_direct_edge_graph(graph, v2, v1);

}  /* end of add_edge_graph() */


/************************************************************/
/* remove undirected edge (x,y) from the graph
 *
 * returns EDGE_NOTEXIST if edge does not exist
 * returns EDGE_REMOVE if edge was removed
 */
int
rm_edge_graph(
  graph_type *graph,
  int x,
  int y)
{
  int loop, loop2;

  /* remove y as a neighbour of x */
  for (loop = 0; loop < graph->deg[x]; loop++)
  {
    if (graph->nbr[x][loop] == y)
    {
      for (loop2 = loop+1; loop2 < graph->deg[x]; loop2++)
        graph->nbr[x][loop2-1] = graph->nbr[x][loop2];

      break;
    }
  }

  if (loop == graph->deg[x])		/* edge not found */
  { 
    return(EDGE_NOTEXIST);
  }

  /* remove x as a neighbour of y */
  for (loop = 0; loop < graph->deg[y]; loop++)
  {
    if (graph->nbr[y][loop] == x)
    {
      for (loop2 = loop+1; loop2 < graph->deg[y]; loop2++)
        graph->nbr[y][loop2-1] = graph->nbr[y][loop2];

      break;
    }
  }

  if (loop == graph->deg[y])		/* error, edge not consistant */
  { 
    EXIT_ERROR("Error: inconsistant edge in rm_edge_graph().\n");
  }

  (graph->deg[x])--;
  (graph->deg[y])--;

  return(EDGE_REMOVE);

}  /* end of rm_edge_graph() */


/************************************************************/
/* checks whether 2 vertices are joined by an edge
 * (directed edge x -> y)
 * returns EDGE_EXIST if edge is there, returns EDGE_NOTEXIST if no edge 
 */
int
check_if_edge(
  graph_type *graph,
  int x,
  int y)
{
  int loop;

  for (loop = 0; loop < graph->deg[x]; loop++)
  {
    if (graph->nbr[x][loop] == y)
      return(EDGE_EXIST);
  }

  return(EDGE_NOTEXIST);

}  /* end of check_if_edge() */


/************************************************************/
/* depth-first-search for component checking
 * this function recursively labels vertex 'v' and all its neighbours
 * as belonging to component 'c', using the order[] array to keep track of
 * which component each vertex belongs to.
 */
void
component_dfs(
  graph_type *graph,
  int order[],
  int v,
  int c)
{
  int i;

  order[v] = c;

  for (i = 0; i < graph->deg[v]; i++)
  {
    if ( order[graph->nbr[v][i]] != c )
      component_dfs(graph, order, graph->nbr[v][i],c);
  }

}  /* end of component_dfs() */


/************************************************************/
/* this function counts the number of components that the specified graph
 * has, and returns this number
 */
int
calc_graph_components(
  graph_type *graph)
{
  int i,j;
  int order[MAXVERT];

  for (i = 0; i < graph->numvert; i++)
    order[i]=0;

  j=0;
  for (i = 0; i < graph->numvert; i++)
  {
    if (order[i] == 0)
    {
      j++;
      component_dfs(graph, order, i,j);
    }
  }

  return j;

}  /* end of calc_graph_components() */


/************************************************************/
/* this function performs depth-first-search for detecting a cutpoint
 * returns CUTPNT_EXIST on first detection of a cutpoint, or returns
 * CUTPNT_NOTEXIST
 */
int
cutpoint_dfs(
  graph_type *graph,
  int vert,
  int back[],
  int dfsnumber[],
  int *dfnum)
{
  int loop;

  int nextvert;
  
  (*dfnum)++;
  dfsnumber[vert] = *dfnum;
  back[vert] = *dfnum;
  
  for (loop = 0; loop < graph->deg[vert]; loop++)
  {
    nextvert = graph->nbr[vert][loop];

    if (dfsnumber[nextvert] == 0) 
    {
      if ( cutpoint_dfs(graph, nextvert, back, 
			dfsnumber, dfnum) == CUTPNT_EXIST)
      {
        return CUTPNT_EXIST; 
      }
      if (back[nextvert] >= dfsnumber[vert]) 
      {
        /* vertex 'vert' is an articulation point */
        return CUTPNT_EXIST;
      }
      else 
      {
        if (back[nextvert] < back[vert]) 
          back[vert] = back[nextvert];
      }
    } 
    else  
    {
      if (dfsnumber[nextvert] < back[vert]) 
        back[vert] = dfsnumber[nextvert];
    }
  }

  return CUTPNT_NOTEXIST;

}  /* end of cutpoint_dfs() */


/************************************************************/
/* this function checks to see if a cutpoint (articulation point) exists, 
 * and returns CUTPNT_EXIST if one is found, or CUTPNT_NOTEXIST if one does 
 * _not_ exist in the specified graph
 *
 * this code originally written by Culberson, from Baase
 * assumption:  vertex #1 has degree > 0
 */
int
check_graph_cutpoints(
  graph_type *graph)
{
  int dfsnumber[MAXVERT];
  int back[MAXVERT];		/* record shallowest back edge point */
  int dfnum;			/* current depth first number */

  int loop;
  int nextvert;

  for (loop = 0; loop < graph->numvert; loop++)
    dfsnumber[loop] = 0;

  /* handle root case */
  back[0] = 1;
  dfsnumber[0] = 1;
  nextvert = graph->nbr[0][0];
  
  dfnum = 1;

  if (cutpoint_dfs(graph, nextvert, back, dfsnumber, &dfnum) == CUTPNT_EXIST) 
    return CUTPNT_EXIST;

  /* check if root vertex is a cutpoint */
  for (loop = 1; loop < graph->deg[0]; loop++)
  {
    if (dfsnumber[graph->nbr[0][loop]] == 0)
    {
      /* root vertex 0 is a cutpoint */
      return CUTPNT_EXIST;
    }
  }

  return CUTPNT_NOTEXIST;

}  /* end of check_graph_cutpoints() */


