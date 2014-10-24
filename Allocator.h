// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2014
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>    // assert
#include <cstddef>    // ptrdiff_t, size_t
#include <new>        // bad_alloc, new
#include <stdexcept>  // invalid_argument
#include <iostream>

// ---------
// Allocator
// ---------

template <typename T, int N>
class Allocator {
 public:
  // --------
  // typedefs
  // --------

  typedef T value_type;

  typedef std::size_t size_type;
  typedef std::ptrdiff_t difference_type;

  typedef value_type* pointer;
  typedef const value_type* const_pointer;

  typedef value_type& reference;
  typedef const value_type& const_reference;

 public:
  // -----------
  // operator ==
  // -----------

  friend bool operator==(const Allocator&, const Allocator&) {
    return true;
  }  // this is correct

  // -----------
  // operator !=
  // -----------

  friend bool operator!=(const Allocator& lhs, const Allocator& rhs) {
    return !(lhs == rhs);
  }

 private:
  // ----
  // data
  // ----

  char a[N];

  // -----
  // valid
  // -----

  /**
   * O(1) in space
   * O(n) in time
   * <your documentation>
   */
  bool valid() const {
    uint b = 0, e;
    while (b < N) {
      int v = view(b);

      e = b + sizeof(int) + (v < 0 ? -v : v);
      if (e >= N) return false;

      if (v != view(e)) return false;

      b = e + sizeof(int);
    }
    return true;
  }

  /**
   * O(1) in space
   * O(1) in time
   * <your documentation>
   */
  int& view(int i) { return *reinterpret_cast<int*>(&a[i]); }

  inline int min_block() { return (sizeof(T) + sizeof(int) * 2); }

 public:
  // ------------
  // constructors
  // ------------

  /**
   * O(1) in space
   * O(1) in time
   * throw a bad_alloc exception, if N is less than sizeof(T) + (2 *
   * sizeof(int))
   */
  Allocator() {
    if (N < sizeof(int) * 2) {
      // this does not match std::allocator::allocator as
      // std::allocator::allocator does not take a byte size
      throw std::bad_alloc();
    }
    view(0) = N - sizeof(int) * 2;
    view(N - sizeof(int)) = view(0);
    assert(valid());
  }

  // Default copy, destructor, and copy assignment
  // Allocator  (const Allocator&);
  // ~Allocator ();
  // Allocator& operator = (const Allocator&);

  // --------
  // allocate
  // --------

  /**
   * O(1) in space
   * O(n) in time
   * after allocation there must be enough space left for a valid block
   * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
   * choose the first block that fits
   * return 0, if allocation fails
   */
  pointer allocate(size_type n) {
    assert(valid());
    int size = n * sizeof(T);
    uint b = 0, e;
    while (b < N) {
      int v = view(b);
      e = b + sizeof(int) + (v < 0 ? -v : v);
      if (v >= size) {
        if (v - size < min_block()) {
          size = v;
        }
        view(b) = -(size);
        view(b + size + sizeof(int)) = view(b);

        if ((b + size + sizeof(int)) != e) {
          int diff = v - size - 2 * sizeof(int);
          view(b + size + sizeof(int) * 2) = diff;
          view(e) = diff;
        }
        return reinterpret_cast<pointer>(&a[b + sizeof(int)]);
      }
      b = e + sizeof(int);
    }
    throw std::bad_alloc();
  }  // replace!

  // ---------
  // construct
  // ---------

  /**
   * O(1) in space
   * O(1) in time
   * <your documentation>
   */
  void construct(pointer p, const_reference v) {
    new (p) T(v);  // this is correct and exempt
    assert(valid());
  }  // from the prohibition of new

  // ----------
  // deallocate
  // ----------

  /**
   * O(1) in space
   * O(1) in time
   * after deallocation adjacent free blocks must be coalesced
   * <your documentation>
   */
  void deallocate(pointer p, size_type t) {
    assert(valid());
    uint b1 = (uint)(reinterpret_cast<char*>(p) - a) - sizeof(int);
    uint b = 0, e;
    int size = t * sizeof(T);
    while (b < N) {
      int v = view(b);
      e = b + sizeof(int) + (v < 0 ? -v : v);
      if (b == b1) {
        if (-v >= size) {
          break;
        }
        throw std::invalid_argument("Invalid size_type t");
      }
      b = e + sizeof(int);
    }
    if (b >= N) throw std::invalid_argument("Invalid pointer p");

    if (b1 >= sizeof(int)) {
      int v = view(b1 - sizeof(int));
      if (v > 0) {
        b1 = b1 - sizeof(int) * 2 - v;
        size += sizeof(int) * 2 + view(b1);
      }
    }
    if (e < N - sizeof(int)) {
      int v = view(e + sizeof(int));
      if (v > 0) {
        e = e + sizeof(int) * 2 + v;
        size += sizeof(int) * 2 + view(e);
      }
    }

    if (size < -view(b1)) {
      size = -view(b1);
    }
    view(b1) = size;
    view(e) = size;
  }

  // -------
  // destroy
  // -------

  /**
   * O(1) in space
   * O(1) in time
   * throw an invalid_argument exception, if pointer is invalid
   * <your documentation>
   */
  void destroy(pointer p) {
    p->~T();  // this is correct
    assert(valid());
  }

  /**
   * O(1) in space
   * O(1) in time
   * <your documentation>
   */
  const int& view(int i) const { return *reinterpret_cast<const int*>(&a[i]); }
};

#endif  // Allocator_h
