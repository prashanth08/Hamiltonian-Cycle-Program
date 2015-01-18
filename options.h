
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: options.h
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


#ifndef _OPTIONS_H_
#define _OPTIONS_H_


/************************************************************/
/* options data structure:  
 *   note that this is a global, so is accessible by the entire
 *   software package.
 *
 *   note also that this structure contains option data structures
 *     for the different modules, so that the options.h file must
 *     be included after the other module .h files.
 *   
 */
/************************************************************/

typedef struct {


  char test_fn[STRLEN];		/* filename of testfile */

  /* if == ALG_NOSOLVE, then don't solve */
  int algorithm;

  /* maximum amount of time to let algorithm run for */
  int alg_timelimit;

  heuristic_alg_options_type heur_alg;
  backtrack_alg_options_type bt_alg;
  
  /* if == GEN_NOGRAPH, then don't generate a graph */
  int graphgentype;

  graphgen_options_type graphgen;

  /* filename of options file */
  char options_fn[STRLEN];	

  /* base filename for output files 
   * this is equal to the options filename with the extension stripped
   */
  char output_fn[STRLEN];

  /* if filename is empty, then don't load */
  char loadgraph_fn[STRLEN];	/* graph filename to load */

  /* if filename is empty, then don't save */
  char savegraph_fn[STRLEN];	/* graph filename to save */

  /* this is used only by tester.c code */
  int savegraph;	/* = NOSAVEGRAPH or SAVEGRAPH */
  
  int rng_seed;  /* random number generator seed */

  int num_instance_tests;	/* number of tests per instance of graph */
  int num_graph_tests;		/* number of different graphs to test on */

  int report_flags;	/* indicate which things to report */

  /* options for where to send the output */
  FILE *log_fp;
  FILE *sol_fp;
  FILE *stats_fp;
  FILE *options_fp;
  FILE *summary_fp;

  } options_type;

options_type g_options;


/* defines for the different options */

/* maximum length of option words */
#define OPTLEN	15

/* algorithms */
#define ALG_NOSOLVE 	0
#define ALG_NOPRUNE_BT	1
#define ALG_BACKTRACK	2
#define ALG_POSA_HEUR	3

#define NUM_ALG_OPT	4

#ifdef IN_OPTIONS_FILE
char opt_alg_str[NUM_ALG_OPT][OPTLEN] = {
                "NOSOLVE",
                "noprune_bt",
                "backtrack",
                "posa_heur" };
#else
extern char opt_alg_str[NUM_ALG_OPT][OPTLEN];
#endif

/* graph gen type */
#define GEN_NOGRAPH	0
#define GEN_GEOMETRIC 	1
#define GEN_DEGREEBOUND	2
#define GEN_KNIGHTTOUR	3
#define GEN_CROSSROADS	4
#define GEN_RANDOM	5
#define GEN_ADDCYCLE	6
#define GEN_ADDPATH	7
#define GEN_ICCS	8

#define NUM_GEN_OPT	9

#ifdef IN_OPTIONS_FILE
char opt_gen_str[NUM_GEN_OPT][OPTLEN] = {
                "NOSOLVE",
                "GEOMETRIC",
                "DEGREEBOUND",
                "KNIGHTTOUR",
		"CROSSROADS",
		"RANDOM",
		"ADDCYCLE",
		"ADDPATH",
		"ICCS"};
#else
extern char opt_gen_str[NUM_GEN_OPT][OPTLEN];
#endif


/* report flags - indicate which statistics to calculate/report */
#define REPORT_NONE	0
#define REPORT_GRAPH	0x01
#define REPORT_ALG	0x02
#define REPORT_SOLUTION	0x04
#define REPORT_OPTIONS	0x08
#define REPORT_SUMMARY	0x10

/* savegraph flag */
#define NOSAVEGRAPH 0
#define SAVEGRAPH 1


/************************************************************/
/* defines for option functions
 *   
 */
/************************************************************/

/* return codes from read_next_word() */
#define READ_OK		0x0
#define READ_STRLONG	0x1
#define READ_EOL	0x2
#define READ_EOF	0x4

/* flags to indicate type of word in option file */
#define WORD_NULL	0
#define WORD_EMPTY	1
#define WORD_ARG	2
#define WORD_PARM	3
#define WORD_COMMENT	4
#define WORD_OTHER	5


/************************************************************/
/* function declarations */
/************************************************************/

int read_next_word( FILE *fp, char word[]);

int read_endofline( FILE *fp, char line[]);

void strip_extension_str( char word[]);

void parse_parameter_str( char word[], char parmstr[], char valstr[]);

int check_word(char word[]);

void parse_option_file(char filename[]);

void init_options();

void print_options(FILE *fp);

#endif /* _OPTIONS_H_ */

