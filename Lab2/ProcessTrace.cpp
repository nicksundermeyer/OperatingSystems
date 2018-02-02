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

ProcessTrace::ProcessTrace(string file_name) {
    file.open(file_name);
    
    // make sure file read is valid
    if (!file){
        cerr << "cannot open file" << endl;
        exit(1);
    }
}

ProcessTrace::~ProcessTrace() {
    file.close(); 
}

bool ProcessTrace::execute(){
    string current_line;
    int line_number = 1;
    while (getline(file, current_line)){
        istringstream iss(current_line);
        cout << dec << line_number << ':';
        parseCommand(iss);
        line_number ++;
    }
}

void ProcessTrace::parseCommand(istringstream& iss){
    string command;
    iss >> command;

    if (command == "alloc"){
        int size;

        iss >> hex >> size;

        ProcessTrace::alloc(size);

    } else if (command == "compare"){
        int addr;
        vector<uint8_t> expected_values;

        iss >> hex >> addr;
        
        int v;
        while(iss >> hex >> v){
            expected_values.push_back(v);
        }

        ProcessTrace::compareBytes(addr, expected_values);

    } else if (command == "put"){
        int addr;
        vector<uint8_t> values;

        iss >> hex >> addr;
        
        int v;
        while(iss >> hex >> v){
            values.push_back(v);
        }
        
        ProcessTrace::putBytes(addr, values);

    } else if (command == "fill"){
        int addr;
        int count;
        int value;

        iss >> hex >> addr;
        iss >> hex >> count;
        iss >> hex >> value;

        ProcessTrace::fillBytes(addr, count, value);

    } else if (command == "copy"){
        int dest_addr;
        int src_addr;
        int count;

        iss >> hex >> dest_addr;
        iss >> hex >> src_addr;
        iss >> hex >> count;

        ProcessTrace::copyBytes(dest_addr, src_addr, count);

    } else if (command == "dump"){
        int addr;
        int count;

        iss >> hex >> addr;
        iss >> hex >> count;
        
        ProcessTrace::dumpBytes(addr, count);

    } else {
        cerr << "oops, unknown command" << endl;
    }
}

bool ProcessTrace::alloc(int size){
    cout << "alloc " << hex << size << endl;
    if (size < 0){
        cerr << "invalid size" << endl;
        return false;
    }
    ProcessTrace::memory.resize(size);
    return true;
}

bool ProcessTrace::compareBytes(int addr, vector<uint8_t> expected_values){
    cout << "compare " << hex << addr;
    for (int i=0; i < expected_values.size(); i++){
        cout << " " << hex << expected_values[i] + 0;
    }
    cout << endl;
    for (int i=0; i < expected_values.size(); i++){
        if (expected_values[i] != ProcessTrace::memory[addr + i]){
            cout << "compare error at address " << hex << addr + i;
            cout << ", expected " << hex << expected_values[i] + 0;
            cout << ", actual is " << hex << ProcessTrace::memory[addr + i] + 0;
            cout << endl;
        }
    }
    
}
bool ProcessTrace::putBytes(int addr, vector<uint8_t> values){
    cout << "put " << hex << addr;
    for (int i=0; i < values.size(); i++){
        cout << " " << hex << values[i] + 0;
    }
    cout << endl;
    for (int i=0; i < values.size(); i++){
        ProcessTrace::memory[addr + i] = values[i];
    }
    return true;
}

bool ProcessTrace::fillBytes(int addr, int count, int value){
    cout << "fill " << hex << addr << " ";
    cout << hex << count << " " << hex << value << endl;
    for (int i=0; i < count; i++){
        ProcessTrace::memory[addr + i] = value;
    }
}

bool ProcessTrace::copyBytes(int dest_addr, int src_addr, int count){
    cout << "copy " << hex << dest_addr << " ";
    cout << hex << src_addr << " " << hex << count << endl;
    vector<uint8_t> cp;
    for (int i=0; i < count; i++){
        cp.push_back(ProcessTrace::memory[src_addr + i]);
    }
    
    for (int i=0; i < count; i++){
        ProcessTrace::memory[dest_addr + i] = cp[i];
    }
}

bool ProcessTrace::dumpBytes(int addr, int count){
    cout << "dump " << hex << addr << " " << hex << count << endl;
    cout << addr;
        
    stringstream line;

    for(int i = 0; i < count; i++){
        if(i % 16 == 0){
            line << '\n';
        }
        
        line << " ";

        if(ProcessTrace::memory[addr+i] < 16 ){
            line << "0";
        }

        line << hex << ProcessTrace::memory[addr+i] + 0;
    }
    string s = line.str();
    cout << s << endl;

}