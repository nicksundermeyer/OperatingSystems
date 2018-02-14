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
//    std::cout << "RR " << block_duration << " " << time_slice << std::endl;
//    Process current_process = NULL;
//    
//    for(uint32_t time=0; time<300; time++){
//        
//        // add process to ready upon arrival
//        for(int i=0; i<processes.size(); i++){
//            if (processes[i].arrival_time = time){
//                ready.insert(0, processes[i]);
//            }
//        }  
//        
//        if(current_process == NULL){
//            current_process = ready.back();
//            ready.pop_back();
//        }
//        
//        current_process.processed_time ++;
//        
//        
//        
//        
//        
//        
//    }
}

void Scheduler::simulate_SPN()
{
    uint32_t time=0;
    bool processing = false; // whether a process is currently using the CPU
    bool idle = false;
    bool running = true;
    float turnaround_time = 0;
    std::vector<Process> process = processes; // local copy of processes to allow removing processes without messing up other scheduler
    int current_interval = 0; // time interval running current process/idle
    Scheduler::Process current_process; // current process being run
    std::vector<Process>::iterator it;
    
    std::cout << "SPN " << block_duration << std::endl;
    
    // add first process
    ready.push_back(process[0]);
    process.erase(process.begin());
    
    // time loop
    while(running)
    {
//	if(time > 140)
//	    printVector(ready);
	
	if(!idle)
	{
	    // if there is not already a process running, find a new process to run
	    if(!processing)
	    {
		if(!ready.empty())
		{
		    // check through currently ready processes for shortest
		    float shortest_process = std::min(ready[0].block_interval, ready[0].total_time-ready[0].processed_time);
		    int shortest_process_index = 0;
		    for(int i=0; i<ready.size(); i++)
		    {
//			if(time > 180)
//			{
//			    std::cout << ready[i].name << " " << ready[i].block_interval << std::endl;
//			}
				
			if(std::min(ready[i].block_interval, ready[i].total_time-ready[i].processed_time) < shortest_process)
			{
			    shortest_process = std::min(ready[0].block_interval, ready[i].total_time-ready[i].processed_time);
			    shortest_process_index = i;
			}
		    }
		    
		    processing = true;

		    // set currently running process, and remove it from the ready list
		    current_process = ready[shortest_process_index];
		    ready.erase(ready.begin() + shortest_process_index);
		    current_interval = 0;
		}
		else
		{
		    idle = true;
		    current_interval = 0;
		}
	    }
	    else
	    {
		// if process is done, remove it and look for new process
		if(current_process.processed_time >= current_process.total_time)
		{
		    std::cout << " " << time-current_interval << "  " << current_process.name << "   " << current_interval << "    T" << std::endl;
		    processing = false;
		    turnaround_time += (time-current_process.arrival_time);
		}

		// if past block interval duration, put the process in the blocked list and look for new process
		else if(current_interval >= current_process.block_interval)
		{
		    processing = false;
		    current_process.blocked_time = 0;
		    blocked.push_back(current_process);
		    std::cout << " " << time-current_interval << "  " << current_process.name << "   " << current_interval << "    B" << std::endl;
		}

		// update current process time if there is one running
		current_process.processed_time++;
		current_interval++;

		if(processing)
		{
		    time++;
		    // iterate through blocked processes to check if they should be readied
		    for(int i=0; i<blocked.size(); i++)
		    {
			if(blocked[i].blocked_time >= block_duration)
			{
			    ready.push_back(blocked[i]);
			    blocked.erase(blocked.begin() + i);
			}
			
			blocked[i].blocked_time++;
		    }
		}
	    }
	}
        else
	{
	    // idle
	    // iterate through blocked processes and find the shortest remaining block
	    int shortest_block = block_duration - blocked[0].blocked_time;
	    int shortest_block_index = 0;
	    for(int i=0; i<blocked.size(); i++)
	    {
		if((block_duration - blocked[i].blocked_time) < shortest_block)
		{
		    shortest_block = block_duration - blocked[i].blocked_time;
		    shortest_block_index = i;
		}
	    }
	    
	    // add idle time to all other blocks
	    for(int i=0; i<blocked.size(); i++)
	    {
		blocked[i].blocked_time += shortest_block;
	    }
	    
	    // add idle time to time
	    time += shortest_block;
	    
	    // return process to ready
	    ready.push_back(blocked[shortest_block_index]);
	    
	    // print out to console
	    std::cout << " " << time-shortest_block << "   <idle>  " << shortest_block << std::endl;
	    
	    // return from idle state
	    idle = false;
	}
        
        // iterate through processes to add processes when they arrive
        for (int i=0; i<process.size(); i++){
	    if(process[i].arrival_time >= time)
            {
                ready.push_back(process[i]);
                process.erase(process.begin()+i);
            }
        }
	
        // if ready, blocked, processes are empty and the current process is done, terminate
        if(ready.empty() && blocked.empty() && process.empty() && current_process.processed_time > current_process.total_time)
        {
            std::cout << "  " << time << "    <done>  " << turnaround_time/processes.size() << std::endl;
            running = false;
        }
    }
}

void Scheduler::printVector(std::vector<Process> v)
{
    for(int i=0; i<v.size(); i++)
    {
	std::cout << v[i].name << " " << v[i].block_interval << " ";
    }
    std::cout << std::endl;
}