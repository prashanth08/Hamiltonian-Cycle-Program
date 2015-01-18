

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

