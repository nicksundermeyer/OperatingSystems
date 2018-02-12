/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Scheduler.cpp
 * Author: ear
 * 
 * Created on February 8, 2018, 6:26 PM
 */

#include "Scheduler.h"

Scheduler::Scheduler(std::string file_name, uint32_t block_duration, uint32_t time_slice) {
    Scheduler::file_name = file_name;
    Scheduler::block_duration = block_duration;
    Scheduler::time_slice = time_slice;
    
    std::ifstream file;
    file.open(file_name);
    
    // make sure file read is valid
    if (!file){
        std::cout << "cannot open file" << std::endl;
        exit(1);
    }
    
    std::string name;
    uint32_t arrival_time;
    uint32_t total_time;
    uint32_t block_interval;
    
    std::string current_line;
    while (getline(file, current_line)){
        std::istringstream iss(current_line);
        
        iss >> name;
        iss >> arrival_time;
        iss >> total_time;
        iss >> block_interval;
        
        Process p;
        p.name = name;
        p.arrival_time = arrival_time;
        p.total_time = total_time;
        p.block_interval = block_interval;
        
        processes.push_back(p);
    }
    
    std::list<Process>::iterator it;
    for (it=processes.begin(); it!=processes.end(); ++it){
        std::cout << ' ' << (*it).name;
    }
    
    file.close();
}

Scheduler::~Scheduler() {
}

void Scheduler::simulate_RR(){
    
    for(uint32_t time=0; time<300; time++){
        
        // add to ready upon arrival
        std::list<Process>::iterator it;
        for (it=processes.begin(); it!=processes.end(); ++it){
            //if ()
        }
        
       
        
    }
}

