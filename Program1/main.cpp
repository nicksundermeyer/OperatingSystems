/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: ear
 *
 * Created on February 8, 2018, 6:15 PM
 */

#include <cstdlib>
#include "Scheduler.h"

/*
 * 
 */

int main(int argc, char** argv) {
    
    Scheduler s("program1_sample1.txt", 50, 10);
    s.simulate_SPN();
    
    return 0;
}

