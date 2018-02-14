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
    int time = 0; // scheduler time
    bool running = true; // scheduler running
    bool idle = false;
    int ci = 0; // index in ready vector of current process
    int current_interval = 0;
    std::vector<Process> process = processes;
    float turnaround_time = 0;
    
    std::cout << "SPN	" << block_duration << std::endl;
    
    while(running)
    {
	std::vector<int> added;
	//add to ready list at arrival time
	for(int i=0; i<process.size(); i++)
	{
	    if(process[i].arrival_time <= time)
	    {
		ready.push_back(process[i]);
		added.push_back(i);
	    }
	}
	
	// erase from process afterwards to avoid adding multiple copies
	for(int i=0; i<added.size(); i++)
	{
	    process.erase(process.begin() + i);
	}
	
	// if nothing ready, then do an idle cycle
	if(ready.empty())
	    idle = true;
	
	if(!idle)
	{
	    // re-add to ready list if done blocking
	    for(int i=0; i<blocked.size(); i++)
	    {
		// if ready to be unblocked, re-add to ready list
		if(blocked[i].blocked_time >= block_duration)
		{
		    ready.push_back(blocked[i]);
		    blocked.erase(blocked.begin() + i);
		}
	    }
	    
	    // check through currently ready processes for shortest
	    float shortest_process = std::min(ready[0].block_interval, ready[0].total_time-ready[0].processed_time);
	    ci = 0;

	    for(int i=0; i<ready.size(); i++)
	    {
		if(std::min(ready[i].block_interval, ready[i].total_time-ready[i].processed_time) < shortest_process)
		{
		    shortest_process = std::min(ready[i].block_interval, ready[i].total_time-ready[i].processed_time);
		    ci = i;
		}
	    }
	    
	    // update time
	    time += shortest_process;
	    ready[ci].processed_time += shortest_process;
	    
	    // update time for blocked items
	    for(int i=0; i<blocked.size(); i++)
	    {
		// add block interval to all items in block
		blocked[i].blocked_time += shortest_process;
	    }

	    // put process on blocked list or remove if complete
	    if(ready[ci].processed_time < ready[ci].total_time)
	    {
		std::cout << " " << time-shortest_process << "	" << ready[ci].name << "	" << shortest_process << "  B" << std::endl;
		ready[ci].blocked_time = 0;
		blocked.push_back(ready[ci]);
	    }
	    else
	    {
		std::cout << " " << time-shortest_process << "	" << ready[ci].name << "	" << shortest_process << "  T" << std::endl;
		turnaround_time += (time-ready[ci].arrival_time);
	    }

	    // remove from ready list
	    ready.erase(ready.begin() + ci);
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
	    blocked.erase(blocked.begin() + shortest_block_index);
	    
	    // print out to console
	    std::cout << " " << time-shortest_block << "	<idle>	" << shortest_block << std::endl;
	    
	    idle = false;
	}
	
	// re-add to ready list if done blocking
	for(int i=0; i<blocked.size(); i++)
	{
	    // if ready to be unblocked, re-add to ready list
	    if(blocked[i].blocked_time >= block_duration)
	    {
		ready.push_back(blocked[i]);
		blocked.erase(blocked.begin() + i);
	    }
	}
	
	// if ready and blocked are empty, we are done
	if(ready.empty() && blocked.empty())
	{
	    std::cout << " " << time << "   <done>  " << turnaround_time/processes.size() << std::endl;
	    running = false;
	}
    }
}

void Scheduler::printVector(std::vector<Process> v)
{
    for(int i=0; i<v.size(); i++)
    {
	std::cout << v[i].name << " " << v[i].block_interval << " " << v[i].blocked_time << " ";
    }
    std::cout << std::endl;
}