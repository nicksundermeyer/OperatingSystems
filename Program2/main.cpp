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
#include "ProcessTrace.h"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    mem::MMU MMU_name(256);
    
    ProcessTrace p("trace1v.txt");
    p.Execute();
    return 0;
}

