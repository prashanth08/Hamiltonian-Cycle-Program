
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: hamcycle.h
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


#ifndef _HAMCYCLE_H_
#define _HAMCYCLE_H_


/************************************************************/
/* defines and data structures */
/************************************************************/

/* data structure for hamiltonian cycle algorithms:  path structures */

/* to be indexed by path position */
typedef struct {
  int gvert;    /* graph vertex # */
  int next;     /* points to location in this array of next vertex in path */
  } path_type;

/* to be indexed by graph vertex # */
typedef struct {
   int pathpos; /* position in path (-1 if not in path) */
   int ended;   /* length of path ended on this vertex (to avoid repeats) */
                /* for posa's only */
  } graphpath_type;


/* defines for prunelevel in hc_do_pruning() */
#define HC_PRUNE_NONE  0x0
#define HC_PRUNE_BASIC 0x1
#define HC_PRUNE_CYC   0x2
#define HC_PRUNE_CONNECT 0x4
#define HC_PRUNE_CUTPOINT 0x8
#define HC_PRUNE_ALL (HC_PRUNE_BASIC | HC_PRUNE_CYC | HC_PRUNE_CONNECT | \
HC_PRUNE_CUTPOINT)

/* data structures for saving deleted edges */
typedef struct {
  int v1;
  int v2;
  } edge_type;

typedef struct {
  edge_type stack[(MAXVERT*MAXDEGREE/2)];
  int pointer;
  } edgestack_type;

/* defines for the possible return values from the posa algorithm
 * hc_not_found - no cycle found, but one may exist
 * hc_not_exist - no cycle can exist on this graph
 */
#define HC_FOUND 0
#define HC_NOT_EXIST 1
#define HC_NOT_FOUND 2

/* defines for return values for hc_verify_solution */
#define HC_NOT_VERIFY 0
#define HC_VERIFY 1

/* values for selectflag parameter for select_initvert() */
#define INITVERT_RANDOM 0
#define INITVERT_MAXDEG 1
#define INITVERT_RANDEG 2
#define INITVERT_FIRST 3

/* return values for hc_check_timelimit() */
#define HC_QUIT 0
#define HC_CONTINUE 1

/* return values for test_hc_alg() */
#define RUN_NORMAL 0
#define RUN_TIMELIMIT 1

/************************************************************/
/* function declarations */
/************************************************************/

int hc_check_timelimit();

int hc_verify_solution( graph_type *graph, int solution[]);

void push_edge_to_stack( int v1, int v2, edgestack_type *edges);

void pop_edge_from_stack( int *v1, int *v2, edgestack_type *edges);

void copy_edgestack( edgestack_type *fromstack, edgestack_type *tostack);

void init_edgestack( edgestack_type *edges);

int get_curpnt_edgestack( edgestack_type *edges);

int select_initvertex( graph_type *graph, int selectflag);

int hc_do_pruning( graph_type *graph, int *prune, int prunelevel,
  edgestack_type *edgestack);

void add_vert_to_path( path_type path[], graphpath_type graphpath[], 
  int *pstart, int *pend, int *plength, int vert);

void remove_endvert_from_path( path_type path[], graphpath_type graphpath[],
  int *pstart, int *pend, int *plength, int oldend);

void hc_reverse_path( path_type path[], graphpath_type graphpath[],
  int *endpathv, int revpathv, int plength);

int hc_path_to_cycle( graph_type *graph, path_type path[],
  graphpath_type graphpath[], int *pstart, int *pend, int plength);

int test_hc_alg( graph_type *graph, stat_pertrial_type *trialstats);


#endif /* _HAMCYCLE_H_ */

