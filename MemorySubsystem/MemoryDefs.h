/* 
 * Definitions of widely used types and constants for the 
 * MemorySubsystem library
 * 
 * File:   MemoryDefs.h
 * Author: Mike Goss <mikegoss@cs.du.edu>
 *
 * Created on August 8, 2017, 4:00 PM
 */

#ifndef MEM_MEMORYDEFS_H
#define MEM_MEMORYDEFS_H

#include <cstdint>

namespace mem {

// Define address type (32 bits)
typedef uint32_t Addr;

// Define size of page (and page frame) in bytes (0x1000 == 4096).
// Also define masks for page number and offset.
const int  kPageSizeBits = 12;  // shift count for page size
const Addr kPageSize = (1 << kPageSizeBits);
const Addr kPageOffsetMask = (kPageSize - 1);
const Addr kPageNumberMask = ~kPageOffsetMask;

}  // namespace mem

#endif /* MEM_MEMORYDEFS_H */

