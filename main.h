
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend and Joe Culberson
 * Email: basil@cs.ualberta.ca
 * File: main.h
 * Contents: includes all the other .h files in the correct order
 *   this file should be included at the top of every .c file 
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


#ifndef _MAIN_H_
#define _MAIN_H_

/************************************************************/
/* system includes */
/************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <strings.h>
#include "getopt.h"


/************************************************************/
/* macros/defines */
/************************************************************/

/* math type defines */
#define EPSILON (0.0000001)
#define PI (3.14159)
#define LARGEVAL (10000000)

/* string type defines */
#define STRLEN (256)


/* return codes */
#define RET_OK 0
#define RET_ERROR 1

/* error handling macros */
#define WARN_ERROR(mesg) { fprintf(stderr,mesg); }
#define WARN_ERROR1(mesg,arg) { fprintf(stderr,mesg, arg); }

#define EXIT_ERROR(mesg) { fprintf(stderr,mesg);  exit(1); }
#define EXIT_ERROR1(mesg,arg) { fprintf(stderr,mesg,arg);  exit(1); }


/************************************************************/
/* project includes */
/************************************************************/

#include "graphdata.h"		/* needs to be at the top */
#include "stats.h"		/* needs to be near the top */

#include "graphgen.h"

#include "hamcycle.h"

#include "backtrack.h"
#include "heuristic.h"

#include "tester.h"		/* needs to be near the top */
#include "options.h"	/* must be below other module includes */


/************************************************************/
/* warning-avoidance function declarations */
/************************************************************/

int fprintf();
int fscanf();
int fflush();
int printf();
int sscanf();
int scanf();
/* char toupper(); */
time_t time();
/* void fclose(); */
void srand48();
long lrand48();
long clock();
void rewind();
int system();
/* extern void srandom(int seed);
extern long random(); */
extern  int getrusage(int who, struct rusage *rusage);

/* defines for print_info_summary() function */
#define PRINT_INFO_HEADER 	0
#define PRINT_INFO_ALL 		1

/************************************************************/
/* function declarations */
/************************************************************/

void print_info_summary( stat_exp_type *stats, FILE *fp, int printflag);


#endif /* _MAIN_H_ */

