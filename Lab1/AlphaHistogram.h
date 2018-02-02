/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   AlphaHistogram.h
 * Author: sundy
 *
 * Created on January 7, 2018, 8:15 PM
 */

#ifndef ALPHAHISTOGRAM_H
#define ALPHAHISTOGRAM_H

#include <iostream>
#include <fstream>
#include <map>

class AlphaHistogram
{
public:
  AlphaHistogram(const char *fileName);                           // constructor
  AlphaHistogram(const AlphaHistogram &orig) = delete;            // copy constructor
  AlphaHistogram(AlphaHistogram &&orig) = delete;                 // move constructor
  AlphaHistogram &operator=(const AlphaHistogram &orig) = delete; // copy assignment operator
  AlphaHistogram operator=(AlphaHistogram &&orig) = delete;       // move assignment operator
  virtual ~AlphaHistogram();                                      // destructor

  int count(char input);
  void createMap();
  void print();

private:
  std::map<char, int> map;
};

#endif /* ALPHAHISTOGRAM_H */
