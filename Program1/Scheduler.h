/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Scheduler.h
 * Author: ear
 *
 * Created on February 8, 2018, 6:26 PM
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <list>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

class Scheduler {
public:
    // constructor
    Scheduler(std::string file_name, uint32_t block_duration, uint32_t time_slice);
    
    // destructor
    virtual ~Scheduler();
    
    // copy constructor
    Scheduler(const Scheduler& orig) = delete;
    
    // move constructor
    Scheduler(Scheduler &&orig) = delete;
    
    // copy assignment operator
    Scheduler &operator=(const Scheduler &orig) = delete;
    
    // move assignment operator
    Scheduler operator=(const Scheduler &&orig) = delete;
    
    void simulate_RR();
    void simulate_SPN();
    
private:
    
    struct Process{
        std::string name;
        uint32_t arrival_time;
        uint32_t total_time;
        uint32_t processed_time;
        uint32_t block_interval;
        uint32_t blocked_time;
    };
    
    std::string file_name;
    uint32_t block_duration;
    uint32_t time_slice;
    
    std::vector<Process> processes;
    std::vector<Process> ready;
    std::vector<Process> blocked;
};

#endif /* SCHEDULER_H */

