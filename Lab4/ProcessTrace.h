/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProcessTrace.h
 * Author: Evan Reierson and Nick Sundermeyer
 *
 * Created on January 20, 2018, 3:35 PM
 */

#ifndef PROCESSTRACE_H
#define PROCESSTRACE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <MMU.h>

class ProcessTrace {
public:
    // constructor
    ProcessTrace(std::string trace_file);
    
    // destructor
    virtual ~ProcessTrace();
    
    // copy constructor
    ProcessTrace(const ProcessTrace& orig) = delete;
    
    // move constructor
    ProcessTrace(ProcessTrace &&orig) = delete;
    
    // copy assignment operator
    ProcessTrace &operator=(const ProcessTrace &orig) = delete;
    
    // move assignment operator
    ProcessTrace operator=(const ProcessTrace &&orig) = delete;
    
    /**
    * execute - simulate trace file execution using a vector
    * 
    * @return - boolean success of executing trace file
    */
    bool execute();
    
private:
    /**
    * alloc - allocate memory from address 0 to size-1
    * 
    * @param size       number of bytes to allocate
    * @return           boolean success
    */
    bool alloc(int size);
    
    /**
    * compareBytes - compare bytes in memory to expected bytes
    * 
    * @param addr               starting address
    * @param expected_values    list of bytes expected to be in memory
    * @return                   boolean success    
    */
    bool compareBytes(uint32_t addr, std::vector<uint8_t> expected_values);
    
    /**
    * putBytes - store values in memory starting at the specified address
    * 
    * @param addr       starting address
    * @param values     values to put in memory
    * @return           boolean success
    */    
    bool putBytes(uint32_t addr, std::vector<uint8_t> values);
    
    /**
    * fillBytes - fill memory with count instances of value starting at address
    * 
    * @param addr       starting address
    * @param count      number of instances
    * @param value      value to store 
    * @return           boolean success
    */    
    bool fillBytes(uint32_t addr, uint32_t count, uint32_t value);
    
    /**
    * copyBytes - copy count bytes from src_addr to dest_addr
    * 
    * @param dest_addr      destination address
    * @param src_addr       source address
    * @param count          number of bytes to copy
    * @return               boolean success
    */    
    bool copyBytes(uint32_t dest_addr, uint32_t src_addr, uint32_t count);
    
    /**
    * dumpBytes - write count bytes to standard output starting at address
    * 
    * @param addr       starting address
    * @param count      number of bytes to output
    * @return           boolean success;
    */    
    bool dumpBytes(uint32_t addr, uint32_t count);
    
    /**
    * parseCommand - parse line of trace file and execute appropriate method
    * 
    * @param iss        istringstream to parse
    * @param count      number of bytes to output
    */  
    void parseCommand(std::istringstream& iss);
    
    
    
    // member variables
    std::ifstream file;
    // vector<uint8_t> memory;
    mem::MMU*  memory;
    
};

#endif /* PROCESSTRACE_H */

