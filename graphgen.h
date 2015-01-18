


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

