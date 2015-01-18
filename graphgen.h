
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: graphgen.h
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


#ifndef _GRAPHGEN_H_
#define _GRAPHGEN_H_


/************************************************************/
/* defines and data structures */
/************************************************************/

/* defines for various parameters to geometric graph generator */

#define CONNECT_NEAR 0
#define CONNECT_FAR 1

#define GRAPH_WRAP 0
#define GRAPH_NOWRAP 1

#define MAXBOARDSIZE 100

#define HAM_DONTCARE 0
#define HAM_ENSURE 1

#define MAXNUMADDPATHS 2

/* options structure */

typedef struct {
  
  /* common options */
  int nvertex;		/* number of vertices */
  int makeham;		/* make sure graph is hamilton */
                        /* (HAM_ENSURE or HAM_DONTCARE */
  
  /* geometric graph options */
  float dist;		/* distance */
  int dim;		/* # of dimensions */
  int dflag;		/* distance flag = CONNECT_NEAR or CONNECT_FAR */
  int wrapflag;		/* wrap flag = GRAPH_WRAP or GRAPH_NOWRAP */
  int mindeg;		/* minimum degree that graph is forced to have */
  
  /* degreebound graph options */
  int degsize;		/* largest degree with non-zero percentage */
  float degpercent[MAXDEGREE];	/* array of percentages for each degree */

  /* knighttour parameters */
  int board1;
  int board2;
  int move1;
  int move2;

  /* crossroads parameters */
  int numsubgraphs;

  /* random graph parameters */
  float meandeg;	/* mean vertex degree */
  float degconst;	/* meandeg = degconst * (ln n + ln ln n) */

  /* add-cycle graph parameters */
  float numcycles;

  /* add-path graph parameters */
  float pathlengths[MAXNUMADDPATHS];

  /* ICCS graph parameters */
/* int numsubgraphs; */	/* already a crossroads parameter */
  int indsetsize;

  }  graphgen_options_type;


/************************************************************/
/* function declarations */
/************************************************************/

void add_cycles_to_graph( graph_type *graph, float ncycles);

void gen_iccs_graph( graph_type *graph, int subgraphs, int indsetsize);

void gen_addpath_graph( graph_type *graph, int nvertex, int numpath,
	float pathlen[]);

void gen_addcycle_graph( graph_type *graph, int nvertex, float ncycles);

void gen_random_graph( graph_type *graph, int numvert, int numedges);

void gen_crossroads_graph( graph_type *graph, int subgraphs);

void gen_knighttour_graph( graph_type *graph, int boardx, int boardy, 
	int move1, int move2);

void gen_old_degbound_graph( graph_type *graph, int numvert, float meandeg, 
	int var);

void gen_degbound_graph( graph_type *graph, int numvert, int degsize, 
	float degpercent[]);

void gen_geo_graph( int size, double dist2, int dflag, int dim, int wrap,
        graph_type *graph);

int parse_graphgen_options( FILE *fp, char word[], int gengraphtype);

void init_graphgen_options( );

void print_graphgen_options( FILE *fp);


#endif /* _GRAPHGEN_H_ */

