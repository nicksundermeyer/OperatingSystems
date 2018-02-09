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
    
private:
    
    struct Process{
        std::string name;
        uint32_t arrival_time;
        uint32_t total_time;
        uint32_t block_interval;
    };
    
    std::string file_name;
    uint32_t block_duration;
    uint32_t time_slice;
    
    std::list<Process> ready;
    std::list<Process> blocked;
    
};

#endif /* SCHEDULER_H */
