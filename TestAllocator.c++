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
struct SelfTestAllocator : testing::Test {
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

typedef testing::Types<std::allocator<int>, std::allocator<double>, std::allocator<bool>,
                       Allocator<int, 100>, Allocator<double, 100>, Allocator<bool, 100> > my_types;

typedef testing::Types<Allocator<int, 100>, Allocator<double, 100>, Allocator<bool, 100> > self_types;

TYPED_TEST_CASE(TestAllocator, my_types);
TYPED_TEST_CASE(SelfTestAllocator, self_types);

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

TYPED_TEST(SelfTestAllocator, Max_one) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 1;
  const value_type v = 1;
  int counter = 100 / (sizeof(value_type) + sizeof(int) * 2);
  pointer* bs = new pointer[counter];

  for(int i = 0; i < counter; i++) {
    bs[i] = x.allocate(s);
    TestFixture::assert_allocate(x, bs[i], v, s);
  }

  try {
    x.allocate(s);
    ASSERT_TRUE(0);
  } catch (std::bad_alloc e) {
    x.deallocate(bs[counter - 1], s);
  }

  bs[counter - 1] = x.allocate(s);
  TestFixture::assert_allocate(x, bs[counter - 1], v, s);

  for (int i = 0; i < counter; i++) {
    x.deallocate(bs[i], s);
  }
  delete bs;
  ASSERT_TRUE(1);
}

TYPED_TEST(SelfTestAllocator, Over9000) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 100 << 1;
  const value_type v = 2;
  try {
    pointer b = x.allocate(s);
    ASSERT_TRUE(0);
  } catch (std::bad_alloc e) {
    ASSERT_TRUE(1);
  }
}


TYPED_TEST(TestAllocator, Construct) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;


  const difference_type s = 1;
  const value_type v = 2;

  allocator_type x;
  const pointer b = x.allocate(s);
  TestFixture::assert_allocate(x, b, v, s);
  x.construct(b, v);
  ASSERT_EQ(*b, v);
  x.destroy(b);
  x.deallocate(b, s);
  ASSERT_TRUE(1);
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
  TestFixture::assert_allocate(x, b, v, s);
  for (int i = 0; i < s; i++) {
    x.construct(b + i, v);
    ASSERT_EQ(*(b + i), v);
  }
  for (int i = 0; i < s; ++i) {
    x.destroy(b + i);
  }
  x.deallocate(b, s);
  ASSERT_TRUE(1);
}

TYPED_TEST(SelfTestAllocator, Bad_dealloc_1) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 10;
  const value_type v = 2;
  const pointer b = x.allocate(s);
  TestFixture::assert_allocate(x, b, v, s);
  try {
    x.deallocate(b, s << 1);
    ASSERT_TRUE(0);
  } catch (...) {

  }
  x.deallocate(b, s);
  ASSERT_TRUE(1);
}

TYPED_TEST(SelfTestAllocator, Bad_dealloc_2) {
  typedef typename TestFixture::allocator_type allocator_type;
  typedef typename TestFixture::value_type value_type;
  typedef typename TestFixture::difference_type difference_type;
  typedef typename TestFixture::pointer pointer;

  allocator_type x;
  const difference_type s = 10;
  const value_type v = 2;
  pointer b = x.allocate(s);
  TestFixture::assert_allocate(x, b, v, s);
  try {
    x.deallocate(b - 1, s);
    ASSERT_TRUE(0);
  } catch (...) {

  }
  x.deallocate(b, s);
  ASSERT_TRUE(1);
}

TEST(Various, Test1) {
  try {
    Allocator<int, 1> x;
  } catch (std::bad_alloc e) {
    ASSERT_TRUE(1);
  }
}


TEST(Various, Test2) {
  Allocator<int, 8> x;
  try {
    x.allocate(1);
    ASSERT_TRUE(0);
  } catch (std::bad_alloc e) {
    ASSERT_TRUE(1);
  }
}
