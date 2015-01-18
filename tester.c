
#define IN_TESTER_FILE

/* project includes */
#include "main.h"


/************************************************************/
/* this function prints out a string summarizing the
 * current experiment (for the summary file)
 * information in g_options is used
 * the passed in string is used to pass the information back
 * note: the passed in string must already be allocated
 */
void
print_experiment_summary(
  char expstr[])
{
  char tmpstr[40];
  int loop;

  expstr[0] = '\0';

  /* specify graph, according to type */
  switch (g_options.graphgentype)
  {
    case GEN_ICCS:
      sprintf(expstr, "ic%di%d", g_options.graphgen.numsubgraphs,
	g_options.graphgen.indsetsize);
      break;

    case GEN_ADDPATH:
      sprintf(expstr, "ap%dp%.2fp%.2f", g_options.graphgen.nvertex,
          g_options.graphgen.pathlengths[0], 
          g_options.graphgen.pathlengths[1]);
      break;

    case GEN_ADDCYCLE:
      sprintf(expstr, "ac%dc%.2f", g_options.graphgen.nvertex,
          g_options.graphgen.numcycles);
      break;

    case GEN_RANDOM:
      if (g_options.graphgen.makeham != HAM_ENSURE)
        sprintf(expstr, "ra%d", g_options.graphgen.nvertex);
      else
        sprintf(expstr, "rah%d", g_options.graphgen.nvertex);

      if (g_options.graphgen.degconst > 0.0)
        sprintf(tmpstr, "d%.3f", g_options.graphgen.degconst);
      else
        sprintf(tmpstr, "m%.3f", g_options.graphgen.meandeg);

      strcat(expstr,tmpstr);
      break;

    case GEN_CROSSROADS:
      sprintf(expstr, "cr%d", g_options.graphgen.numsubgraphs);
      break;

    case GEN_DEGREEBOUND:
      if (g_options.graphgen.makeham != HAM_ENSURE)
        sprintf(expstr, "db%d", g_options.graphgen.nvertex);
      else
        sprintf(expstr, "dbh%d", g_options.graphgen.nvertex);

      for (loop = 2; loop <= g_options.graphgen.degsize; loop++)
      {
        if (g_options.graphgen.degpercent[loop] == 0)
        {
          continue;
        }
        sprintf(tmpstr, "d%dp%.3f", loop, g_options.graphgen.degpercent[loop]);
        strcat(expstr, tmpstr);
      }
      break;

    case GEN_KNIGHTTOUR:
      sprintf(expstr, "ktm%dx%db%dx%d",
        g_options.graphgen.move1, g_options.graphgen.move2,
        g_options.graphgen.board1, g_options.graphgen.board2);
      break;

   case GEN_GEOMETRIC:
      sprintf(expstr, "ge%dd%.4fd%d", g_options.graphgen.nvertex,
        g_options.graphgen.dist, g_options.graphgen.dim);
      if (g_options.graphgen.dflag == CONNECT_FAR)
      {
        strcat(expstr, "f");
      }
      if (g_options.graphgen.wrapflag == GRAPH_NOWRAP)
      {
        strcat(expstr, "n");
      }
      if (g_options.graphgen.mindeg > 0)
      {
        sprintf(tmpstr, "d%d", g_options.graphgen.mindeg);
        strcat(expstr, tmpstr);
      }
      break;

  }  /* end of switch on graph type */

  /* add separator */
  strcat(expstr, "-");

  /* specify experiments, according to type */
  switch(g_options.algorithm)
  {
    case ALG_NOPRUNE_BT:
      strcat(expstr, "nb");
      break;

    case ALG_BACKTRACK:
      strcat(expstr, "ba");
      if (g_options.bt_alg.restart_increment > 1)
      {
        sprintf(tmpstr, "i%d", g_options.bt_alg.restart_increment);
        strcat(expstr,tmpstr);
      }

      strcat(expstr, "O");

      /* initial vertex selection strategy */
      switch (g_options.bt_alg.initvertflag)
      {
        case INITVERT_RANDOM:
          strcat(expstr, "r");
          break;
        case INITVERT_MAXDEG:
          strcat(expstr, "m");
          break;
        case INITVERT_RANDEG:
          strcat(expstr, "d");
          break;
      }

      /* degree sort strategy */
      switch (g_options.bt_alg.degsortflag)
      {
        case DEGSORT_RAND:
          strcat(expstr, "r");
          break;
        case DEGSORT_MIN:
          strcat(expstr, "i");
          break;
        case DEGSORT_MAX:
          strcat(expstr, "d");
          break;
      }

      /* graph pruning/checking options */
      if (g_options.bt_alg.pruneoptflag & HC_PRUNE_BASIC) 
        strcat(expstr, "b");
      if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CYC) 
        strcat(expstr, "c");
      if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CONNECT)
        strcat(expstr, "o");
      if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CUTPOINT)
        strcat(expstr, "a");

      break;

    case ALG_POSA_HEUR:
      strcat(expstr, "ph");

      /* check if have options to specify */
      if ( (g_options.heur_alg.visitflag == VISIT_RAND) &&
           (g_options.heur_alg.completeflag == COMPLETE_NORM) )
      {
        /* normal posa's, no options to specify */
        break;
      }

      strcat(expstr, "O");
      if (g_options.heur_alg.visitflag == VISIT_SMART)
        strcat(expstr, "v");
      if (g_options.heur_alg.completeflag == COMPLETE_SMART)
        strcat(expstr, "s");
      if (g_options.heur_alg.cycleextendflag == CYCLEEXTEND)
        strcat(expstr, "c");
      break;

  }  /* end of switch statment for algorithms */

  /* add separator */
  strcat(expstr, "-");

  /* add experimental information */
  if (g_options.savegraph == 1)
  {
    strcat(expstr, "s");
  }
  sprintf(tmpstr, "i%dg%d", g_options.num_instance_tests,
    g_options.num_graph_tests);

  strcat(expstr, tmpstr);

}  /* end of print_experiment_summary() */


/************************************************************/
/*  this function reads the next experiment string from the specified file
 *  skipping any comments that are encounteredd
 */
int 
get_next_expstr( 
  FILE *fp, 
  char expstr[]) 
{
  int wordtype;
  int status;

  /* parse test file for next experiment string */
  do
  {
    status = read_next_word(fp, expstr);

    if (status & READ_EOF)
      break;

    if (status & READ_STRLONG)
      fprintf(stderr,"Warning:  truncated string while reading test file.\n");

    wordtype = check_word(expstr);

    /* if comment, then skip to end of line */
    if (wordtype == WORD_COMMENT)
    {
      status = read_endofline(fp, expstr);

      if (status & READ_EOF)
        break;
    }
  }  while (wordtype == WORD_COMMENT);

  return (status);

}  /* end of get_next_expstr() */


/************************************************************/
/* this function gets a substring of specified length, starting at
 * a specified index, from a given string
 * the function returns the number of characters read
 */
int
get_substring(
  char substr[],
  char sourcestr[],
  int index,
  int strlen)
{
  int loop;

  for (loop = 0; loop < strlen; loop++)
  {
    substr[loop] = sourcestr[loop+index];
    if (sourcestr[loop+index] == '\0')
    {
      loop++;
      break;
    }
  }
  substr[loop] = '\0';	/* terminate string correctly */
  return (loop);

}  /* end of get_substring() */


/************************************************************/
/* this function gets a substring that consists of a number 
 * starting at a specified index, from a given string
 *
 * the function returns the number of characters read
 * note that negative numbers are not allowed 
 *  (just advance index past negative sign)
 */
int
get_number_substring(
  char substr[],
  char sourcestr[],
  int index)
{
  int loop = 0;
  int done = 0;
  char ch;

  do
  {
    ch = sourcestr[loop+index];
    if ( (ch >= '0' && ch <= '9') || (ch == '.') )
    {
      substr[loop] = sourcestr[loop+index];
      loop++;
    }
    else
      done = 1;
  }  while (!done);

  substr[loop] = '\0';
  return (loop);

}  /* end of get_number_substring() */


/************************************************************/
/* this function parses the specified experiment summary string, and
 * saves the resulting information in the global options structure (g_options)
 */
void
parse_experiment_string_for_options(
  char expstr[])
{
  int index = 0; 
  char str[STRLEN];
  int done;
  int degree;

  /* get first 2 letters to determine type of graph */
  index += get_substring(str, expstr, index, 2);

  if (strcasecmp(str, "ic") == 0)
  {
    g_options.graphgentype = GEN_ICCS;
  }
  else if (strcasecmp(str, "ap") == 0)
  {
    g_options.graphgentype = GEN_ADDPATH;
  }
  else if (strcasecmp(str, "ac") == 0)
  {
    g_options.graphgentype = GEN_ADDCYCLE;
  }
  else if (strcasecmp(str, "ra") == 0)
  {
    g_options.graphgentype = GEN_RANDOM;
  }
  else if (strcasecmp(str, "cr") == 0)
  {
    g_options.graphgentype = GEN_CROSSROADS;
  }
  else if (strcasecmp(str, "db") == 0)
  {
    g_options.graphgentype = GEN_DEGREEBOUND;
  }
  else if (strcasecmp(str, "kt") == 0)
  {
    g_options.graphgentype = GEN_KNIGHTTOUR;
  }
  else if (strcasecmp(str, "ge") == 0)
  {
    g_options.graphgentype = GEN_GEOMETRIC;
  }
  else 
  {
    fprintf(stderr, "Unrecognized graph generation type `%s' in "
		    "experiment `%s'.\n", str, expstr);
    EXIT_ERROR(" ");
  }

  /* initialize graph options */
  init_graphgen_options();

  /* parse graph generation options based on graph type */
  switch(g_options.graphgentype)
  {
    case GEN_ICCS:
      /* get # of subgraphs */
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.numsubgraphs = atoi(str);

      /* get independent set size */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'i') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.indsetsize = atoi(str);
      break;

    case GEN_ADDPATH:
      /* get # of vertices */
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.nvertex = atoi(str);

      /* get pathlengths parameter */
      /* this section assumes only 2 paths */
      /* path #1 */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'p') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      sscanf(str, "%f", &(g_options.graphgen.pathlengths[0]) );

      /* path #2 */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'p') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      sscanf(str, "%f", &(g_options.graphgen.pathlengths[1]) );
      break;

    case GEN_ADDCYCLE:

      /* get # of vertices */
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.nvertex = atoi(str);

      /* get numcycles parameter */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'c') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      sscanf(str, "%f", &(g_options.graphgen.numcycles) );
      break;

    case GEN_RANDOM:
      /* check for hamiltonicity flag */
      if (expstr[index] == 'h')
      {
        index += get_substring(str, expstr, index, 1);
        if (str[0] != 'h')
        {
          EXIT_ERROR1("Error reading `h' flag in experiment `%s'.\n",expstr);
        }
        g_options.graphgen.makeham = HAM_ENSURE;
      }

      /* get # of vertices */
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.nvertex = atoi(str);

      /* get next parameter */
      index += get_substring(str, expstr, index, 1);

      if (str[0] == 'd')   /* degconst */
      {
        index += get_number_substring(str, expstr, index);
        sscanf(str, "%f", &(g_options.graphgen.degconst));
      }
      else if (str[0] == 'm') 	/* mean degree */
      {
        index += get_number_substring(str, expstr, index);
        sscanf(str, "%f", &(g_options.graphgen.meandeg));
      }
      else
      {
        EXIT_ERROR("Bad format of experiment string.\n");
      }
      break;

    case GEN_CROSSROADS:
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.numsubgraphs = atoi(str);
      break;

    case GEN_DEGREEBOUND:

      /* check for hamiltonicity flag */
      if (expstr[index] == 'h')
      {
        index += get_substring(str, expstr, index, 1);
        if (str[0] != 'h')
        {
          EXIT_ERROR1("Error reading `h' flag in experiment `%s'.\n",expstr);
        }
        g_options.graphgen.makeham = HAM_ENSURE;
      }

      /* get # of vertices */
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.nvertex = atoi(str);

      done = 0;
      do  /* loop reading in degrees and percentages until reach separator */
      {
        /* check for degree/percentage set */
        index += get_substring(str, expstr, index, 1);
        if (str[0] == '-')        /* have separator */
        {
          /* restore index to read separator */
          index--;
          done = 1;
        }
        else if (str[0] == 'd')   /* degree/percentage set */
        {
          index += get_number_substring(str, expstr, index);
          degree = atoi(str);

          index += get_substring(str, expstr, index, 1);
          if (str[0] != 'p')
          {
            EXIT_ERROR("Bad format of experiment string.\n");
          }

          index += get_number_substring(str, expstr, index);
          sscanf(str,"%f",&(g_options.graphgen.degpercent[degree]));

          if (g_options.graphgen.degsize < degree)
          {
            g_options.graphgen.degsize = degree;
          }
        }
        else
        {
          EXIT_ERROR("Bad format of experiment string.\n");
        }
      } while (!done);
      break;

    case GEN_KNIGHTTOUR:

      /* get move #1 */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'm') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.move1 = atoi(str);

      /* get move #2 */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'x') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.move2 = atoi(str);

      /* get board size #1 */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'b') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.board1 = atoi(str);

      /* get board size #2 */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'x') EXIT_ERROR("Bad format of experiment string.\n");
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.board2 = atoi(str);

      /* calculate number of vertices */
      g_options.graphgen.nvertex = g_options.graphgen.board1 * 
					g_options.graphgen.board2;
      break;

    case GEN_GEOMETRIC:

      /* get # of vertices */
      index += get_number_substring(str, expstr, index);
      g_options.graphgen.nvertex = atoi(str);

      EXIT_ERROR("Geometric graph automated testing not implemented yet.\n");
      /* @@ geometric graph automated testing */
      break;

  }  /* end of graphgentype switch statement */

  /* finished parsing graph generation, read in separator */
  index += get_substring(str, expstr, index, 1);
  if (str[0] != '-') EXIT_ERROR("Bad format of experiment string.\n");

  /* read in algorithm */
  /* get first 2 letters to determine type of algorithm */
  index += get_substring(str, expstr, index, 2);

  if (strcasecmp(str, "nb") == 0)
  {
    g_options.algorithm = ALG_NOPRUNE_BT;
  }
  else 
  if (strcasecmp(str, "ba") == 0)
  {
    g_options.algorithm = ALG_BACKTRACK;
  }
  else 
  if (strcasecmp(str, "ph") == 0)
  {
    g_options.algorithm = ALG_POSA_HEUR;
  }
  else
  {
    fprintf(stderr, "Unrecognized algorithm type `%s' in "
		    "experiment `%s'.\n", str, expstr);
    EXIT_ERROR(" ");
  }

  /* parse algorithm options based on algorithm type */
  switch(g_options.algorithm)
  {
    case ALG_BACKTRACK:
      /* set default options */
      init_backtrack_alg_options();

      /* read in next character (the 'O') 
         or it could be the optional iterated restart 'i'
       */
      index += get_substring(str, expstr, index, 1);
      if ((str[0] != 'O') && (str[0] != 'i') )
      {
        fprintf(stderr, "Missing option specifier 'O' for pruning backtrack ");
        fprintf(stderr, "in experiment `%s'.\n", expstr);
        EXIT_ERROR(" ");
      }

      /* read in iterated restart increment value */
      if (str[0] == 'i')
      {
        index += get_number_substring(str, expstr, index);
        g_options.bt_alg.restart_increment = atoi(str);

        /* now read in 'O' */
        index += get_substring(str, expstr, index, 1);
        if (str[0] != 'O')
        {
          fprintf(stderr, "Missing option specifier 'O' for pruning ");
          fprintf(stderr, "backtrack in experiment `%s'.\n", expstr);
          EXIT_ERROR(" ");
        }
      }
        
      /* read in initial vertex selection strategy */
      index += get_substring(str, expstr, index, 1);
   
      if (str[0] == 'r')
        g_options.bt_alg.initvertflag = INITVERT_RANDOM;
      else if (str[0] == 'm')
        g_options.bt_alg.initvertflag = INITVERT_MAXDEG;
      else if (str[0] == 'd')
        g_options.bt_alg.initvertflag = INITVERT_RANDEG;
      else
      {
        fprintf(stderr, "Bad option %c specified for pruning ", str[0]);
        fprintf(stderr, "backtrack in experiment `%s'.\n", expstr);
        EXIT_ERROR(" ");
      }

      /* read in degree sorting method */
      index += get_substring(str, expstr, index, 1);
   
      if (str[0] == 'r')
        g_options.bt_alg.degsortflag = DEGSORT_RAND;
      else if (str[0] == 'i')
        g_options.bt_alg.degsortflag = DEGSORT_MIN;
      else if (str[0] == 'd')
        g_options.bt_alg.degsortflag = DEGSORT_MAX;
      else
      {
        fprintf(stderr, "Bad option %c specified for pruning ", str[0]);
        fprintf(stderr, "backtrack in experiment `%s'.\n", expstr);
        EXIT_ERROR(" ");
      }

      /* read in pruning options, stop when reach separator or end */
      while(1)
      {
        index += get_substring(str, expstr, index, 1);

        /* exit condition from this parse-prune-bt-options loop */
        if (str[0] == '-' || str[0] == '\0')
        {
          index --;
          break;
        }

        switch(str[0])
        {
          case 'b':
            g_options.bt_alg.pruneoptflag |= HC_PRUNE_BASIC;
            break;
          case 'c':
            g_options.bt_alg.pruneoptflag |= HC_PRUNE_CYC;
            break;
          case 'o':
            g_options.bt_alg.pruneoptflag |= HC_PRUNE_CONNECT;
            break;
          case 'a':
            g_options.bt_alg.pruneoptflag |= HC_PRUNE_CUTPOINT;
            break;
          default:
            fprintf(stderr, "Unrecognized pruning backtrack option `%c' in "
  		    "experiment `%s'.\n", str[0], expstr);
            EXIT_ERROR(" ");
            break;
        }  /* end of switch statement */
      }  /* end of while loop */
      break;

    case ALG_POSA_HEUR:
      /* set normal heuristic options */
      init_heuristic_alg_options();

      /* read in next character: if O, then options are specified */
      index += get_substring(str, expstr, index, 1);
      if (str[0] != 'O') 
      {
        /* no options, so continue with parsing */
        index --;
        break;
      }
     
      /* read in options, stop when reach separator */
      while(1)
      {
        index += get_substring(str, expstr, index, 1);

        /* exit condition from this parse-heuristic-options loop */
        if (str[0] == '-' || str[0] == '\0')
        {
          index --;
          break;
        }

        switch(str[0])
        {
          case 'v':
            g_options.heur_alg.visitflag = VISIT_SMART;
            break;
          case 's':
            g_options.heur_alg.completeflag = COMPLETE_SMART;
            break;
          case 'c':
            g_options.heur_alg.cycleextendflag = CYCLEEXTEND;
            break;
          default:
            fprintf(stderr, "Unrecognized heuristic algorithm  option `%c' "
  		    "in experiment `%s'.\n", str[0], expstr);
            EXIT_ERROR(" ");
            break;
        }  /* end of switch statement */
      }  /* end of parse-heuristic-options loop */
      break;

  }  /* end of switch statement for parsing algorithm options */

  /* finished parsing algorithm, read in separator */
  index += get_substring(str, expstr, index, 1);
  if (str[0] != '-') EXIT_ERROR("Bad format of experiment string.\n");

  /* parse test data */
  while(1)
  {
    index += get_substring(str, expstr, index, 1);

    /* termination conditions */
    if (str[0] == '\0')
      break;

    switch(str[0])
    {
      case '\0':
        /* reached end of string */
        break;

      case 't':
	g_options.alg_timelimit = -1;	/* turn off time limit */
	break;

      case 's': 
        g_options.savegraph = SAVEGRAPH;
        break;

      case 'i':
        index += get_number_substring(str, expstr, index);
        g_options.num_instance_tests = atoi(str);
        break;
      
      case 'g':
        index += get_number_substring(str, expstr, index);
        g_options.num_graph_tests = atoi(str);
        break;
      
      default:
        fprintf(stderr, "Unrecognized test option `%c' in "
  		    "experiment `%s'.\n", str[0], expstr);
        EXIT_ERROR(" ");
        break;
    }  /* end of parse-test-data switch statement */
  }  /* end of parse-test-data-loop */  

}  /* end of parse_experiment_string_for_options() */


/************************************************************/
/* this function creates an .opt file for running a test
 * using the current options (g_options)
 */
void
create_opt_file(
  FILE *fp,
  char expstr[])
{
  int loop;

  fprintf(fp, "# Created using automated tester\n");
  fprintf(fp, "# Experiment = %s\n", expstr);

  /* specify graph generation method */
  switch (g_options.graphgentype)
  {
    case GEN_ICCS:
      fprintf(fp, "\n-graphgen iccs");
      fprintf(fp, " +subgraphs=%d +indsetsize=%d ",
	g_options.graphgen.numsubgraphs, g_options.graphgen.indsetsize);
      break;
      
    case GEN_ADDPATH:
      fprintf(fp, "\n-graphgen addpath");
      fprintf(fp, " +nvertex=%d +path1=%.3f +path2=%.3f ",
	g_options.graphgen.nvertex,
	g_options.graphgen.pathlengths[0], 
	g_options.graphgen.pathlengths[1]);
      break;

    case GEN_ADDCYCLE:
      fprintf(fp, "\n-graphgen addcycle");
      fprintf(fp, " +nvertex=%d +numcycles=%.3f ",
	g_options.graphgen.nvertex, g_options.graphgen.numcycles);
      break;

    case GEN_RANDOM:
      fprintf(fp, "\n-graphgen random +nvertex=%d ",
	g_options.graphgen.nvertex);
      if (g_options.graphgen.degconst > 0.0)
        fprintf(fp, "+degconst=%.3f", g_options.graphgen.degconst);
      else
        fprintf(fp, "+meandeg=%.3f", g_options.graphgen.meandeg);
      break;

    case GEN_CROSSROADS:
      fprintf(fp, "\n-graphgen crossroads +subgraphs=%d", 
	g_options.graphgen.numsubgraphs);
      break;

    case GEN_DEGREEBOUND:
      fprintf(fp, "\n-graphgen degreebound");
      if (g_options.graphgen.makeham == HAM_ENSURE)
        fprintf(fp, " +ensureham");

      fprintf(fp, " +nvertex=%d", g_options.graphgen.nvertex);

      for (loop = 2; loop <= g_options.graphgen.degsize; loop++)
      {
        if (g_options.graphgen.degpercent[loop] == 0.0)
          continue;

        fprintf(fp, " +d%d=%.3f", loop, g_options.graphgen.degpercent[loop]);
      }
      break;

    case GEN_KNIGHTTOUR:
      fprintf(fp, "\n-graphgen knighttour ");
      fprintf(fp, "+move1=%d +move2=%d +board1=%d +board2=%d",
        g_options.graphgen.move1, g_options.graphgen.move2, 
        g_options.graphgen.board1, g_options.graphgen.board2); 
      break;

    case GEN_GEOMETRIC:
      EXIT_ERROR("Geometric graphs not implemented.\n"); /* @@ */
      break;

  }  /* end of graph generation switch statement */  

  /* specify algorithm */
  switch (g_options.algorithm)
  {
    case ALG_POSA_HEUR:
      fprintf(fp, "\n-algorithm posa_heur");
      if (g_options.heur_alg.visitflag == VISIT_SMART)
        fprintf(fp, " +smartvisit");
      if (g_options.heur_alg.completeflag == COMPLETE_SMART)
        fprintf(fp, " +smartcomplete");
      if (g_options.heur_alg.cycleextendflag == CYCLEEXTEND)
        fprintf(fp, " +cycleextend");
      break;

    case ALG_NOPRUNE_BT:
      fprintf(fp, "\n-algorithm noprune_bt");
      break;

    case ALG_BACKTRACK:
      fprintf(fp, "\n-algorithm backtrack");

      if (g_options.bt_alg.restart_increment > 1)
      {
        fprintf(fp, " +restart=%d", g_options.bt_alg.restart_increment);
      }

      switch( g_options.bt_alg.initvertflag )
      {
        case INITVERT_RANDOM:
          /* default case */
          break;
        case INITVERT_MAXDEG:
          fprintf(fp, " +initvert=maxdeg");
          break;
        case INITVERT_RANDEG:
          fprintf(fp, " +initvert=randeg");
          break;
      }

      switch( g_options.bt_alg.degsortflag )
      {
        case DEGSORT_RAND:
          /* default case */
          break;
        case DEGSORT_MAX:
          fprintf(fp, " +degsort=max");
          break;
        case DEGSORT_MIN:
          fprintf(fp, " +degsort=min");
          break;
      }

      if (g_options.bt_alg.pruneoptflag != HC_PRUNE_NONE)
      {
        fprintf(fp, " +pruneopt=");
        if (g_options.bt_alg.pruneoptflag & HC_PRUNE_BASIC)
          fprintf(fp, "B");
        if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CYC)
          fprintf(fp, "C");
        if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CONNECT)
          fprintf(fp, "O");
        if (g_options.bt_alg.pruneoptflag & HC_PRUNE_CUTPOINT)
          fprintf(fp, "A");
      }
      break;
  }

  /* specify test information */
  fprintf(fp, "\n-instancetests %d", g_options.num_instance_tests);
  fprintf(fp, "\n-graphtests %d", g_options.num_graph_tests);

  /* savegraph */
  if (g_options.savegraph == SAVEGRAPH)
    fprintf(fp, "\n-savegraph");

  /* timelimit */
  fprintf(fp, "\n-timelimit %d", g_options.alg_timelimit);

  /* don't need +alg report since RNGSEED printed in statfile */
  fprintf(fp, "\n-report +summary");

  fprintf(fp, "\n");

}  /* end of create_opt_file() */


/************************************************************/
/* this function initilizes the global option structure and sets defaults
 * according to the needs of the tester program
 * thus it is different from init_options() in options.c
 */
void
init_test_options()
{

  g_options.algorithm = ALG_NOSOLVE;
  g_options.graphgentype = GEN_NOGRAPH;

  /* time limit is 3600 seconds = 60 minutes */
  g_options.alg_timelimit = 60 * 60;

  g_options.savegraph = NOSAVEGRAPH;

  g_options.num_instance_tests = 1;
  g_options.num_graph_tests = 1;

}  /* end of init_test_options() */


/************************************************************/
/* this function handles automated testing, using a testfile
 * progname is the name of the program (for executing the tests)
 */
void
do_automated_testing(
  char progname[])
{
  FILE *test_fp;

  char opt_fn[STRLEN];
  FILE *opt_fp;

  char results_fn[STRLEN];
  FILE *results_fp;
  
  int status;

  int testnum = 0;
  char expstr[STRLEN];

  char cmdstr[STRLEN];

  char sum_fn[STRLEN];
  FILE *sum_fp;

  printf("Doing automated testing using file `%s'...\n", g_options.test_fn);

  /* open test file */
  test_fp = fopen( g_options.test_fn, "r");
  if (test_fp == NULL)
  {
    EXIT_ERROR1("Error opening test file `%s' for reading.\n", 
	g_options.test_fn);
  }

  /* create results filename, and open file */
  strcpy(results_fn, g_options.test_fn);
  strip_extension_str(results_fn);
  strcat(results_fn, ".results");
  printf("Test results are being save to file `%s'...\n", results_fn);

  results_fp = fopen( results_fn, "w");
  if (results_fp == NULL)
  {
    EXIT_ERROR1("Error opening results file `%s' for writing.\n", results_fn);
  }

  /* print header to results file */
  print_info_summary( (stat_exp_type *) NULL, results_fp, PRINT_INFO_HEADER);
  
  /* verify correctness of specified tests first
   * read in and parse all experiments, to see if they are all valid 
   */
  testnum = 0;
  while(1)
  {
    status = get_next_expstr(test_fp, expstr);

    if (status & READ_EOF)
      break;
    if (status & READ_STRLONG)
      fprintf(stderr,"Warning:  truncated string while reading test file.\n");

    /* have next experiment string */
    testnum++;

    /* parse experiment string to get options */
    parse_experiment_string_for_options(expstr);

    /* note: cannot generate new exp str to compare to original since
     *   there might be non-important formating differences that will
     *   cause the comparison to fail 
     */
  }

  /* reset file pointer to start */
  rewind(test_fp);
  testnum = 0;

  /* read in experiment strings from test file, and perform the specified
   * experiments
   */
  while (1)
  {

    /* parse test file for next experiment string */
    status = get_next_expstr(test_fp, expstr);

    if (status & READ_EOF)
      break;

    testnum++;

    /* initialize options */
    init_test_options();

    /* parse experiment string to get options */
    parse_experiment_string_for_options(expstr);
  
    /* create options filename, and open file */
    strcpy(opt_fn, expstr);
    strcat(opt_fn, ".opt");
    
    opt_fp = fopen( opt_fn, "w");
    if (opt_fp == NULL)
    {
      EXIT_ERROR1("Error opening options file `%s' for writing.\n", opt_fn);
    }

    /* create options .opt file using current options settings */
    create_opt_file(opt_fp, expstr);
    fclose(opt_fp);

    /* run test.  cannot just call perform_experiment()
     * because various initialization of output files and what not
     * is done in main()
     */
    sprintf(cmdstr, "%s -o %s > /dev/null", progname, opt_fn);
    printf("Running experiment '%s'...\n", cmdstr);

    system(cmdstr);
  
    /* parse .summary file, save results in .result file */
    strcpy(sum_fn, expstr);
    strcat(sum_fn, ".summary");
    
    sum_fp = fopen( sum_fn, "r");
    if (sum_fp == NULL)
    {
      EXIT_ERROR1("Error opening summary file `%s' for reading.\n", sum_fn);
    }

    /* skip first line */
    status = read_endofline(sum_fp, expstr);

    if (status & READ_EOF)
    {
      EXIT_ERROR1("Error reading summary file `%s'.\n", sum_fn);
    }

    /* read next line (that has test results) */
    status = read_endofline(sum_fp, expstr);

    if (status & READ_EOF)
    {
      EXIT_ERROR1("Error reading summary file `%s'.\n", sum_fn);
    }

    /* save line to .result file */
    fprintf(results_fp, "%s\n", expstr);
    fflush(results_fp);

    fclose(sum_fp);
    
  }  /* end of parse-for-next-experiment-string while loop */

  if (testnum == 0)
  {
    EXIT_ERROR("Test file contained no experiments.\n");
  }

}  /* end of do_automated_testing() */

