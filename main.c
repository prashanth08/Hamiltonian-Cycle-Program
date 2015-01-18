
/************************************************************/
/* Hamiltonian Cycle Program
 *
 * Author(s): Basil Vandegriend 
 * Email: basil@cs.ualberta.ca
 * File: main.c
 * Contents: main body of program
 *           main control functions
 *
 */
/*
	Copyright (c) 1998 Basil Vandegriend 
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

/* project includes */
#include "main.h"

/* defines */
#define DEFAULT_OPTION_FILENAME "test.opt"


/************************************************************/
/* function to test if graph has certain desired properties
 * should probably put this function in hamcycle.c
 * tests if graph has min degree >= 2
 * tests if graph is biconnected
 */
void
test_graph_properties(
  graph_type *graph,
  stat_pergraph_type *graphstat)
{
  int loop;

  /* test if graph has min degree >= 2 */
  graphstat->mindeg2 = 1;
  for (loop = 0; loop < graph->numvert; loop++)
  {
    if (graph->deg[loop] < 2)
    {
      graphstat->mindeg2 = 0;
      break;
    }
  }

  /* test if graph is biconnected */
  /* if minimum degree is < 2, then not biconnected */
  if (graphstat->mindeg2 == 0)
  {
    graphstat->biconnected = 0;
    return;
  }

  if (check_graph_cutpoints(graph) == CUTPNT_EXIST)
    graphstat->biconnected = 0;
  else
    graphstat->biconnected = 1;
  
}  /* end of test_graph_properties() */


/************************************************************/
/* function to generate desired graph, using options
 * note that graph statistics are also calculated
 */
void
generate_graph(
  graph_type *graph)
{
  int done = 0;
  int flags;
  int alg;
  long algtime;
  stat_pertrial_type trialstat;
  int solution[MAXVERT];
  int ret;
  int num_edges;

  do   /* do-while loop for hamiltonicity */
  {
    switch(g_options.graphgentype)
    {
      case GEN_ICCS:
        gen_iccs_graph( graph, g_options.graphgen.numsubgraphs,
		g_options.graphgen.indsetsize);
	break;

      case GEN_ADDPATH:
        gen_addpath_graph( graph, g_options.graphgen.nvertex,
		2, g_options.graphgen.pathlengths);
	break;

      case GEN_ADDCYCLE:	
        gen_addcycle_graph( graph, g_options.graphgen.nvertex,
				g_options.graphgen.numcycles);
	break;

      case GEN_RANDOM:	
	/* if degconst is specified, then calculate the mean degree */
        if (g_options.graphgen.degconst > 0.0)
        {
          g_options.graphgen.meandeg = g_options.graphgen.degconst *
		(float) (log( (double) g_options.graphgen.nvertex) +
		 log(log( (double) g_options.graphgen.nvertex)));
        }
      
        /* calculate the number of edges */
        num_edges = (int) ( ( (float) g_options.graphgen.meandeg * 
		g_options.graphgen.nvertex) / 2.0 );
        gen_random_graph( graph, g_options.graphgen.nvertex, num_edges);
        break;

      case GEN_CROSSROADS:	
        gen_crossroads_graph( graph, g_options.graphgen.numsubgraphs);
        break;

      case GEN_DEGREEBOUND:	
        gen_degbound_graph( graph, g_options.graphgen.nvertex,
  			g_options.graphgen.degsize,
  			g_options.graphgen.degpercent);
        break;
  
      case GEN_KNIGHTTOUR:	
  
        gen_knighttour_graph( graph, g_options.graphgen.board1,
  			g_options.graphgen.board2,
  			g_options.graphgen.move1,
  			g_options.graphgen.move2);
  
        break;
  
      case GEN_GEOMETRIC:
  
        gen_geo_graph( 	g_options.graphgen.nvertex,
  			g_options.graphgen.dist * g_options.graphgen.dist,
  			g_options.graphgen.dflag, 
  			g_options.graphgen.dim, 
  			g_options.graphgen.wrapflag, graph);
        break;
  
      default:
        EXIT_ERROR("Error: invalid graph type in generate_graph().\n");
        break;
  
    }  /* end of switch statement */

    if (g_options.graphgen.makeham != HAM_ENSURE)
    {
      done = 1;
    }
    else /* need to confirm that cycle exists; use backtrack algorithm */
    {
      flags = g_options.report_flags;
      alg = g_options.algorithm;
      algtime = g_options.alg_timelimit;
      g_options.report_flags = REPORT_NONE;
      g_options.algorithm = ALG_BACKTRACK;
      g_options.alg_timelimit = -1;	

      trialstat.result = HC_NOT_FOUND;

      /* set algorithm options */
      g_options.bt_alg.initvertflag = INITVERT_RANDOM;
      g_options.bt_alg.degsortflag = DEGSORT_MIN;
      g_options.bt_alg.pruneoptflag = HC_PRUNE_BASIC | HC_PRUNE_CYC;

      ret = master_backtrack_alg(graph, &trialstat, solution);
      
      if (ret == HC_FOUND)
        done = 1;
      else
      {
        if (flags & REPORT_ALG)
        {
        fprintf(g_options.log_fp,"  Regenerating graph (not hamiltonian)...\n");
        }
      }

      /* restore flags */
      g_options.report_flags = flags;
      g_options.algorithm = alg;
      g_options.alg_timelimit = algtime;

    }  /* end of makeham code */

  } while(!done);  /* hamiltonicity while loop */

  /* graph is generated, so calculate graph statistics */
  calc_graph_stats(graph);

}  /* end of generate_graph() */


/************************************************************/
/* this function prints out the information for the summary file
 * if printflag == PRINT_INFO_HEADER then just the header is printed
 * otherwise everything is printed
 */
void
print_info_summary(
  stat_exp_type *stats,
  FILE *fp,
  int printflag)
{
  char sumstr[STRLEN];
 
  print_experiment_summary(sumstr);

  /* note that if multiple graphs tested, additional information
   * is printed out (after normal information)
   */
 
  /* print out header */
  fprintf(fp, "%30s", "Experiment");

  if (g_options.algorithm == ALG_POSA_HEUR)
  {
    /* print heuristic stats */
    fprintf(fp, "\t%% ham graph");
    fprintf(fp, "\talg success");
    fprintf(fp, "\texpected time");
  }
  else
  if ( (g_options.algorithm == ALG_NOPRUNE_BT) ||
        (g_options.algorithm == ALG_BACKTRACK) )
  {
    /* print backtrack stats */
    fprintf(fp, "\t%% ham graph");
    fprintf(fp, "\talg success");
    fprintf(fp, "\texpected time");
  }
  else
  {
    /* print combination */
    fprintf(fp, "\t%% ham graph");
    fprintf(fp, "\talg success");
    fprintf(fp, "\texpected time");
  }

  fprintf(fp, "\n");

  if (printflag == PRINT_INFO_HEADER)
    return;

  /* print out information */
  fprintf(fp, "%30s", sumstr);

  if (g_options.algorithm == ALG_POSA_HEUR)
  {
    /* print heuristic stats */
    fprintf(fp, "\t%.1f %%    ", stats->perham.ave*100);
    fprintf(fp, "\t%.1f %%    ", stats->algsuccess.ave*100);
    fprintf(fp, "\t%.1f (%.1f)", stats->tmexpect.ave,
	stats->tmexpect.stddev);
  }
  else
  if ( (g_options.algorithm == ALG_NOPRUNE_BT) ||
        (g_options.algorithm == ALG_BACKTRACK) )
  {
    /* print backtrack stats */
    fprintf(fp, "\t%.1f %%    ", stats->perham.ave*100);
    fprintf(fp, "\t%.1f %%    ", stats->algsuccess.ave*100);
    fprintf(fp, "\t%.1f (%.1f)", stats->tmexpect.ave,
	stats->tmexpect.stddev);
  }

  fprintf(fp, "\n");

}  /* end of print_info_summary() */


/************************************************************/
/* function to perform the experiments
 *   - load/generate graph
 *   - save graph if desired
 *   - print graph statistics if desired
 *   - run algorithm on graph, gather algorithm statistics
 *   - report algorithm statistics if desired
 *   - report solution if desired
 */
void
perform_experiments() 
{
  graph_type graph;
  stat_exp_type stats;

  int trialnum = 0;
  int graphnum = 0;

  FILE *fp;

  int gloop, iloop;
  int ret = 0;
 
  /************************************************************/
  /* obtain graph for testing */
  init_graph(&graph);
 
  if (g_options.graphgentype == GEN_NOGRAPH)
  {
    fp = fopen(g_options.loadgraph_fn, "r");
    if (fp == NULL)
    {
      EXIT_ERROR1("Error opening graph file `%s'.\n",g_options.loadgraph_fn);
    }
  
    if (input_graph(fp, &graph) != RET_OK)
    {
      EXIT_ERROR1("Error loading graph file `%s'.\n",g_options.loadgraph_fn);
    }
    fclose(fp);

    calc_graph_stats(&graph);
  }
  else
  {
    generate_graph(&graph);
  }
 
  /************************************************************/
  /* generate and print graph statistics if desired 
   * note:  only want to print statistics of first graph
   *        (if generating more than one)
   */
  if ( (g_options.report_flags & REPORT_GRAPH) && 
	(g_options.report_flags & REPORT_ALG) )
  {
    print_graph_stats(g_options.log_fp, &graph);
    fflush(g_options.log_fp);
  }

  /************************************************************/
  /* if don't want to solve, then just return */
  if (g_options.algorithm == ALG_NOSOLVE)
    return;
    
  /************************************************************/
  /* set up statistics */
  init_exp_stats(&stats);

  /************************************************************/
  /* start looping through experiments */
  while(1)
  {
    if (g_options.report_flags & REPORT_ALG)
    {
      fprintf(g_options.log_fp, "\nRunning trial #%d on graph #%d.\n", 
  	trialnum+1, graphnum+1);
      fflush(g_options.log_fp);
    }
  
    /* save graph if desired */
    if (check_word(g_options.savegraph_fn) != WORD_EMPTY)
    {
      char tempname[STRLEN];

      if (g_options.num_graph_tests > 1)
        sprintf(tempname,"%s%d",g_options.savegraph_fn, graphnum+1);
      else
        sprintf(tempname,"%s",g_options.savegraph_fn);

      fp = fopen(tempname, "w");
      if (fp == NULL)
      {
        WARN_ERROR1("Error opening graph file `%s'.  ",tempname);
        WARN_ERROR("Cannot save graph file.\n");
      }
  
      if (output_graph(fp, &graph,"") != RET_OK)
      {
        WARN_ERROR1("Error saving graph file `%s'.  ",tempname);
        WARN_ERROR("Cannot save graph file.\n");
      }
  
      fclose(fp);
    }

    /* run algorithm on graph */
    switch(g_options.algorithm)
    {
      case ALG_NOPRUNE_BT:
      case ALG_BACKTRACK:
      case ALG_POSA_HEUR:
        ret = test_hc_alg(&graph, &(stats.graph[graphnum].trial[trialnum]) );
        break;

    }  /* end of switch statement */

    /* save graph if timelimit reached */
    if (ret == RUN_TIMELIMIT)
    {
      char filename[STRLEN];
    
      sprintf(filename, "%s.graph%d", g_options.output_fn, graphnum+1);
      fp = fopen(filename, "w");
      if (fp == NULL)
      {
        WARN_ERROR1("Error opening graph file `%s'.  ",filename);
        WARN_ERROR("Cannot save hard graph file.\n");
      }
  
      if (output_graph(fp, &graph,"") != RET_OK)
      {
        WARN_ERROR1("Error saving graph file `%s'.  ",filename);
        WARN_ERROR("Cannot save hard graph file.\n");
      }
  
      fclose(fp);
    }

    /* update graph stats if necessary */
    if (stats.graph[graphnum].trial[trialnum].result == HC_FOUND)
    {
      if (stats.graph[graphnum].graphham != HC_NOT_EXIST) 
        stats.graph[graphnum].graphham = HC_FOUND;
      else
      {
        EXIT_ERROR("HC_FOUND and HC_NOT_EXIST occurred for same graph.\n");
      }
    }
    else if (stats.graph[graphnum].trial[trialnum].result == HC_NOT_EXIST)
    {
      if (stats.graph[graphnum].graphham != HC_FOUND)
        stats.graph[graphnum].graphham = HC_NOT_EXIST;
      else
      {
        EXIT_ERROR("HC_FOUND and HC_NOT_EXIST occurred for same graph.\n");
      }
    }

    trialnum++;

    /* check if finished with tests on current graph */
    if (trialnum == g_options.num_instance_tests)
    {
      /* generate mindegree2 and biconnected statistics 
       * this assumes graph data structure was never changed
       */
      test_graph_properties(&graph, &(stats.graph[graphnum]));

      graphnum++;
      trialnum = 0;
      if (graphnum == g_options.num_graph_tests)
        break;	/* exit from while loop */
      else		/* generate new graph */
      {
        if (g_options.report_flags & REPORT_ALG)
          fprintf(g_options.log_fp, "\n");
 
        init_graph(&graph);		/* this resets solve flag */
        generate_graph(&graph);

      }
    }

  }  /* end of while loop through tests */

  /* print out all the statistics - first print header */
  fprintf(g_options.stats_fp, 
	"\nGraph\tTrial\tSolve?\tTime\tNodes\tNodes/NumVert\n");

  /* loop through graphs and trials, printing information */
  for (gloop = 0; gloop < g_options.num_graph_tests; gloop++)
  {
    for (iloop = 0; iloop < g_options.num_instance_tests; iloop++)
    {
      fprintf(g_options.stats_fp, "\n%d\t%d\t", gloop+1, iloop+1);
      if (stats.graph[gloop].trial[iloop].result == HC_FOUND)
        fprintf(g_options.stats_fp, "yes\t");
      else if (stats.graph[gloop].trial[iloop].result == HC_NOT_EXIST)
        fprintf(g_options.stats_fp, "si\t");
      else
        fprintf(g_options.stats_fp, "no\t");
      fprintf(g_options.stats_fp, "%.1f\t%d\t%.1f\n",
		stats.graph[gloop].trial[iloop].time,
		stats.graph[gloop].trial[iloop].nodes,
	        (float) ((float) stats.graph[gloop].trial[iloop].nodes /
	                 (float) g_options.graphgen.nvertex) );
    }

    /* print GRAPH_INST statistics */
    fprintf(g_options.stats_fp, "\tIs graph hamiltonian: ");
    if (stats.graph[gloop].graphham == HC_FOUND)
      fprintf(g_options.stats_fp, "yes\n");
    else if (stats.graph[gloop].graphham == HC_NOT_EXIST)
      fprintf(g_options.stats_fp, "no\n");
    else
      fprintf(g_options.stats_fp, "unknown\n");

  }

  /* calculate overall statistics */
  calc_exp_stats(&stats);

  /* print overall stats */
  fprintf(g_options.stats_fp, "\n\nOverall Statistics\n\n");

  if (g_options.algorithm == ALG_POSA_HEUR)
  {
    /* print heuristic stats */
    fprintf(g_options.stats_fp, 
	"Percentage hamiltonian graphs = %.1f %% \n",
	stats.perham.ave * 100);
    fprintf(g_options.stats_fp, "Algorithm success rate = %.1f %% \n",
	stats.algsuccess.ave * 100);
    fprintf(g_options.stats_fp, "Time for success = %.1f (%.1f)\n",
	stats.tmsuccess.ave, stats.tmsuccess.stddev);
    fprintf(g_options.stats_fp, "Time for failure = %.1f (%.1f)\n",
	stats.tmfail.ave, stats.tmfail.stddev);
    fprintf(g_options.stats_fp, "Expected Time = %.1f (%.1f)\n",
	stats.tmexpect.ave, stats.tmexpect.stddev);
  }
  else
  if ( (g_options.algorithm == ALG_NOPRUNE_BT) || 
	(g_options.algorithm == ALG_BACKTRACK) )
  {
    /* print backtrack stats */
    fprintf(g_options.stats_fp, 
	"Percentage (min degree >= 2) = %.1f %%\n", 100*stats.permindeg2.ave);
    fprintf(g_options.stats_fp, 
	"Percentage biconnected = %.1f %%\n", 100*stats.perbiconnect.ave);
    fprintf(g_options.stats_fp, 
	"Percentage hamiltonian graphs = %.1f %% \n",
	stats.perham.ave * 100);
    fprintf(g_options.stats_fp, "Algorithm success rate = %.1f %% \n",
	stats.algsuccess.ave * 100);
    fprintf(g_options.stats_fp, "Time for success = %.1f (%.1f)\n",
	stats.tmsuccess.ave, stats.tmsuccess.stddev);
    fprintf(g_options.stats_fp, "Time for failure = %.1f (%.1f)\n",
	stats.tmfail.ave, stats.tmfail.stddev);
    fprintf(g_options.stats_fp, "Expected Time = %.1f (%.1f)\n",
	stats.tmexpect.ave, stats.tmexpect.stddev);
    fprintf(g_options.stats_fp, "Time on ham graphs = %.1f (%.1f)\n",
	stats.tmham.ave, stats.tmham.stddev);
    fprintf(g_options.stats_fp, "Time on non-ham graphs = %.1f (%.1f)\n",
	stats.tmnoham.ave, stats.tmnoham.stddev);
    fprintf(g_options.stats_fp, "Total Time = %.1f (%.1f)\n",
	stats.tmtotal.ave, stats.tmtotal.stddev);
  }

  /* print results lines which can be grepped */
  fprintf(g_options.stats_fp, "\nResults Lines:\n\n");

  fprintf(g_options.stats_fp, 
    "AA:%%Mindeg>=2,%%Biconnect,%%Ham:");
  fprintf(g_options.stats_fp, " %.1f%%", 100*stats.permindeg2.ave);
  fprintf(g_options.stats_fp, "  %.1f%%", 100*stats.perbiconnect.ave);
  fprintf(g_options.stats_fp, "  %.1f%%", 100*stats.perham.ave);
  fprintf(g_options.stats_fp, "\n");

  fprintf(g_options.stats_fp, 
    "BB:Time:Tot,Ham,NoHam:");
  fprintf(g_options.stats_fp, "%.1f (%.1f)",
	stats.tmtotal.ave, stats.tmtotal.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)",
	stats.tmham.ave, stats.tmham.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)",
	stats.tmnoham.ave, stats.tmnoham.stddev);
  fprintf(g_options.stats_fp, "\n");

  fprintf(g_options.stats_fp, 
    "CC:Node:Tot,Ham,NoHam,NoFind:");
  fprintf(g_options.stats_fp, " %.1f (%.1f)", 
        stats.nodes_tot.ave, stats.nodes_tot.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)", 
        stats.nodes_ham.ave, stats.nodes_ham.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)", 
        stats.nodes_noham.ave, stats.nodes_noham.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)", 
        stats.nodes_nofound.ave, stats.nodes_nofound.stddev);
  fprintf(g_options.stats_fp, "\n");

  fprintf(g_options.stats_fp, 
    "DD:Ratios:Tot,Ham,NoHam,NoFind:");
  fprintf(g_options.stats_fp, " %.1f (%.1f)", 
        stats.noderatio_tot.ave, stats.noderatio_tot.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)", 
        stats.noderatio_ham.ave, stats.noderatio_ham.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)", 
        stats.noderatio_noham.ave, stats.noderatio_noham.stddev);
  fprintf(g_options.stats_fp, "  %.1f (%.1f)", 
        stats.noderatio_nofound.ave, stats.noderatio_nofound.stddev);
  fprintf(g_options.stats_fp, "\n");

  fprintf(g_options.stats_fp, "\n");

  /* write to summary file if it is being used */
  if (g_options.report_flags & REPORT_SUMMARY)
    print_info_summary(&stats, g_options.summary_fp, PRINT_INFO_ALL);

}  /* end of perform_experiments() */


/************************************************************/
/* open the output files and print initial information 
 * to them
 */
void
setup_output_files()
{
  char filename[STRLEN];

  /* setup stat file for writing test results */
  strcpy(filename, g_options.output_fn);
  strcat(filename, ".stats");
  g_options.stats_fp = fopen(filename, "w");
  if (g_options.stats_fp == NULL)
  {
    EXIT_ERROR1("Error opening stats file `%s' for writing.\n", filename);
  }
  printf("Statistical information written to `%s'.\n", filename);
  fprintf(g_options.stats_fp, "Stats file for statistical information.\n\n");

  /* setup log file (if desired) */
  if (g_options.report_flags & REPORT_ALG)
  {
    strcpy(filename, g_options.output_fn);
    strcat(filename, ".log");
    g_options.log_fp = fopen(filename, "w");
    if (g_options.log_fp == NULL)
    {
      EXIT_ERROR1("Error opening log file `%s' for writing.\n", filename);
    }
    printf("Program execution information written to `%s'.\n", filename);
    fprintf(g_options.log_fp,
        "Log file for program execution information.\n\n");
  }

  /* setup solutions file (if desired) */
  if (g_options.report_flags & REPORT_SOLUTION)
  {
    strcpy(filename, g_options.output_fn);
    strcat(filename, ".sol");
    g_options.sol_fp = fopen(filename, "w");
    if (g_options.sol_fp == NULL)
    {
      EXIT_ERROR1("Error opening solution file `%s' for writing.\n",filename);
    }
    printf("Solution (if found) will be written to `%s'.\n", filename);

    /* print graph information if knightstour */
    if (g_options.graphgentype == GEN_KNIGHTTOUR)
    {
      fprintf(g_options.sol_fp,"amove %d\n", g_options.graphgen.move1);
      fprintf(g_options.sol_fp,"bmove %d\n", g_options.graphgen.move2);
      fprintf(g_options.sol_fp,"nboard %d\n", g_options.graphgen.board1);
      fprintf(g_options.sol_fp,"mboard %d\n", g_options.graphgen.board2);
    }
  }

  /* setup options file */
  if (g_options.report_flags & REPORT_OPTIONS)
  {
    strcpy(filename, g_options.output_fn);
    strcat(filename, ".options");
    g_options.options_fp = fopen(filename, "w");
    if (g_options.options_fp == NULL)
    {
      EXIT_ERROR1("Error opening options file `%s' for writing.\n", filename);
    }
  }

  /* setup summary file */
  if (g_options.report_flags & REPORT_SUMMARY)
  {
    strcpy(filename, g_options.output_fn);
    strcat(filename, ".summary");
    g_options.summary_fp = fopen(filename, "w");
    if (g_options.summary_fp == NULL)
    {
      EXIT_ERROR1("Error opening summary file `%s' for writing.\n", filename);
    }
    printf("Summary information written to `%s'.\n", filename);
  }

  /************************************************************/
  /* if specified, print the options settings */
  if (g_options.report_flags & REPORT_OPTIONS)
  {
    print_options(g_options.options_fp);
    fflush(g_options.options_fp);
  }

  /* print summary of options to statistic file, if using it */
  if (check_word(g_options.output_fn) != WORD_EMPTY)
  {
    fprintf(g_options.stats_fp,"algorithm to execute on graph: %s\n",
          opt_alg_str[g_options.algorithm]);

    if (check_word(g_options.loadgraph_fn) != WORD_EMPTY)
    {
      fprintf(g_options.stats_fp, "loading graph from file: %s\n",
              g_options.loadgraph_fn);
    }
    else
    {
      fprintf(g_options.stats_fp,"generating graph of type: %s\n",
          opt_gen_str[g_options.graphgentype]);
      print_graphgen_options(g_options.stats_fp);
    }
    fflush(g_options.stats_fp);
  }

  /* save rng_seed information to stats file */
  fprintf(g_options.stats_fp, "\nRNG Seed = %d\n", g_options.rng_seed);

}  /* end of setup_output_files() */


/************************************************************/
/* print usage information 
 */
void
print_usage_info()
{
  printf("Usage Information:\n");
  printf("  <no arguments> : load options from default file `%s'\n",
         DEFAULT_OPTION_FILENAME);
  printf("  -t <filename>  : load testfile from file <filename>\n");
  printf("  -o <filename>  : load options from file <filename>\n");
  printf("  -h             : print this information \n");
  printf("\n");
  printf("See the manual for details on the format of option files ");
  printf("and test files.\n");

  printf("\n");

}  /* end of print_usage_info() */


/************************************************************/
/* main body of program 
 *   - do initial command-line parsing
 *   - load options
 *   - start experiments
 */
/************************************************************/
int
main( 
  int argc, 
  char *argv[] )
{
  int loadtestfile = 0;

  int ch;

  /* print program header */
  printf("\n_____Hamiltonian Cycle Program_____\n\n");

  /************************************************************/
  /* parse simple command-line options 
   * possibilities:
   * > main <no arguments>  - load from default options file
   * > main -o <filename>   - load from options file 'filename'
   * > main -t <filename>   - load testfile from file 'filename', run tests
   * > main -h 		   - print usage information (help)
   */
  opterr = 0;
  while ( (ch = getopt(argc, argv, "o:t:h")) != -1)
  {
    switch(ch)
    {
      case 'h':
        print_usage_info();
        exit(1);
        break;
        
      case 'o':
        strcpy(g_options.options_fn, optarg);
        break;

      case 't':
        strcpy(g_options.test_fn, optarg);
        loadtestfile = 1;
        break;

      default:
        EXIT_ERROR("Invalid argument on command line.\n");
        break;

    }  /* end of switch */
  }  /* end of while */

  if (argc == 1)  /* no arguments specified */
  {
    /* set default option filename */
    strcpy(g_options.options_fn, DEFAULT_OPTION_FILENAME);
  }

  /* check if testfile was specified, if so, go to automated testing */
  if (loadtestfile)
  {
    do_automated_testing(argv[0]);
    printf("\n");
    exit( 0 );
  }

  /* initialize options */
  init_options();

  /* load/parse options */
  printf("Parsing options file `%s'...\n", g_options.options_fn);
  parse_option_file(g_options.options_fn);
  
  /* seed random number generator */
  srandom( g_options.rng_seed );
  srand48( (long) g_options.rng_seed );

  /* setup correct handling of output and print initial information */
  setup_output_files();

  /* start the experiments */
  if (g_options.report_flags & REPORT_ALG)
  {
    fprintf(g_options.log_fp, "\nStarting the experiment(s)...\n");
    fflush(g_options.log_fp);
  }
  perform_experiments();

  printf("\n");
  
  exit( 0 );

}  /* end of main() */

