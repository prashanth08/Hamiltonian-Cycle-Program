

#ifndef _MAIN_H_
#define _MAIN_H_

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

