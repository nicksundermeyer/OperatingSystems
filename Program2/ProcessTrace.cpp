/*
 * ProcessTrace implementation 
 */

/* 
 * File:   ProcessTrace.cpp
 * Author: Mike Goss <mikegoss@cs.du.edu>
 * 
 */

#include "ProcessTrace.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>

using mem::Addr;
using mem::MMU;
using mem::PMCB;
using std::cin;
using std::cout;
using std::cerr;
using std::getline;
using std::istringstream;
//using std::string;
using std::vector;

ProcessTrace::ProcessTrace(std::string file_name_, MMU &memoryptr, PMCB &pmcbptr, PageFrameAllocator &pfaptr) 
:	file_name(file_name_), 
	line_number(0),
	memory(memoryptr),
	pmcb(pmcbptr),
        pfa(pfaptr)
{
  // Open the trace file.  Abort program if can't open.
  trace.open(file_name, std::ios_base::in);
  if (!trace.is_open()) {
    cerr << "ERROR: failed to open trace file: " << file_name << "\n";
    exit(2);
  }
  
  // create first-level page table (lines 54-59 in MMUTests.cpp)
  // write method for second-level page tables?
  
}

ProcessTrace::~ProcessTrace() {
  trace.close();
}

void ProcessTrace::Execute(void) {
    // Read and process commands
    std::string line;                // text line read
    std::string cmd;                 // command from line
    vector<uint32_t> cmdArgs;   // arguments from line
  
    // Select the command to execute
    
    while (ParseCommand(line, cmd, cmdArgs)) {
        if (cmd == "alloc" ) {
          CmdAlloc(line, cmd, cmdArgs);    // allocate memory
        } else if (cmd == "compare") {
          CmdCompare(line, cmd, cmdArgs);  // get and compare multiple bytes
        } else if (cmd == "put") {
          CmdPut(line, cmd, cmdArgs);      // put bytes
        } else if (cmd == "fill") {
          CmdFill(line, cmd, cmdArgs);     // fill bytes with value
        } else if (cmd == "copy") {
          CmdCopy(line, cmd, cmdArgs);     // copy bytes to dest from source
        } else if (cmd == "dump") {
          CmdDump(line, cmd, cmdArgs);     // dump byte values to output
        } else if (cmd == "#"){}
        else {
          cerr << "ERROR: invalid command at line " << line_number << ":\n" 
                  << line << "\n";
          exit(2);
        }
    }
}

bool ProcessTrace::ParseCommand(
    std::string &line, std::string &cmd, vector<uint32_t> &cmdArgs) {
  cmdArgs.clear();
  line.clear();
  
  // Read next line
  if (std::getline(trace, line)) {
    ++line_number;
    cout << std::dec << line_number << ":" << line << "\n";
    
    // Make a string stream from command line
    istringstream lineStream(line);
    
    // Get command
    lineStream >> cmd;
    
    // Get arguments
    uint32_t arg;
        while (lineStream >> std::hex >> arg) {
            cmdArgs.push_back(arg);
        }
    return true;
  } else if (trace.eof()) {
      return false;
  } else {
    cerr << "ERROR: getline failed on trace file: " << file_name 
            << "at line " << line_number << "\n";
    exit(2);
  }
}

void ProcessTrace::CmdAlloc(const std::string &line, 
                            const std::string &cmd, 
                            const vector<uint32_t> &cmdArgs) {
    // Allocate the specified memory size
    Addr page_count = (cmdArgs.at(0) + mem::kPageSize - 1) / mem::kPageSize;
    for (Addr i=0; i<page_count; i++){
        //pfa.Allocate(1, )
    }
    
}

void ProcessTrace::CmdCompare(const std::string &line,
                              const std::string &cmd,
                              const vector<uint32_t> &cmdArgs) {
    
    uint32_t addr = cmdArgs.at(0);

    try{
        std::cout << "yolo " <<  std::hex << addr << std::endl;
        memory.ToPhysical(addr, addr, false);
    } catch (mem::PageFaultException& some_shit){
        
    }
    
    // Compare specified byte values
    size_t num_bytes = cmdArgs.size() - 1;
    uint8_t buffer[num_bytes];
    
//    PMCB p;
//    memory.get_PMCB(p);
//    p.vm_enable = 0;
//    memory.set_PMCB(p);
    
    memory.get_bytes(buffer, addr, num_bytes);

    //std::cout << "yolo" <<  addr << std::endl;
    for (int i = 1; i < cmdArgs.size(); ++i) {
      if(buffer[i-1] != cmdArgs.at(i)) {
        cout << "compare error at address " << std::hex << addr
                << ", expected " << static_cast<uint32_t>(cmdArgs.at(i))
                << ", actual is " << static_cast<uint32_t>(buffer[i-1]) << "\n";
      }
      ++addr;
    }
}

void ProcessTrace::CmdPut(const std::string &line,
                          const std::string &cmd,
                          const vector<uint32_t> &cmdArgs) {
  // Put multiple bytes starting at specified address
  uint32_t addr = cmdArgs.at(0);
  size_t num_bytes = cmdArgs.size() - 1;
  uint8_t buffer[num_bytes];
  for (int i = 1; i < cmdArgs.size(); ++i) {
     buffer[i-1] = cmdArgs.at(i);
  }
  memory.put_bytes(addr, num_bytes, buffer);
}

void ProcessTrace::CmdCopy(const std::string &line,
                           const std::string &cmd,
                           const vector<uint32_t> &cmdArgs) {
  // Copy specified number of bytes to destination from source
  Addr dst = cmdArgs.at(0);
  Addr src = cmdArgs.at(1);
  Addr num_bytes = cmdArgs.at(2);
  uint8_t buffer[num_bytes];
  memory.get_bytes(buffer, src, num_bytes);
  memory.put_bytes(dst, num_bytes, buffer);
}

void ProcessTrace::CmdFill(const std::string &line,
                          const std::string &cmd,
                          const vector<uint32_t> &cmdArgs) {
  // Fill a sequence of bytes with the specified value
  Addr addr = cmdArgs.at(0);
  Addr num_bytes = cmdArgs.at(1);
  uint8_t val = cmdArgs.at(2);
  for (int i = 0; i < num_bytes; ++i) {
    memory.put_byte(addr++, &val);
  }
}

void ProcessTrace::CmdDump(const std::string &line,
                          const std::string &cmd,
                          const vector<uint32_t> &cmdArgs) {
  uint32_t addr = cmdArgs.at(0);
  uint32_t count = cmdArgs.at(1);

  // Output the address
  cout << std::hex << addr;

  // Output the specified number of bytes starting at the address
  for(int i = 0; i < count; ++i) {
    if((i % 16) == 0) {  // line break every 16 bytes
      cout << "\n";
    }
    uint8_t byte_val;
    memory.get_byte(&byte_val, addr++);
    cout << " " << std::setfill('0') << std::setw(2)
            << static_cast<uint32_t> (byte_val);
  }
  cout << "\n";
}