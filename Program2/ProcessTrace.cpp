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
  // put memory in physical mode
  std::vector<uint32_t> page_frames;
  PMCB temp;
  memory.get_PMCB(temp);
  temp.vm_enable = false;
  memory.set_PMCB(temp);
  
  // allocate one page frame for the top level page table
  pfa.Allocate(3, page_frames);
  std::cout << page_frames[0] << ":" << page_frames[1] << ":" << page_frames[2] << std::endl;
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
	try
	{
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
	catch(const std::exception& e)
	{
	    PMCB temp;
	    memory.get_PMCB(temp);
	    std::cout << "Exception: " << temp.next_vaddress << " " << e.what() << std::endl;
	    temp.operation_state = mem::PMCB::NONE;
	    memory.set_PMCB(temp);
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
    Addr vaddr = cmdArgs.at(0);
    Addr size = cmdArgs.at(1);
    
    if(vaddr % 0x1000 == 0 && size % 0x1000 == 0)
    {
	Addr page_count = (size + mem::kPageSize - 1) / mem::kPageSize;

	// make sure pmcb is in virtual mode
	pmcb.vm_enable = true;
	
	// allocate page frames for size, starting at vadress
//	pfa.Allocate(vaddr, size, )
    }
    else
    {
	std::cout << "vaddr or size not correct multiples" << std::endl;
    }
}

void ProcessTrace::CmdCompare(const std::string &line,
                              const std::string &cmd,
                              const vector<uint32_t> &cmdArgs) {
    
    uint32_t addr = cmdArgs.at(0);

//    std::cout << std::hex << addr << std::endl;
    memory.ToPhysical(addr, addr, false);
    
    // Compare specified byte values
    size_t num_bytes = cmdArgs.size() - 1;
    uint8_t buffer[num_bytes];
    
//    PMCB p;
//    memory.get_PMCB(p);
//    p.vm_enable = 0;
//    memory.set_PMCB(p);
    
    memory.get_bytes(buffer, addr, num_bytes);

    //std::cout << addr << std::endl;
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