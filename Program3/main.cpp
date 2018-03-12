/*
 * COMP3361 Winter 2018
 * Programming Assignment 2 Sample Solution
 */

/* 
 * File:   main.cpp
 * Author: Mike Goss <mikegoss@cs.du.edu>
 *
 * Created on October 28, 2017, 10:08 PM
 */


#include "PageFrameAllocator.h"
#include "ProcessTrace.h"

#include <MMU.h>

#include <cstdlib>
#include <iostream>

/*
 * 
 */
int main(int argc, char* argv[]) {
  // Use command line argument as file name
//  if (argc != 2) {
//    std::cerr << "usage: Lab2 trace_file\n";
//    exit(1);
//  }
    
    std::vector<ProcessTrace*> scheduler;
    std::vector<std::string> trace_names;
    
    trace_names.push_back("trace4v_multi-l2-tables.txt");
    //trace_names.push_back("trace1v.txt");
//    trace_names.push_back("trace1v.txt");
    

    
    uint32_t time_slice = 1;
  
 
  
    // add process traces to vector
    for (int i=0; i<trace_names.size(); i++){
        mem::MMU* memory = new mem::MMU(1024);
        PageFrameAllocator* allocator = new PageFrameAllocator(*memory);       
        scheduler.push_back(new ProcessTrace(*memory, *allocator, trace_names[i]));
    }
    
    // initialize each process trace
    for (int i=0; i<scheduler.size(); i++){  
        scheduler[i]->Initialize();
    }  
    
    uint32_t num_terminated = 0;
    uint32_t t = 1;
//    while (num_terminated < scheduler.size()){
//        
//    }
    

    
    for(int i=0; i<100; i++){
        for (int j=0; j<time_slice; j++){
            std::cout << t+j << ":" << i % scheduler.size() + 1<< ":";
            scheduler[i%scheduler.size()]->Execute();
        }
        t+=time_slice;
    }

    
    
    
  
  //ProcessTrace trace(memory, allocator, "trace2v_multi-page.txt");
  
    
    // clean up memory
    for (int i=0; i<scheduler.size(); i++){
        scheduler[i]->~ProcessTrace();
    }
    
   
   

  return 0;
}












/*
 * Hey Volkan,
 * 
 * We added the quota command and then used CmdAlloc in Cmd put, copy, and fill.
 * trace1v.txt appears to be working for one process, but the rest aren't really
 * working yet.
 * 
 * We left off trying to fix the "duplicate allocated at..." error when running
 * it with trace2v_multi-page.txt and we think the problem has to do with setting
 * the pmcb state so that it continues the trace file in the correct place after
 * a PageFaultException. We are also getting some compare errors later on which
 * might be related to this problem.
 * 
 * Some ideas for stuff to work on:
 * - fix that error
 * - get the multiprogramming working in main
 * 
 * Thanks!
 * Evan and Nick
 */
