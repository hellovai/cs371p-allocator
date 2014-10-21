// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2014
// Glenn P. Downing
// ------------------------------------

// --------
// includes
// --------

#include <algorithm>  // count
#include <memory>     // allocator

#include "gtest/gtest.h"

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : testing::Test {
  // --------
  // typedefs
  // --------

  typedef A allocator_type;
  typedef typename A::value_type value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer pointer;

  static void assert_allocate(allocator_type x, pointer b, value_type v,
                              difference_type s) {
    ASSERT_FALSE(b == 0);
    pointer e = b + s;
    pointer p = b;
    try {
      while (p != e) {
        x.construct(p, v);
        ++p;
      }
    } catch (...) {
      while (b != p) {
        --p;
        x.destroy(p);
      }
      x.deallocate(b, s);
      throw;
    }
    ASSERT_EQ(s, std::count(b, e, v));
    while (b != e) {
      --e;
      x.destroy(e);
    }
  }
};

typedef testing::Types<std::allocator<int>, std::allocator<double>,
                       Allocator<int, 100>, Allocator<double, 100> > my_types;

TYPED_TEST_CASE(TestAllocator, my_types);

TYPED_TEST(TestAllocator, One) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 1;
  const value_type v = 2;
  const pointer b = x.allocate(s);

  TestFixture::assert_allocate(x, b, v, s);
  x.deallocate(b, s);
}

TYPED_TEST(TestAllocator, Ten) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 10;
  const value_type v = 2;
  const pointer b = x.allocate(s);

  TestFixture::assert_allocate(x, b, v, s);
  x.deallocate(b, s);
}

TYPED_TEST(TestAllocator, Two_Three) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 3;
  const value_type v = 2;
  const pointer b1 = x.allocate(s);
  const pointer b2 = x.allocate(s);

  TestFixture::assert_allocate(x, b1, v, s);
  TestFixture::assert_allocate(x, b2, v, s);
  x.deallocate(b1, s);
  x.deallocate(b2, s);
}

TYPED_TEST(TestAllocator, Four_Three) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 3;
  const value_type v = 2;
  const pointer b1 = x.allocate(s);
  const pointer b2 = x.allocate(s);
  const pointer b3 = x.allocate(s);
  x.deallocate(b2, s);
  const pointer b4 = x.allocate(s);
  ASSERT_EQ(b4, b2);

  TestFixture::assert_allocate(x, b1, v, s);
  TestFixture::assert_allocate(x, b3, v, s);
  TestFixture::assert_allocate(x, b4, v, s);
  x.deallocate(b1, s);
  x.deallocate(b3, s);
  x.deallocate(b4, s);
}

TYPED_TEST(TestAllocator, Over9000) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = x.max_size() + 1;
  const value_type v = 2;
  pointer b;
  try {
    b = x.allocate(s);
  } catch (std::bad_alloc e) {
    b = 0;
  }

  ASSERT_TRUE(b == 0);
}


TYPED_TEST(TestAllocator, Construct) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 1;
  const value_type v = 2;
  const pointer b = x.allocate(s);
  if (b != 0) {
    TestFixture::assert_allocate(x, b, v, s);
    x.construct(b, v);
    ASSERT_EQ(*b, v);
    x.deallocate(b, s);
  }
}

TYPED_TEST(TestAllocator, ConstructLoop) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 10;
  const value_type v = 2;
  const pointer b = x.allocate(s);
  if (b != 0) {
    TestFixture::assert_allocate(x, b, v, s);
    for (int i = 0; i < s; i++) {
      x.construct(b + i, v);
      ASSERT_EQ(*(b + i), v);
    }
    x.deallocate(b, s);
  }
}
