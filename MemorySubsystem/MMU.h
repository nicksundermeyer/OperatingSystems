/* 
 * Interface to Virtual Memory (Memory Management Unit)
 * 
 * File:   MMU.h
 * Author: Mike Goss <mikegoss@cs.du.edu>
 *
 * Created on August 7, 2017, 3:41 PM
 * 
 */

#ifndef MEM_MMU_H
#define MEM_MMU_H

#include "PageTable.h"
#include "PMCB.h"
#include "TLB.h"

#include <memory>

namespace mem {

class MMU {
public:
/**
   * Constructor (TLB enabled)
   * 
   * MMU is initialized with virtual memory disabled (pmcb is 0). 
   * Set the PMCB non-zero to enable it.
   * 
   * @param frame_count_ number of page frames to allocate in physical memory
   * @param tlb_size_ number of entries in TLB (must be > 0)
   * @throws std::bad_alloc if insufficient memory
   */
  MMU(Addr frame_count_, size_t tlb_size)
  : frame_count(frame_count_),
    phys_mem(frame_count_ * kPageSize),
    tlb(std::make_unique<TLB>(tlb_size)) {
  };
  
  /**
   * Constructor (TLB disabled)
   * 
   * MMU is initialized with virtual memory disabled (pmcb is 0). 
   * Set the PMCB non-zero to enable it. No TLB is used with MMU.
   * 
   * @param frame_count_ number of page frames to allocate in physical memory
   * @throws std::bad_alloc if insufficient memory
   */
  MMU(Addr frame_count_) 
  : frame_count(frame_count_), 
    phys_mem(frame_count_ * kPageSize),
    tlb(nullptr)
  {
  };
  
  ~MMU() { }
  
  MMU(const MMU &other) = delete;  // no copy constructor
  MMU(MMU &&other) = delete;       // no move constructor
  MMU operator=(const MMU &other) = delete;  // no copy assign
  MMU operator=(MMU &&other) = delete;       // no move assign

  /**
   * get_frame_count - return number of page frames allocated
   * 
   * @return number of page frames in physical memory 
   */
  Addr get_frame_count() const { return frame_count; }
  
  /**
   * get_byte - get a single byte from the specified virtual address
   * 
   * @param dest - destination byte
   * @param vaddress - address of data in memory
   */
  void get_byte(uint8_t *dest, Addr vaddress);
  
  /**
   * get_bytes - copy a range of bytes to caller buffer
   * 
   * @param dest where to copy to
   * @param address source virtual address
   * @param count number of bytes to copy
   */
  void get_bytes(uint8_t *dest, Addr vaddress, Addr count);

  /**
   * put_byte - store a single byte to the specified virtual address
   * 
   * @param vaddress - address of data in memory
   * @param data - pointer to data to store at address
   */
  void put_byte(Addr vaddress, uint8_t *data);

  /**
   * put_bytes - copy a range of bytes into physical memory
   * 
   * @param vaddress virtual address destination
   * @param count number of bytes to copy
   * @param src source buffer
   */
  void put_bytes(Addr vaddress, Addr count, uint8_t *src);
  
  /**
   * set_PMCB - set the Processor Memory Control Block to be used by the MMU
   * 
   * If the remaining_count is > 0, the read or write operation will
   * resume. The specified PMCB is copied into the MMU.
   * 
   * IMPORTANT: for a resumed operation, make sure that the user_buffer still
   * points to a valid location within the original buffer, and that the
   * original buffer has not be deallocated!
   * 
   * NOTE: may throw a page fault or other exception during execution of a
   * resumed operation; calling code must be able to handle this correctly.
   * 
   * @param new_pmcb - start using this PMCB
   */
  void set_PMCB(const PMCB &new_pmcb);
  
  /**
   * get_PMCB - get a copy of the current PMCB contents
   * 
   * @param cur_pmcb location to store PMCB contents
   */
  void get_PMCB(PMCB &cur_pmcb) const { cur_pmcb = pmcb; }
  
  /**
   * ToPhysical - convert virtual address to physical address.
   * 
   * If virtual mode is enabled, map a virtual address to a physical address. 
   * If the TLB is enabled and the page containing the virtual address is in 
   * the TLB, the page table is not consulted. Otherwise, the address is mapped
   * using the page table, and the mapping is cached in the TLB (if enabled).
   * 
   * If virtual mode is disabled, paddress is set to vaddress. The TLB is
   * unused and unchanged in this case.
   * 
   * @param vaddress virtual address to map
   * @param paddress returns corresponding physical address (undefined if not mapped)
   * @param write_op true if mapping for a write operation
   * @throws PageFaultException if address unmapped
   * @throws WritePermissionFaultException if write op and page is not writable
   * @throws InvalidMMUOperationException if bad page table or other errors
   */
  void ToPhysical(Addr vaddress, Addr &paddress, bool write_op);
  
  /**
   * get_byte_count - return total number of bytes transferred so far 
   *   to/from physical memory.
   * 
   * @return count of bytes transferred.
   */
  uint64_t get_byte_count() const { return phys_mem.get_byte_count(); }
  
  /**
   * isTLBEnabled - query whether MMU has TLB enabled
   * 
   * @return true if TLB enabled, false otherwise
   */
  bool isTLBEnabled() const { return tlb.get() != nullptr; }
  
  /**
   * FlushTLB - flush the TLB
   */
  void FlushTLB() { if (tlb) tlb->Flush(); }
  
  /**
   * get_TLBStats - get TLB statistics
   * 
   * @param stats statistics from TLB
   * @throws InvalidMMUOperationException if TLB not enabled
   */
  void get_TLBStats(TLB::TLBStats &stats);
  
private:
  Addr frame_count;  // number of frames allocated in physical memory
  PhysicalMemory phys_mem;
  PMCB pmcb;  // current MMU control information
  
  // TLB (null if TLB disabled)
  std::unique_ptr<TLB> tlb;
  
  /**
   * InitMemoryOperation - setup memory operation in PMCB
   * 
   * @param op - READ_OP or WRITE_OP
   * @param vaddress virtual address
   * @param count number of bytes
   * @param user_buffer pointer to caller buffer (must be at least count bytes)
   */
  void InitMemoryOperation(PMCB::PMCB_op op, 
                           Addr vaddress, 
                           Addr count, 
                           uint8_t *user_buffer);
  
  /**
   * Execute - execute a (possibly partially complete) operation using the
   *           current PMCB contents.  The PMCB should contain the state of the
   *           operation as expected stored by MMU.  Executing when no
   *           partially completed operation is pending 
   *           (PMCB.operation_state == NONE) will result in an 
   *           InvalidMMUOperationException.
   */
  void Execute(void);
};

}  // namespace mem

#endif /* MEM_MMU_H */

