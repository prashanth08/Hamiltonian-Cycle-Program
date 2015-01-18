


#define IN_OPTIONS_FILE

/* project includes */
#include "main.h"


/* defines for parsing */
#define EOF (-1)
#define EOL '\n'
#define ISWHITE(x) ( ((x) == ' ') || ((x) == '\t') || ((x) == EOL) ) 


/************************************************************/
/* basic file parsing functions
 *
 */
/************************************************************/


/************************************************************/
/* this function returns the next whitespace-delineated word in the 
 * specified file pointer.
 * a status code is also returned, with different bits (flags) set
 * according to different conditions
 * READ_EOF:  reached end-of-file
 * READ_EOL:  read EOL after reading word.
 * READ_STRLONG: word was too long (>255 characters), so only the first
 *    		 255 characters were stored
 * 
 * parameters:
 *   FILE *fp: INPUT		: file pointer to read from
 *   char word[]: OUTPUT	: pre-allocated string (length STRLEN) to 
 *                        	  store next word
 *   int (normal return)	: status code (or'd bits READ_EOF, READ_EOL, 
 * 						READ_STRLONG), or = READ_OK
 */
int
read_next_word(
  FILE *fp,
  char word[])
{
  int retcode = READ_OK;

  char ch;
  int ret;
  int index = 0;

  word[0] = '\0';

  /* read till no more whitespace
   * note that EOL (end-of-line) is considered whitespace
   */
  do
  {
    ret = fscanf(fp, "%c", &ch);

    if (ret == EOF)
      return (retcode | READ_EOF);

  }  while ( ISWHITE(ch) );

  /* add character to word, and read next one, till end of word
   * (whitespace) or EOF
   */
  while ( !(ISWHITE(ch)) )
  {
    /* store character */
    word[index++] = ch;

    /* read next */
    ret = fscanf(fp, "%c", &ch);

    if (ch == EOL)
      retcode |= READ_EOL;

    if (ret == EOF)
    {
      word[index++] = '\0';
      return (retcode | READ_EOF);
    }

  }
   
  word[index++] = '\0';

  return(retcode);

}  /* end of read_next_word() */


/************************************************************/
/* this function reads from the specified file pointer to the end of 
 * the current line (or end of file)
 * the information read is returned
 *
 * a status code is also returned, with different bits (flags) set
 * according to different conditions
 * READ_EOF:  reached end-of-file
 * READ_EOL:  read EOL. 
 *
 * parameters:
 *   FILE *fp: INPUT            : file pointer to read from
 *   char line[]: OUTPUT        : pre-allocated string (length STRLEN) to
 *                                store next word
 *   int (normal return)        : status code (or'd bits READ_EOF, READ_EOL,
 *                                            READ_STRLONG)
 */
int
read_endofline(
  FILE *fp,
  char line[])
{
  char ch;
  int ret;
  int index = 0;

  line[0] = '\0';

  /* read character, and add to line (if not EOL, EOF) */
  while(1)
  {
    /* read next */
    ret = fscanf(fp, "%c", &ch);

    if (ret == EOF)
    {
      line[index++] = '\0';
      return (READ_EOF);
    }

    if (ch == EOL)
    {
      line[index++] = '\0';
      return (READ_EOL);
    }
    
    /* store character */
    line[index++] = ch;
  }

  /* bogus return to avoid warning */
  return (READ_OK);

}  /* end of read_endofline() */


/************************************************************/
/* this function strips an extension from the back of a filename,
 * and returns the shortened filename 
 * (if no extension, the original filename is returned)
 * note: an extension is a period, following by anything
 * ie: test.opt -->  test
 * ie: test2.5.3.opt ---> test2.5.3
 */
void
strip_extension_str(
  char word[])
{
  int loop;

  if (word == NULL)
    return;

  for (loop = strlen(word); loop >= 0; loop--)
  {
    if (word[loop] == '.')
      break;
  }

  if (word[loop] == '.')
    word[loop] = '\0';

}  /* end of strip_extension_str() */


/************************************************************/
/* this function parses the specified parameter 
 *
 * there are 2 possible formats
 * 1. +parmflag
 * 2. +parmvar=parmvalue
 *
 * +parmvar or +parmflag is placed in parmstr
 * parmvalue is placed in valstr
 *  if no parmvalue, valstr = empty
 *
 * assumptions:
 * the passed in word is a parameter (WORD_PARM)
 * parmstr and valstr are already allocated strings 
 */
void
parse_parameter_str(
  char word[],
  char parmstr[],
  char valstr[])
{
  int index;
  int valindex;

  /* make valstr empty */
  valstr[0] = '\0';

  /* copy word into parmstr till reach equals sign */
  for (index = 0; word[index] != '\0'; index++)
  {
    parmstr[index] = word[index];
    if (word[index] == '=')
      break;
  }
  parmstr[index] = '\0';

  /* if parameter is format 1, then finished */
  if (word[index] == '\0')
    return;
  else
  {
    /* format 2: get value str */
    for (valindex = index+1, index = 0; 
		word[valindex] != '\0'; valindex++, index++)
    {
      valstr[index] = word[valindex];
    }

    valstr[index] = '\0';
  }

}  /* end of parse_parmeter_str() */
      

/************************************************************/
/* this function checks the specified word to see if what it is.
 * returns WORD_NULL if the string is not allocated
 * returns WORD_EMPTY if the string is empty
 * returns WORD_ARG if an argument  "-X"
 * returns WORD_PARM if a parameter   "+X"
 * returns WORD_COMMENT if a comment "#"
 * returns WORD_OTHER if neither of the two
 */
int
check_word(
  char word[])
{
  if (word == NULL)
    return(WORD_NULL);

  if (word[0] == '\0')
    return(WORD_EMPTY);

  if (word[0] == '-')
    return(WORD_ARG);

  if (word[0] == '+')
    return(WORD_PARM);

  if (word[0] == '#')
    return(WORD_COMMENT);

  return(WORD_OTHER);

}  /* end of check_word() */


/************************************************************/
/* this function parses the option file arguments.
 * input parameters:  word : the argument
 * 		      fp   : file pointer to file, set to the parameters of
 *                           the argument
 * output parameters: word : the next argument/comment (non-parameter) word
 *                   return: the status of the most recent read 
 */ 
int
parse_argument(
  FILE *fp,
  char word[])
{
  /* this variable is set to 1 if an extra word was read.  note that if
   * readextra is not set by the end of the function, an additional call
   * to read_next_word() is made, to fulfill the expectations for 
   * parse_argument() 
   */
  int readextra = 0;

  int status;
  int loop;

  /* algorithm argument:
   * format: 
   *   -algorithm <algorithm> [+algparm1 [+algparm2] ...]
   */
  if (strcasecmp(word, "-algorithm") == 0)
  {
    /* read algorithm to use */
    status = read_next_word(fp, word);

    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -algorithm option.\n");
    }

    if (check_word(word) != WORD_OTHER)
    {
      EXIT_ERROR("Type of algorithm not specified for -algorithm option.\n");
    }

    /* check that valid algorithm is specified */
    for (loop = 0; loop < NUM_ALG_OPT; loop++)
    {
      if ( strcasecmp(word, opt_alg_str[loop]) == 0 )
      {
        g_options.algorithm = loop;
        break;
      }
    }

    if (loop == NUM_ALG_OPT)
    {
      EXIT_ERROR1("Invalid algorithm `%s' specified in option file.\n",word);
    }

    /* now read algorithm parameters */
    switch(g_options.algorithm)
    {
      case ALG_NOPRUNE_BT:
      case ALG_BACKTRACK:
        status = parse_backtrack_alg_options(fp, word);
        readextra = 1;  
        break;

      case ALG_POSA_HEUR:
        status = parse_heuristic_alg_options(fp, word);
        readextra = 1;  
        break;
    }
      
  }  /* end of -algorithm */ 
  else

  /* loadgraph argument:
   * format: 
   *   -loadgraph <filename>
   */
  if (strcasecmp(word, "-loadgraph") == 0)
  {
    /* read filename of graph to use */
    status = read_next_word(fp, word);

    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -loadgraph option.\n");
    }

    if (check_word(word) != WORD_OTHER)
    {
      EXIT_ERROR("Filename not specified for -loadgraph option.\n");
    }

    strcpy(g_options.loadgraph_fn, word);
  
  }  /* end of -loadgraph */ 
  else

  /* savegraph argument:
   * format: 
   *   -savegraph [<filename>]
   * If <filename> is not specified then the filename = base filename +
   * extension ".graph" + graph number if more than one graph
   */
  if (strcasecmp(word, "-savegraph") == 0)
  {
    /* read filename of graph to use */
    status = read_next_word(fp, word);

    /* check if filename was specified or not */
    if (check_word(word) != WORD_OTHER)
    {
      readextra = 1;      
      strcpy(g_options.savegraph_fn, g_options.output_fn);
      strcat(g_options.savegraph_fn, ".graph");
    }
    else
      strcpy(g_options.savegraph_fn, word);

  }  /* end of -savegraph */ 
  else

  /* graphgen argument:
   * format: 
   *   -graphgen <graphtype> [+parm1 [+parm2] ... ]
   */
  if (strcasecmp(word, "-graphgen") == 0)
  {
    /* read type of graph to generate */
    status = read_next_word(fp, word);

    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -graphgen option.\n");
    }

    if (check_word(word) != WORD_OTHER)
    {
      EXIT_ERROR("Filename not specified for -graphgen option.\n");
    }

    /* check that valid type of graph is specified */
    for (loop = 0; loop < NUM_GEN_OPT; loop++)
    {
      if ( strcasecmp(word, opt_gen_str[loop]) == 0 )
      {
        g_options.graphgentype = loop;
        break;
      }
    }

    if (loop == NUM_GEN_OPT)
    {
      EXIT_ERROR1("Invalid graph type `%s' specified in option file.\n", word);
    }

    /* now read graph type parameters */
    status = parse_graphgen_options(fp, word, g_options.graphgentype);
    readextra = 1;

  }  /* end of -graphgen */ 
  else

  /* report argument:
   * format: 
   *   -report +repparm1 +repparm2 +repparm3
   */
  if (strcasecmp(word, "-report") == 0)
  {
    /* read parameters till reach next argument 
     * parameters indicate which things to report:
     *   +GRAPH 	: graph statistics (default on)
     *   +ALG		: algorithm running statistics (default on)
     *   +SOLUTION	: print solution to problem if found (default on)
     *   +OPTIONS	: print option settings 
     *   +SUMMARY	: print one-line summary
     *
     * default = none
     */

    status = read_next_word(fp, word);
    readextra = 1;
 
    while ( !(status & READ_EOF) )
    {
      if (check_word(word) == WORD_PARM)
      {
        if (strcasecmp(word, "+graph") == 0)
          g_options.report_flags |= REPORT_GRAPH;
        else if (strcasecmp(word, "+alg") == 0)
          g_options.report_flags |= REPORT_ALG;
        else if (strcasecmp(word, "+solution") == 0)
          g_options.report_flags |= REPORT_SOLUTION;
        else if (strcasecmp(word, "+options") == 0)
          g_options.report_flags |= REPORT_OPTIONS;
        else if (strcasecmp(word, "+summary") == 0)
          g_options.report_flags |= REPORT_SUMMARY;
        else
        {
          WARN_ERROR1("Warning:  Unidentified parameter `%s'.\n", word);
        }
      }
      else
        break;

      status = read_next_word(fp, word);
    }  /* end of while loop */

  }  /* end of -report */ 
  else

  /* timelimit argument:
   * format: 
   *   -timelimit <time>
   */
  if ( (strcasecmp(word, "-timelimit")==0) )
  {
    status = read_next_word(fp, word);
 
    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -randseed option.\n");
    }

    /* word is WORD_ARG type if specify -1 for timelimit */
    if ( (check_word(word) != WORD_OTHER) && (check_word(word) != WORD_ARG))
    {
      EXIT_ERROR("Time limit not specified for -timelimit option.\n");
    }

    g_options.alg_timelimit = (int) atoi(word);

    if (g_options.alg_timelimit < -1) 
    {
      EXIT_ERROR1("Invalid time limit %s specified for -timelimit option.\n", word);
    }

  }  /* end of -timelimit */ 
  else

  /* random number seed argument:
   * format: 
   *   -randseed <seed>
   */
  if ( (strcasecmp(word, "-randseed") == 0) )
  {
    status = read_next_word(fp, word);
 
    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -randseed option.\n");
    }

    if (check_word(word) != WORD_OTHER)
    {
      EXIT_ERROR("Random seed not specified for -randseed option.\n");
    }

    g_options.rng_seed = (int) atoi(word);

  }  /* end of -randseed */ 
  else

  /* # of test per graph instance argument
   * format: 
   *   -instancetests <num>
   */
  if (strcasecmp(word, "-instancetests") == 0)
  {
    status = read_next_word(fp, word);
 
    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -instancetests option.\n");
    }

    if (check_word(word) != WORD_OTHER)
    {
      EXIT_ERROR("Number of tests not specified for -instancetests option.\n");
    }

    g_options.num_instance_tests = (int) atoi(word);

    if (g_options.num_instance_tests > MAXTRIALTESTS)
    {
      EXIT_ERROR("Number of tests specified for -instancetests option too large.\n");
    }

  }  /* end of -instancetests */ 
  else

  /* # of test per graph type argument
   * format: 
   *   -graphtests <num>
   */
  if (strcasecmp(word, "-graphtests") == 0)
  {
    status = read_next_word(fp, word);
 
    if (status & READ_EOF)
    {
      EXIT_ERROR("Unexpected end of file when processing -graphtests option.\n");
    }

    if (check_word(word) != WORD_OTHER)
    {
      EXIT_ERROR("Number of tests not specified for -graphtests option.\n");
    }

    g_options.num_graph_tests = (int) atoi(word);

    if (g_options.num_graph_tests > MAXGRAPHTESTS)
    {
      EXIT_ERROR("Number of tests specified for -graphtests option too large.\n");
    }

  }  /* end of -graphtests */ 
  else

  /* end of argument list, argument is invalid */
  {
    fprintf(stderr,"Argument `%s' is not recognized.\n", word);
    EXIT_ERROR("Error reading option file.\n");
  }
  
  /* if it is needed, read the next word here */
  if (readextra == 0)
  {
    status = read_next_word(fp, word);
  }

  return(status);

}  /* end of parse_argument() */


/************************************************************/
/* this function checks the settings of g_options for consistancy in
 * specified argument settings
 * serious errors are reported, and the program then terminates
 * minor errors are reported, and arbitrarily fixed
 */
void
verify_option_consistancy()
{

  /* a source for a graph (load from file or generate) must be specified */
  if ( (g_options.graphgentype == GEN_NOGRAPH) && 
	(check_word(g_options.loadgraph_fn) == WORD_EMPTY) )
  {
    EXIT_ERROR("Error: no source (load/generate) for a graph was specified.\n");
  }

  /* two different sources for a graph should not be specified */
  if ( (g_options.graphgentype != GEN_NOGRAPH) && 
	(check_word(g_options.loadgraph_fn) != WORD_EMPTY) )
  {
    WARN_ERROR("Warning: `load graph' and `generate graph' options both ");
    WARN_ERROR("specified.\n \t Ignoring `generate graph' option.\n");
    g_options.graphgentype = GEN_NOGRAPH;
  }

  /* if loading a graph, only need 1 set of tests on that graph, not more */
  if (check_word(g_options.loadgraph_fn) != WORD_EMPTY) 
  {
    if (g_options.num_graph_tests != 1)
    {
      WARN_ERROR("Warning: can not have multiple graph tests when ");
      WARN_ERROR("loading a graph.  Setting # of graph tests to 1.\n");

      g_options.num_graph_tests = 1;
    }
  }

  /* if saving a graph, no point if loaded that graph from a file 
   * this may change if extra info can be saved with the graph
   */
  if ( (check_word(g_options.savegraph_fn) != WORD_EMPTY) &&
       (check_word(g_options.loadgraph_fn) != WORD_EMPTY) )
  {
    WARN_ERROR("Warning: `save graph' option was selected together with");
    WARN_ERROR("`load graph' option.\n  Ignoring `save graph' option.\n");
  }

  /* if have multiple graphs being generated (num_graph_tests > 1) then 
   * warn user if printing statistics that only the first
   * graph is used for these tasks.
   */
  if ( (g_options.num_graph_tests > 1) &&  
       (g_options.report_flags & REPORT_GRAPH) )
  {
    WARN_ERROR("Warning: the `report graph statistics' option was selected ");
    WARN_ERROR("with multiple graphs.\n  Only the statistics of the first ");
    WARN_ERROR("graph generated will be presented.\n");
  }

  /* if using ensureham option, should only be using heuristic algorithms */
  if ( (g_options.graphgen.makeham == HAM_ENSURE) 
	&& (g_options.algorithm != ALG_POSA_HEUR) )
  {
    EXIT_ERROR("Can only using `+ensureham' option with heuristic algorithms.\n");
  }

}  /* end of verify_option_consistancy() */


/************************************************************/
/* this function loads and parses the option file
 * 
 */
void
parse_option_file(
  char filename[])
{
  FILE *fp;
  char word[STRLEN];
  int status;
  int wordtype;


  /* open file for reading */
  fp = fopen (filename, "r");
  if (fp == NULL)
  {
    EXIT_ERROR1("Error opening options file `%s'.\n",filename);
  }

  
  /* read first word */
  status = read_next_word(fp, word);

  /* enter main parse loop */
  while (1) 
  {
    /* loop termination condition:  exit upon reaching end of file */
    if (status & READ_EOF)
      break;

    if (status & READ_STRLONG)
      fprintf(stderr,"Warning:  truncated string while reading options.\n");

    wordtype = check_word(word);

    /* if comment, then skip to end of line */
    if (wordtype == WORD_COMMENT)
    {
      status = read_endofline(fp, word);
      
      if (status & READ_EOF)
        break;
      else  /* read next word, and process it */
      { 
        status = read_next_word(fp, word);
        continue;  
      }
    }

    /* if word = argument, parse the argument 
     * we expect this code to read new words till it comes to 
     * another argument (or comment), so it will return the new word
     * and status flag of the read 
     */
    if (wordtype == WORD_ARG)
    {
      status = parse_argument(fp, word);
      continue;  
    }
    else  /* word is PARM or OTHER, in the wrong place */
    {
      EXIT_ERROR1("Error reading `%s' in options file.\n", word);
    }
    
  }  /* end of while loop */
 
  /* verify that argument settings are consistant */
  verify_option_consistancy();

  /* close option file */
  fclose(fp);

}  /* end of parse_option_file() */


/************************************************************/
/* this function initializes the global option structure, and sets
 * some defaults
 * the options_fn (option filename) must be defined before calling this
 */
void
init_options()
{
  /* set defaults so that the software does nothing */
  
  g_options.algorithm = ALG_NOSOLVE;

  g_options.alg_timelimit = -1;		/* no timelimit */
  g_options.graphgentype = GEN_NOGRAPH;
  g_options.loadgraph_fn[0] = '\0';
  g_options.savegraph_fn[0] = '\0';

  /* default RNG seed is randomly generated from the current time */
  g_options.rng_seed = (int) time(NULL);	

  /* set numbers of tests to 1 */
  g_options.num_instance_tests = 1;
  g_options.num_graph_tests = 1;

  /* set broadest display options */
  g_options.report_flags = (REPORT_NONE);

  /* set outputfile name to base name = options file name - extension */
  strcpy(g_options.output_fn, g_options.options_fn);
  strip_extension_str(g_options.output_fn);

  g_options.stats_fp = stdout;
  g_options.log_fp = NULL;
  g_options.options_fp = NULL;
  g_options.summary_fp = NULL;
  g_options.sol_fp = NULL;

  /* initialize other sets of options */
  init_graphgen_options();
  init_heuristic_alg_options();
  init_backtrack_alg_options();

}  /* end of init_options() */


/************************************************************/
/* this function prints the current option settings
 * to the specified file pointer
 */
void
print_options(
  FILE *fp)
{

  fprintf(fp,"\nOption Settings:\n");

  fprintf(fp,"  algorithm to execute on graph: %s\n",
		opt_alg_str[g_options.algorithm]); 

  switch(g_options.algorithm)
  {
    case ALG_NOPRUNE_BT:
    case ALG_BACKTRACK:
      print_backtrack_alg_options(fp);
      break;

    case ALG_POSA_HEUR:
      print_heuristic_alg_options(fp);
      break;
  }  /* end of switch statement */

  if (check_word(g_options.loadgraph_fn) != WORD_EMPTY)
  {
    fprintf(fp,"  loading graph from file: %s\n", g_options.loadgraph_fn);
  }
  else
  {
    fprintf(fp,"  generating graph of type: %s\n",
	opt_gen_str[g_options.graphgentype]);
    print_graphgen_options(fp);
  }

  if (check_word(g_options.savegraph_fn) != WORD_EMPTY)
  {
    fprintf(fp,"  saving graph to file: %s\n", g_options.savegraph_fn);
  }

  if (g_options.alg_timelimit >= 0)
     fprintf(fp,"  Time Limit = %d seconds \n", g_options.alg_timelimit);
  else
     fprintf(fp,"  No Time Limit \n");

  fprintf(fp,"  RNG seed = %d\n", g_options.rng_seed);

  fprintf(fp,"  # of tests per graph instance = %d\n", 
	g_options.num_instance_tests);

  fprintf(fp,"  # of tests per graph type = %d\n", 
	g_options.num_graph_tests);

  fprintf(fp,"  reporting flags: ");
  if (g_options.report_flags == REPORT_NONE)
    fprintf(fp, "none\n");
  else
  {
    if (g_options.report_flags & REPORT_OPTIONS)
      fprintf(fp," options ");
    if (g_options.report_flags & REPORT_GRAPH)
      fprintf(fp," graph ");
    if (g_options.report_flags & REPORT_ALG)
      fprintf(fp," algorithm ");
    if (g_options.report_flags & REPORT_SOLUTION)
      fprintf(fp," solution ");
    if (g_options.report_flags & REPORT_SUMMARY)
      fprintf(fp," summary ");

    fprintf(fp, "\n");
  }

  fprintf(fp,"\n");  

}  /* end of print_options() */

