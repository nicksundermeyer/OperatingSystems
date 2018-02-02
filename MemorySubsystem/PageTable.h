/* 
 * Definitions for page table
 * 
 * MMU uses a two-level page table.  The top level (directory) consists of 1024
 * entries, each of which points to a second level 1024K-entry page table 
 * (if present).
 * 
 * File:   PageTable.h
 * Author: Mike Goss <mikegoss@cs.du.edu>
 *
 * Created on August 8, 2017, 3:32 PM
 */

#ifndef MEM_PAGETABLE_H
#define MEM_PAGETABLE_H

#include "MemoryDefs.h"

#include <array>

namespace mem {

// Define storage for a single page table entry
typedef Addr PageTableEntry;

// Page table size
const int  kPageTableSizeBits = 10;
const Addr kPageTableEntries = (1 << kPageTableSizeBits);
static_assert(kPageTableEntries == kPageSize / sizeof(PageTableEntry),
              "Page size and page table size mismatch");
const Addr kPageTableSizeBytes = kPageSize;
const Addr kPageTableIndexMask = kPageTableEntries - 1;

// Define bit masks and shifts for fields in page table entry

// The page frame number is stored in the upper 20 bits
const uint32_t kPTE_FrameMask = kPageNumberMask;

// Bit masks for other flags
const uint32_t kPTE_Present = 0;            // page present in memory
const uint32_t kPTE_PresentMask = (1 << kPTE_Present);
const uint32_t kPTE_Writable = 1;           // page is writable
const uint32_t kPTE_WritableMask = (1 << kPTE_Writable);
const uint32_t kPTE_Accessed = 5;           // set when page is accessed
const uint32_t kPTE_AccessedMask = (1 << kPTE_Accessed);
const uint32_t kPTE_Modified = 6;           // set when page is modified
const uint32_t kPTE_ModifiedMask = (1 << kPTE_Modified);

// Define type for a page table as a derived class from std::array.
// The page table is initialized to zero.
class PageTable : public std::array<PageTableEntry, kPageTableEntries> {
public:
  PageTable() {
    fill(0);
  }
};

}  // namespace mem

#endif /* MEM_PAGETABLE_H */

