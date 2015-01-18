
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: graphdata.h
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


#ifndef _GRAPHDATA_H_
#define _GRAPHDATA_H_


/************************************************************/
/* graph data structure:  for sparse (bounded degree) graphs only
 *
 * vertices numbered 0 ... n-1
 * each vertex v has its degree in deg[v]
 * each vertex v has its neighbors in locations x = 0 ... deg[v]-1
 *   of nbr[v][x]
 */
/************************************************************/

/* limits on size of graph allowed */
#define MAXVERT 1600
#define MAXDEGREE 50

typedef struct {
  int numvert;                  /* number of vertices */
  int deg[MAXVERT];             /* degree of each vertex */

  int nbr[MAXVERT][MAXDEGREE];  /* neigbhours of each vertex */

  int numedges;			/* total number of edges */
  float meandeg;		/* mean vertex degree */
  float stddevdeg;		/* deviation in vertex degree */
  int mindeg;			/* minimum vertex degree */
  int maxdeg;			/* maximum vertex degree */

  int deghistogram[MAXDEGREE];	/* histogram of vertex degrees */

  int solve;			/* solvable flag for current problem */
				/* use HC_FOUND, HC_NOT_EXIST */

  } graph_type;


/* graph file information */
#define GFILE_EXT ".graph"
#define GFILE_COMMENT "#"

/* defines for function return values */
#define CUTPNT_NOTEXIST 0
#define CUTPNT_EXIST 1

/* defines for 2 functions: rm_edge_graph, check_if_edge */
#define EDGE_NOTEXIST 	0
#define EDGE_REMOVE 	1
#define EDGE_EXIST	1

/************************************************************/
/* function declarations */
/************************************************************/

int output_graph( FILE *fp, graph_type *graph, char comments[]);

void output_fortran_graph( FILE *fp, graph_type *graph);

int input_graph( FILE *fp, graph_type *graph);

void calc_graph_stats( graph_type *graph);

void print_graph_stats( FILE *fp, graph_type *graph);

void init_graph( graph_type *graph);

void copy_graph( graph_type *source, graph_type *dest);

void add_direct_edge_graph( graph_type *graph, int v1, int v2);

void add_edge_graph( graph_type *graph, int v1, int v2);

int rm_edge_graph( graph_type *graph, int x, int y);

int check_if_edge( graph_type *graph, int x, int y);

int calc_graph_components( graph_type *graph);

int check_graph_cutpoints( graph_type *graph);

#endif /* _GRAPHDATA_H_ */

