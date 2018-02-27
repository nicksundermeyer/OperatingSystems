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
#include <cstring>
#include <bitset>

using mem::Addr;
using mem::MMU;
using mem::PMCB;
using std::cin;
using std::cout;
using std::cerr;
using std::getline;
using std::istringstream;
using std::vector;
using std::bitset;
using std::memcpy;

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

    uint8_t zero[] = {0, 0, 0, 0};
    memory.put_bytes(page_frames[0], sizeof(uint32_t), zero);
    
    // put pmcb back in virtual mode, set base page table address
    memory.get_PMCB(temp);
    temp.vm_enable = true;
    temp.page_table_base = page_frames[0];
    memory.set_PMCB(temp);
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
            // set in virtual mode for most commands
            PMCB temp;
            memory.get_PMCB(temp);
            temp.vm_enable = true;
            memory.set_PMCB(temp);
            
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
	  } else if (cmd == "writable") {
              CmdWritable(line, cmd, cmdArgs);
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
    // set memory to physical mode
    PMCB temp;
    memory.get_PMCB(temp);
    temp.vm_enable = false;
    memory.set_PMCB(temp);
    
    // Get full vaddress and size of the memory to allocate
    Addr vaddr = cmdArgs.at(0);
    Addr size = cmdArgs.at(1);
    
    // check that address is correct
    if(vaddr % 0x1000 == 0 && size % 0x1000 == 0)
    {
	// Use shifts to mask for the left 10 bits (level 1 address) and middle 10 (level 2 address)
	Addr addr_l1 = vaddr >> 22;
	Addr addr_l2 = (vaddr >> 12) & 0xFFF;
	
	// calculate number of page frames needed
	Addr page_count = (size + mem::kPageSize - 1) / mem::kPageSize;        
	
	// allocate the page frames from the page frame allocator
	// store the frames temporarily in a vector
        std::vector<uint32_t> page_frames;
	pfa.Allocate(page_count, page_frames);
        
	// for each page frame we allocated
        for(int i=0; i<page_frames.size(); i++)
        {
	    // use helper function to set the page table to all 0's at the beginning
            clearFrame(page_frames[i]);
            
	    // writing level 2 table address to level 1 table
	    
            uint32_t page_table_entry = page_frames[i];
            
	    // get the page table entry vaddress, shift left for second level page frame number
	    // mask to set far right two bits (write allowed and page present) to 1
            page_table_entry = (page_table_entry << 13) | 3;
            
	    // convert to uint8_t array to use in put_bytes
            uint8_t arr[4];
            arr[0] = page_table_entry >> 24;
            arr[1] = page_table_entry >> 16;
            arr[2] = page_table_entry >> 8;
            arr[3] = page_table_entry;
            
	    // write bytes to memory at the base address + our offset
            memory.put_bytes(temp.page_table_base+addr_l1, sizeof(uint32_t), arr);
            
	    // writing offset in level 2 page table
	    
	    // take full vaddress and shift out the left 20 bits to leave the lower 12 bits
	    // mask with 3 to set write allowed and page present bits
            page_table_entry = vaddr;
            page_table_entry = (page_table_entry << 20) | 3;
            
	    // convert to uin8_t array
            arr[0] = page_table_entry >> 24;
            arr[1] = page_table_entry >> 16;
            arr[2] = page_table_entry >> 8;
            arr[3] = page_table_entry;
            
            // putting data in second level page table
            memory.put_bytes(page_frames[i]*4096 + addr_l2, sizeof(uint32_t), arr);
        }
    }
    else
    {
	std::cout << "vaddr or size not correct multiples" << std::endl;
    }
}

void ProcessTrace::clearFrame(uint32_t startAddress)
{
    uint8_t data = 0;
    
    for(int i=0; i<4096; i++)
    {
        memory.put_byte(startAddress + i, &data);
    }
}

void ProcessTrace::printFrame(uint32_t startAddress)
{
    uint8_t data;
    
    for(int i=0; i<4096; i++)
    {
        memory.get_byte(&data, (startAddress*4096) + i);
        std::cout << std::bitset<8>(data);
//        std::cout << " ";
    }
    std::cout << std::endl;
}

void ProcessTrace::CmdCompare(const std::string &line,
                              const std::string &cmd,
                              const vector<uint32_t> &cmdArgs) {
    
    uint32_t addr = cmdArgs.at(0);

//    std::cout << std::hex << addr << std::endl;
    memory.ToPhysical(addr, addr, false);
    std::cout << bitset<32>(addr) << std::endl;
    
    // Compare specified byte values
    size_t num_bytes = cmdArgs.size() - 1;
    uint8_t buffer[num_bytes];
    
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

void ProcessTrace::CmdWritable(const std::string &line, const std::string &cmd, const std::vector<uint32_t> &cmdArgs)
{
    
}