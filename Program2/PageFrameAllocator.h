/*  PageAllocator - allocate pages in memory
 * 
 * File:   PageFrameAllocator.h
 * Author: Mike Goss <mikegoss@cs.du.edu>
 *
 * Created on February 2, 2018, 3:47 PM
 */

#ifndef PAGEFRAMEALLOCATOR_H
#define PAGEFRAMEALLOCATOR_H

#include <MMU.h>

#include <cstdint>
#include <string>
#include <vector>

class PageFrameAllocator {
public:
  /**
   * Constructor
   * 
   * Allocates the specified number of page frames, and builds free list of
   * all page frames.
   * 
   * @param page_frame_count
   */
  PageFrameAllocator(uint32_t page_frame_count, mem::MMU &memoryptr);
  
  virtual ~PageFrameAllocator() {}  // empty destrucor
  
  // Disallow copy/move
  PageFrameAllocator(const PageFrameAllocator &other) = delete;
  PageFrameAllocator(PageFrameAllocator &&other) = delete;
  PageFrameAllocator &operator=(const PageFrameAllocator &other) = delete;
  PageFrameAllocator &operator=(PageFrameAllocator &&other) = delete;
  
  /**
   * Allocate - allocate page frames from the free list
   * 
   * @param count number of page frames to allocate
   * @param page_frames page frame numbers allocated are pushed on back
   * @return true if success, false if insufficient page frames (no frames allocated)
   */
  bool Allocate(uint32_t count, std::vector<uint32_t> &page_frames);
  
  /**
   * Deallocate - return page frames to free list
   * 
   * @param count number of page frames to free
   * @param page_frames contains page frame numbers to deallocate; numbers are
   *   popped from back of vector
   * @return true if success, false if insufficient page frames in vector
   */
  bool Deallocate(uint32_t count, std::vector<uint32_t> &page_frames);
  
  // Access to private values
  uint32_t get_page_frames_free(void) const { return page_frames_free; }
  
  /**
   * FreeListToString - get string representation of free list
   * 
   * @return hex numbers of all free pages
   */
  std::string FreeListToString(void) const;
  
  static const uint32_t kPageSize = 0x1000;
private:
  // Reference to memory object, and the pmcb to control it
  mem::MMU &memory;
  
  // Number of first free page frame
  uint32_t free_list_head;
  
  // Total number of page frames
  uint32_t page_frames_total;
  
  // Current number of free page frames
  uint32_t page_frames_free;
  
  // End of list marker
  static const uint32_t kEndList = 0xFFFFFFFF;
};

#endif /* PAGEFRAMEALLOCATOR_H */
