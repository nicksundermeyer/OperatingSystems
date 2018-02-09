/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProcessTrace.cpp
 * Author: Evan Reierson and Nick Sundermeyer
 * 
 * Created on January 20, 2018, 3:35 PM
 */

#include "ProcessTrace.h"

ProcessTrace::ProcessTrace(std::string file_name) {
    file.open(file_name);
    
    // make sure file read is valid
    if (!file){
        std::cerr << "cannot open file" << std::endl;
        exit(1);
    }
}

ProcessTrace::~ProcessTrace() {
    file.close(); 
}

bool ProcessTrace::execute(){
    std::string current_line;
    int line_number = 1;
    while (getline(file, current_line)){
        std::istringstream iss(current_line);
        std::cout << std::dec << line_number << ':';
        parseCommand(iss);
        line_number ++;
    }
}

void ProcessTrace::parseCommand(std::istringstream& iss){
    std::string command;
    iss >> command;

    if (command == "alloc"){
        int size;

        iss >> std::hex >> size;

        ProcessTrace::alloc(size);

    } else if (command == "compare"){
        uint32_t addr;
        std::vector<uint8_t> expected_values;

        iss >> std::hex >> addr;
        
        int v;
        while(iss >> std::hex >> v){
            expected_values.push_back(v);
        }

        ProcessTrace::compareBytes(addr, expected_values);

    } else if (command == "put"){
        uint32_t addr;
        std::vector<uint8_t> values;

        iss >> std::hex >> addr;
        
        int v;
        while(iss >> std::hex >> v){
            values.push_back(v);
        }
        
        ProcessTrace::putBytes(addr, values);

    } else if (command == "fill"){
        uint32_t addr;
        int count;
        int value;

        iss >> std::hex >> addr;
        iss >> std::hex >> count;
        iss >> std::hex >> value;

        ProcessTrace::fillBytes(addr, count, value);

    } else if (command == "copy"){
        int dest_addr;
        int src_addr;
        int count;

        iss >> std::hex >> dest_addr;
        iss >> std::hex >> src_addr;
        iss >> std::hex >> count;

        ProcessTrace::copyBytes(dest_addr, src_addr, count);

    } else if (command == "dump"){
        uint32_t addr;
        int count;

        iss >> std::hex >> addr;
        iss >> std::hex >> count;
        
        ProcessTrace::dumpBytes(addr, count);

    } else {
        std::cerr << "oops, unknown command" << std::endl;
    }
}

bool ProcessTrace::alloc(int size){
    std::cout << "alloc " << std::hex << size << std::endl;
    if (size < 0){
        std::cerr << "invalid size" << std::endl;
        return false;
    }
    
    uint32_t page_size;
    if (size % 0x1000 == 0){
        page_size = size / 0x1000;
    } else {
        page_size = size / 0x1000 + 1;
    }
    memory =  new mem::MMU(page_size);
    
    return true;
}

bool ProcessTrace::compareBytes(uint32_t addr, std::vector<uint8_t> expected_values){
    std::cout << "compare " << std::hex << addr;
    for (int i=0; i < expected_values.size(); i++){
        std::cout << " " << std::hex << expected_values[i] + 0;
    }
    std::cout << std::endl;
    
    for (int i=0; i < expected_values.size(); i++){
        uint8_t b;
        memory->get_byte(&b, addr + i);
        if (expected_values[i] != b){
            std::cout << "compare error at address " << std::hex << addr + i;
            std::cout << ", expected " << std::hex << expected_values[i] + 0;
            std::cout << ", actual is " << std::hex << b + 0;
            std::cout << std::endl;
        }
    }
    
    
}
bool ProcessTrace::putBytes(uint32_t addr, std::vector<uint8_t> values){
    std::cout << "put " << std::hex << addr;
    for (int i=0; i < values.size(); i++){
        std::cout << " " << std::hex << values[i] + 0;
    }
    std::cout << std::endl;
    
    memory->put_bytes(addr, (uint32_t)values.size(), &values[0]);
    return true;
}

bool ProcessTrace::fillBytes(uint32_t addr, uint32_t count, uint32_t value){
    std::cout << "fill " << std::hex << addr << " ";
    std::cout << std::hex << count << " " << std::hex << value << std::endl;
    
    std::vector<uint8_t> tmp;
    tmp.resize(count, value);
    
    memory->put_bytes(addr, count, &tmp[0]);
}

bool ProcessTrace::copyBytes(uint32_t dest_addr, uint32_t src_addr, uint32_t count){
    std::cout << "copy " << std::hex << dest_addr << " ";
    std::cout << std::hex << src_addr << " " << std::hex << count << std::endl;
    
    for (int i=0; i < count; i++){
        uint8_t b;
        memory->get_byte(&b , src_addr + i);
        memory->put_byte(dest_addr + i, &b);
    }
    
}

bool ProcessTrace::dumpBytes(uint32_t addr, uint32_t count){
    std::cout << "dump " << std::hex << addr << " " << std::hex << count << std::endl;
    std::cout << addr;
        
    std::stringstream line;
    uint8_t b;
    
    
    for(int i = 0; i < count; i++){
        memory->get_byte(&b, addr + i);
        if(i % 16 == 0){
            line << '\n';
        }
        
        line << " ";

        if(b < 16 ){
            line << "0";
        }

        line << std::hex << b + 0;
    }
    std::string s = line.str();
    std::cout << s << std::endl;

}