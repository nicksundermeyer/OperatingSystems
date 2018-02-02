/* 
 * Interface to Virtual Memory (Memory Management Unit)
 * 
 * File:   MMU.cpp
 * Author: Mike Goss <mikegoss@cs.du.edu>
 * 
 * Created on August 7, 2017, 3:41 PM
 */

#include "MMU.h"

#include "Exceptions.h"

namespace mem {

void MMU::InitMemoryOperation(PMCB::PMCB_op op, 
                              Addr vaddress, 
                              Addr count, 
                              uint8_t* user_buffer) {
  pmcb.operation_state = op;
  pmcb.next_vaddress = vaddress;
  pmcb.remaining_count = count;
  pmcb.user_buffer = user_buffer;
}

void MMU::ToPhysical(Addr vaddress, Addr& paddress, bool write_op) {
  // If not in virtual memory mode, physical == virtual
  if (!pmcb.vm_enable) {
    paddress = vaddress;
    return;
  }
  
  // If address translation cached in TLB, use it
  PageTableEntry second_level_entry = 0;
  bool from_tlb = false;  // true if translation from TLB
  Addr second_level_entry_pa = 0xFFFFFFFF;  // phys addr of 2nd lvl entry
  
  if (tlb) {
    second_level_entry = tlb->Lookup(vaddress);
    // Use TLB entry if page present. If this is a write and the modified
    // bit is not set in the cache, force use of page table so that 
    // the modified bit will be updated in the page table.
    from_tlb = (second_level_entry & kPTE_PresentMask) != 0
            && (!write_op || ((second_level_entry & kPTE_ModifiedMask) != 0));
  }

  if (!from_tlb) {
    // Check for valid top level page table pointer
    if((pmcb.page_table_base & kPageOffsetMask) != 0) { // must start at page boundary
      throw InvalidMMUOperationException("PMCB Error: page table base must be at page boundary");
    }

    // Get entry in top level page table
    Addr top_level_index = (vaddress >> (kPageSizeBits + kPageTableSizeBits))
            & kPageTableIndexMask;
    PageTableEntry top_level_entry;
    Addr top_level_entry_pa =
            pmcb.page_table_base + top_level_index * sizeof(PageTableEntry);
    phys_mem.get_32(&top_level_entry, top_level_entry_pa);
    if((top_level_entry & kPTE_PresentMask) == 0) {
      throw PageFaultException();
    }

    // Set accessed and (optionally) modified flags for top level table
    if((top_level_entry & kPTE_AccessedMask) == 0) {
      top_level_entry = top_level_entry | kPTE_AccessedMask
              | (write_op ? kPTE_ModifiedMask : 0);
      phys_mem.put_bytes(top_level_entry_pa, sizeof(PageTableEntry),
                         reinterpret_cast<uint8_t*> (&top_level_entry));
    }

    // Get 2nd level page table entry
    Addr second_level_address = top_level_entry & kPTE_FrameMask;
    Addr second_level_index = (vaddress >> kPageSizeBits) & kPageTableIndexMask;
    second_level_entry_pa =
            second_level_address + second_level_index * sizeof(PageTableEntry);
    phys_mem.get_32(&second_level_entry, second_level_entry_pa);
  }
  
  // If page not present, throw page fault exception
  if ((second_level_entry & kPTE_PresentMask) == 0) {
    throw PageFaultException();
  }
  
  // If write operation and page not writable, throw exception
  if (write_op && (second_level_entry & kPTE_WritableMask) == 0) {
    throw WritePermissionFaultException();
  }
  
  // If address not from TLB, set accessed and (optionally) modified flags 
  // in 2nd level table, then update the TLB
  if (!from_tlb) {
    PageTableEntry new_second_level_entry = second_level_entry | kPTE_AccessedMask
            | (write_op ? kPTE_ModifiedMask : 0);

    // If changed, write back to page table
    if (new_second_level_entry != second_level_entry) {
      second_level_entry = new_second_level_entry;
      phys_mem.put_bytes(second_level_entry_pa, sizeof(PageTableEntry),
                        reinterpret_cast<uint8_t*>(&second_level_entry));
    }
    
    // Update TLB
    if (tlb) {
      tlb->Cache(vaddress, second_level_entry);
    }
  }
  
  // Page is mapped, return physical
  paddress = (second_level_entry & kPTE_FrameMask) | (vaddress & kPageOffsetMask);
}

void MMU::Execute() {
  if (pmcb.operation_state == PMCB::NONE) return;
  
  if (pmcb.operation_state != PMCB::READ_OP && pmcb.operation_state != PMCB::WRITE_OP) {
    throw InvalidMMUOperationException("PMCB Error: operation is invalid");
  }
  
  while (pmcb.remaining_count > 0) {
    // Check if next page is mapped and has correct write permission
    Addr next_paddress;
    ToPhysical(pmcb.next_vaddress, next_paddress, 
               pmcb.operation_state == PMCB::WRITE_OP);
    
    // Determine remaining count within current page
    Addr count_in_page = std::min(pmcb.remaining_count,
                                  kPageSize - (pmcb.next_vaddress & kPageOffsetMask));
    
    // Transfer bytes
    if (pmcb.operation_state == PMCB::READ_OP) {
      phys_mem.get_bytes(pmcb.user_buffer, next_paddress, count_in_page);
    } else {  // write
      phys_mem.put_bytes(next_paddress, count_in_page, pmcb.user_buffer);
    }
    
    // Advance state of transfer
    pmcb.next_vaddress += count_in_page;
    pmcb.user_buffer += count_in_page;
    pmcb.remaining_count -= count_in_page;
  }
}

void MMU::get_byte(uint8_t *dest, Addr vaddress) {
  InitMemoryOperation(PMCB::READ_OP, vaddress, 1, dest);
  Execute();
}

void MMU::get_bytes(uint8_t *dest, Addr vaddress, Addr count) {
  InitMemoryOperation(PMCB::READ_OP, vaddress, count, dest);
  Execute();
}

void MMU::put_byte(Addr vaddress, uint8_t *data) {
  InitMemoryOperation(PMCB::WRITE_OP, vaddress, 1, data);
  Execute();  
}

void MMU::put_bytes(Addr vaddress, Addr count, uint8_t *src) {
  InitMemoryOperation(PMCB::WRITE_OP, vaddress, count, src);
  Execute();
}

void MMU::set_PMCB(const PMCB &new_pmcb) {
  pmcb = new_pmcb;
  if (pmcb.remaining_count > 0) {
    Execute();
  }
}

void MMU::get_TLBStats(TLB::TLBStats& stats) {
  if (tlb.get() != nullptr) {
    tlb->get_stats(stats);
  } else {
    throw InvalidMMUOperationException("TLB is not enabled, stats not available");
  }
}

}  // namespace mem