/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AlphaHistogram.cpp
 * Author: sundy
 * 
 * Created on January 7, 2018, 8:15 PM
 */

#include "AlphaHistogram.h"

/**
   * Constructor - parse file and create map of chars.
   * 
   * @param fileName    name of input file
   */
AlphaHistogram::AlphaHistogram(const char *fileName)
{
    std::fstream file(fileName);

    char c;
    while (file.get(c))
    {
        if (int(c) >= 0x21 && int(c) <= 0x7E)
        {
            if (map.find(c) == map.end())
            {
                map[c] = 1;
            }
            else
            {
                map[c] = map[c] + 1;
            }
        }
    }
}

int AlphaHistogram::count(char input)
{
    return map[input];
}

void AlphaHistogram::print()
{
    for (auto it = map.begin(); it != map.end(); it++)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

AlphaHistogram::~AlphaHistogram() {}