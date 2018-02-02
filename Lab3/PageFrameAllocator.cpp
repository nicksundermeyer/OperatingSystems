/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PageFrameAllocator.cpp
 * Author: Evan Reierson and Nick Sundermeyer
 *
 * Created on January 27, 2018, 5:04 PM
 */

#include "PageFrameAllocator.h"

PageFrameAllocator::PageFrameAllocator(int page_frames) {
    memory.resize(page_frames * 0x1000, 0);

    free_list_head = 0;
    page_frames_total = page_frames;
    page_frames_free = page_frames;

    uint32_t next_page;
    for (int i=0; i<page_frames; i++)
    {
        if (i == page_frames-1) {
                next_page = 0xFFFFFFFF;
        } else{
                next_page = i + 1;
        }
        set_next_page(i, next_page);
    }
}


PageFrameAllocator::~PageFrameAllocator() {
}


bool PageFrameAllocator::Allocate(uint32_t count, std::vector<uint32_t> &page_frames)
{
    if (count <= page_frames_free){
        uint32_t next_page;
        while(page_frames.size() < count){
            page_frames.push_back(free_list_head);
            free_list_head = get_next_page(free_list_head);
            page_frames_free --;
        }
        return true;
    } else {
        return false;
    }
}

bool PageFrameAllocator::Deallocate(uint32_t count, std::vector<uint32_t> &page_frames){
    if (count <= page_frames.size()){
        uint32_t page_frame;
        for (int i=0; i<count; i++){
            page_frame = page_frames.back();
            page_frames.pop_back();
            set_next_page(page_frame, free_list_head);
            free_list_head = page_frame;
            page_frames_free ++;
        }
        return true;
    } else {
        return false;
    }
}


void PageFrameAllocator::set_next_page(uint32_t page_number, uint32_t next_page){
    std::memcpy(&memory[page_number*0x1000], &next_page, sizeof(uint32_t));
}

uint32_t PageFrameAllocator::get_next_page(uint32_t page_number){
    uint32_t next_page = 0;
    uint8_t b;

    // attempt to account for endianness
    // might be broken here
    for (int i=3; i>=0; i--){
        b = memory[i + 0x1000 * page_number];
        next_page = (next_page << 8) | b;
    }

    return next_page;
}
