/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: sundy
 *
 * Created on January 7, 2018, 6:23 PM
 */

#include <cstdlib>
#include <iostream>
#include "AlphaHistogram.h"

using namespace std;

int main(int argc, char **argv)
{
    //    AlphaHistogram ah("CompleteShakespeare.txt");
    AlphaHistogram ah(argv[1]);
    ah.print();
}
