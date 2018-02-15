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
    /*
     *  Constructor - set up list of processes and other parameters
     * 
     * @file_name - name of the file to read processes from
     * @block_duration - duration of blocking for all processes
     * @time_slice - time quantum for round-robin scheduling
     */
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
    
    /*
     * simulate_RR - simulate round-robin scheduling on processes in the 'processes' vector
     */
    void simulate_RR();
    
    /*
     * simulate_SPN - simulate shortest-process-next scheduling on processes in the 'processes' vector
     */
    void simulate_SPN();
    
private:
    
    struct Process{
        std::string name; // name of process
        uint32_t arrival_time; // at what time the process arrives
        uint32_t total_time; // total time needed to complete process
        uint32_t processed_time; // total time processed
        uint32_t block_interval; // time interval before process blocks
        uint32_t blocked_time; // time in blocked list
        uint32_t turnaround_time;
    };
    
    std::string file_name; // name of file to read
    uint32_t block_duration; // duration of blocking for all processes
    uint32_t time_slice; // time quantum for round-robin
    
    std::vector<Process> processes; // processes to simulate
    std::vector<Process> ready; // currently ready processes
    std::vector<Process> blocked; // currently blocked processes
    
    // trivial function to print out a vector
    void printVector(std::vector<Process> v);
};

#endif /* SCHEDULER_H */

