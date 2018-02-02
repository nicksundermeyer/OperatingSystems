/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PageFrameAllocator.h
 * Author: Evan Reierson and Nick Sundermeyer
 *
 * Created on January 27, 2018, 5:04 PM
 */

#ifndef PAGEFRAMEALLOCATOR_H
#define PAGEFRAMEALLOCATOR_H

#include <iostream>
#include <vector>
#include <cstring>
#include <algorithm>

class PageFrameAllocator {
public:
    PageFrameAllocator(int page_frames);

    virtual ~PageFrameAllocator();

    // copy constructor
    PageFrameAllocator(const PageFrameAllocator& orig) = delete;

    // move constructor
    PageFrameAllocator(PageFrameAllocator &&orig) = delete;

    // copy assignment operator
    PageFrameAllocator &operator=(const PageFrameAllocator &orig) = delete;

    // move assignment operator
    PageFrameAllocator operator=(const PageFrameAllocator &&orig) = delete;

    /**
    * allocate - allocate page frames from the free list
    * 
    * @param count - number of page frames to allocate
    * @param page_frames - vector to which allocated page frames get pushed
    * @return - boolean success of allocating page frames
    */
    bool Allocate(uint32_t count, std::vector<uint32_t> &page_frames);

    /**
    * deallocate - deallocate page frames and return them to the free list
    * 
    * @param count - number of page frames to deallocate
    * @param page_frames - vector from which page frames get popped and then added back to the free list
    * @return - boolean success of deallocating page frames
    */
    bool Deallocate(uint32_t count, std::vector<uint32_t> &page_frames);

    // getters
    uint32_t get_page_frames_total() const { return page_frames_total;}
    uint32_t get_page_frames_free() const { return page_frames_free;}
    uint32_t get_free_list_head() const{ return free_list_head;}

    /**
    * get next page - returns number of next page in the free list
    * 
    * @param page_number - current page in list
    * @return - uint32_t next page in list
    */
    uint32_t get_next_page(uint32_t page_number);
    
    /**
    * set - set the next page in the free list
    * 
    * @param page_number - number of current page
    * @param next_page - number of next page to set
    */ 
    void set_next_page(uint32_t page_number, uint32_t next_page);

private:
    std::vector<uint8_t> memory;
    uint32_t page_frames_total;
    uint32_t page_frames_free;
    uint32_t free_list_head;
};

#endif /* PAGEFRAMEALLOCATOR_H */
