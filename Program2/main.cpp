/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: sundy
 *
 * Created on February 16, 2018, 3:40 PM
 */

#include <cstdlib>
#include <MMU.h>
#include <PMCB.h>
#include "ProcessTrace.h"
#include "PageFrameAllocator.h"
#include <iostream>

/*
 * 
 */
int main(int argc, char** argv) {
    
    // create memory, create and set PMCB
    mem::MMU memory(256);
    mem::PMCB pmcb;
    
    // allocate page frames to memory in physical mode
    pmcb.vm_enable = false;
    memory.set_PMCB(pmcb);
    PageFrameAllocator pfa(256, memory);
    
    ProcessTrace p(argv[1], memory, pmcb, pfa);
    p.Execute();
    return 0;
}

