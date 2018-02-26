/*  PageAllocator - allocate pages in memory
 * 
 * File:   PageFrameAllocator.cpp
 * Author: Mike Goss <mikegoss@cs.du.edu>
 * Edited by: Evan Reierson, Nick Sundermeyer
 * 
 * 
 * Created on February 2, 2018, 3:47 PM
 */

#include "PageFrameAllocator.h"

#include <cstring>
#include <sstream>

using mem::Addr;
using mem::MMU;
using mem::PMCB;

PageFrameAllocator::PageFrameAllocator(uint32_t page_frame_count, MMU &memoryptr) 
:	memory(memoryptr),
	page_frames_total(page_frame_count),
        page_frames_free(page_frame_count),
        free_list_head(0)
{
    
    // add page frames to free list
  // Add all page frames to free list
  for (uint32_t frame = 0; frame < page_frame_count-1; ++frame) {
    uint8_t next[] = {0, 0, 0, frame + 1};
    memory.put_bytes(frame*kPageSize, sizeof(uint32_t), next);
    //memcpy(&memory[frame*kPageSize], &next, sizeof(uint32_t));
  }
  
  // Last page frame has end of list marker
  uint8_t end_list[] = {0xFF, 0xFF, 0xFF, 0xFF};
  memory.put_bytes((page_frame_count-1)*kPageSize, sizeof(uint32_t), end_list);

//  memcpy(&memory[(page_frame_count-1)*kPageSize], &end_list, sizeof(uint32_t));
}

bool PageFrameAllocator::Allocate(uint32_t count, 
                                  std::vector<uint32_t> &page_frames) {
  if (count <= page_frames_free) {  // if enough to allocate
    while (count-- > 0) {
      // Return next free frame to caller
      page_frames.push_back(free_list_head);
      uint8_t next[] = {0, 0, 0, free_list_head*kPageSize};
      // De-link frame from head of free list
      // memory.put_bytes(free_list_head, sizeof(uint32_t), next); // get bytes?
      // need to go from byte array to uint32 free list head
      memory.get_bytes(&free_list_head, &next, sizeof(uint32_t));
      
//      memcpy(&free_list_head, &memory[free_list_head*kPageSize], sizeof(uint32_t));
      --page_frames_free;
    }
    return true;
  } else {
    return false;  // do nothing and return error
  }
}

bool PageFrameAllocator::Deallocate(uint32_t count,
                                    std::vector<uint32_t> &page_frames) {
  // If enough to deallocate
  if(count <= page_frames.size()) {
    while(count-- > 0) {
        // Return next frame to head of free list
        uint32_t frame = page_frames.back();
        page_frames.pop_back();
        uint8_t next[] = {0, 0, 0, free_list_head};
        memory.put_bytes(frame * kPageSize, sizeof(uint32_t), next);

        //memcpy(&memory[frame * kPageSize], &free_list_head, sizeof(uint32_t));
        free_list_head = frame;
        ++page_frames_free;
    }
    return true;
  } else {
    return false; // do nothing and return error
  }
}

std::string PageFrameAllocator::FreeListToString(void) const {
  std::ostringstream out_string;
  
  uint8_t next_free = free_list_head;
  
  while (next_free != kEndList) {
    out_string << " " << std::hex << next_free;
    memory.get_bytes(&next_free, next_free*kPageSize, sizeof(uint32_t));
    //memcpy(&next_free, &memory[next_free*kPageSize], sizeof(uint32_t));
  }
  
  return out_string.str();
}