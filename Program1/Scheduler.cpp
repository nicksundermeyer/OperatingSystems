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
        p.processed_time = 0;
        p.blocked_time = 0;
        
        processes.push_back(p);
    }
    
    file.close();
}

Scheduler::~Scheduler() {
}

void Scheduler::simulate_RR(){
    std::cout << "RR " << block_duration << " " << time_slice << std::endl;
    Process current_process = NULL;
    
    for(uint32_t time=0; time<300; time++){
        
        // add process to ready upon arrival
        for(int i=0; i<processes.size(); i++){
            if (processes[i].arrival_time = time){
                ready.insert(0, processes[i]);
            }
        }  
        
        if(current_process == NULL){
            current_process = ready.back();
            ready.pop_back();
        }
        
        current_process.processed_time ++;
        
        
        
        
        
        
    }
}

void Scheduler::simulate_SPN()
{
//    uint32_t time=0;
//    bool running = false; // whether a process is currently using the CPU
//    int current_interval = 0; // time interval running current process/idle
//    Scheduler::Process current_process; // current process being run
//    
//    std::cout << "SPN " << block_duration << std::endl;
//    
//    // add first process
//    ready.push_back(processes[0]);
//    
//    // time loop
//    while(!ready.empty() || !blocked.empty())
//    {        
//        // if there is not already a process running, find a new process to run
//        if(!running)
//        {
//            // check through currently ready processes for shortest
//            float shortest_process = ready[0].total_time - ready[0].processed_time;
//            int shortest_process_index = 0;
//            for(int i=0; i<ready.size(); i++)
//            {
//                std::cout << ready[i].name << " ";
//                if(ready[i].total_time < shortest_process)
//                {
//                    shortest_process = ready[i].total_time;
//                    shortest_process_index = i;
//                }
//            }
//            std::cout << std::endl;
//            
//            running = true;
//            
//            // set currently running process, and remove it from the ready list
//            current_process = ready[shortest_process_index];
//            ready.erase(ready.begin() + shortest_process_index);
//            current_interval = 0;
//        }
//        
//        // if past block interval duration, put the process in the blocked list and look for new process
//        if(current_interval >= current_process.block_interval)
//        {
//            running = false;
//            blocked.push_back(current_process);
//            std::cout << " " << current_process.name << "   " << current_interval << "    B" << std::endl;
//        }
//        
//        // if process is done, remove it and look for new process
//        if(current_process.processed_time >= current_process.total_time)
//        {
//            std::cout << " " << current_process.name << "   " << current_interval << "    T" << std::endl;
//            running = false;
//        }
//        
//        // update current process time if there is one running
//        if(running)
//        {
//            current_process.processed_time++;
//            current_interval++;
//        }
//        
//        // iterate through processes to add processes when they arrive
//        std::vector<Process>::iterator it;
//        for (it=processes.begin(); it!=processes.end(); ++it){
//            if(it->arrival_time == time)
//            {
////                std::cout << time << "  " << it->name << std::endl;
//                ready.push_back(*it);
//            }
//        }
//        
//        time++;
//    }
}