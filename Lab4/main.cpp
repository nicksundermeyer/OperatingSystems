zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Evan Reierson and Nick Sundermeyer
 *
 * Created on January 17, 2018, 5:27 PM
 */

#include <cstdlib>
#include <iostream>
#include "ProcessTrace.h"


using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    // make sure a text file is supplied as a command line argument
//    if (argc != 2){
//        cout << "oops, please supply the name of a text file" << endl;
//        return 1;
//    }
    
    //ProcessTrace p(argv[1]);
    ProcessTrace p;
    p = ProcessTrace("trace1.txt");
    p.execute();
    
    
    
    return 0;
}



