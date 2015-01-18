
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: stats.h
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

