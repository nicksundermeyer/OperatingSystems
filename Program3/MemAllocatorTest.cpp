/* 
 * File:   MemAllocatorTest
 * Author: mikegoss
 *
 * Created on Oct 28, 2017, 6:12:10 PM
 */

#include <gtest/gtest.h>

#include "MemAllocator.h"

#include <vector>

using mem::Addr;
using std::vector;

class MemAllocatorTest : public testing::Test {
protected:

  void SetUp() {
    // Setup ...
  }

  void TearDown() {
    // Teardown ...
  }

};

TEST_F(MemAllocatorTest, AllocDealloc) {
  const Addr kNFrames = 8;
  MemAllocator allocator(kNFrames);
  
  // Check initial counts
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
  ASSERT_EQ(kNFrames, allocator.get_free_count());
  
  // Allocate some frames
  const Addr kNAlloc = 3;
  vector<Addr> allocated1;
  allocator.AllocateFrames(kNAlloc, allocated1);
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
  ASSERT_EQ(kNFrames - kNAlloc, allocator.get_free_count());
  ASSERT_EQ(kNAlloc, allocated1.size());
  for (size_t i = 0; i < allocated1.size(); ++i) {
    ASSERT_EQ(0, allocated1[i] & mem::kPageOffsetMask);
    ASSERT_NE(allocated1[i], allocated1[(i+1)%kNAlloc]);
  }
  
  // Allocate some more frames
  vector<Addr> allocated2;
  allocator.AllocateFrames(kNAlloc, allocated2);
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
  ASSERT_EQ(kNFrames - 2*kNAlloc, allocator.get_free_count());
  ASSERT_EQ(kNAlloc, allocated2.size());
  for (size_t i = 0; i < allocated2.size(); ++i) {
    ASSERT_EQ(0, allocated2[i] & mem::kPageOffsetMask);
    ASSERT_NE(allocated2[i], allocated2[(i+1)%kNAlloc]);
  }
  
  // Try to allocate more than remaining frames
  vector<Addr> allocated3;
  ASSERT_THROW(allocator.AllocateFrames(allocator.get_free_count()+1, allocated3),
    std::bad_alloc);
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
  ASSERT_EQ(kNFrames - 2*kNAlloc, allocator.get_free_count());
  ASSERT_TRUE(allocated3.empty());
  
  // Try to allocate all remaining frames
  const Addr kNRemain = allocator.get_free_count();
  allocator.AllocateFrames(allocator.get_free_count(), allocated3);
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
  ASSERT_EQ(0, allocator.get_free_count());
  ASSERT_EQ(kNRemain, allocated3.size());
  for (size_t i = 0; i < allocated3.size(); ++i) {
    ASSERT_EQ(0, allocated3[i] & mem::kPageOffsetMask);
    ASSERT_NE(allocated3[i], allocated3[(i+1)%kNAlloc]);
  }
  
  // Free some  allocated frames
  allocator.FreeFrames(allocated1);
  ASSERT_EQ(allocated1.size(), allocator.get_free_count());
  allocator.FreeFrames(allocated3);
  ASSERT_EQ(allocated1.size()+allocated3.size(), allocator.get_free_count());
  allocated1.clear();
  allocated3.clear();
  
  // Allocate some more frames
  allocator.AllocateFrames(kNAlloc, allocated1);
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
  ASSERT_EQ(kNFrames - 2*kNAlloc, allocator.get_free_count());
  ASSERT_EQ(kNAlloc, allocated1.size());
  for (size_t i = 0; i < allocated1.size(); ++i) {
    ASSERT_EQ(0, allocated1[i] & mem::kPageOffsetMask);
    ASSERT_NE(allocated1[i], allocated1[(i+1)%kNAlloc]);
  }

  allocator.FreeFrames(allocated1);
  allocator.FreeFrames(allocated2);
  ASSERT_EQ(kNFrames, allocator.get_free_count());
  ASSERT_EQ(kNFrames, allocator.get_frame_count());
}

