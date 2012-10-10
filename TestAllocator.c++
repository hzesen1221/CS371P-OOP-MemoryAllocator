// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2011
// Glenn P. Downing
// ------------------------------------

/**
 * To test the program:
 *     g++ -ansi -pedantic -lcppunit -ldl -Wall TestAllocator.c++ -o TestAllocator.app
 *     valgrind TestAllocator.app >& TestAllocator.out
 */

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator
#define private public
#define ALLO_SIZE 100
#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_one: provided test
    // --------

    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}


    // --------
    // test_ten: provided test
    // --------

    void test_ten () {
        A x;
        const difference_type s = 10;
        const value_type      v = 2;
        const pointer         b = x.allocate(s);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}


    // --------
    // bad_allocation1: trying to allocate more than the space permits, an exception thrown.
    // --------

    void bad_allocation1 () {
        A x;
        try {
        const pointer p = x.allocate(15);
        x.destroy(p);
        x.deallocate(p, 0);
        }
        catch (std::bad_alloc& e)
        {
            CPPUNIT_ASSERT(true);
        }
        }

    // --------
    // bad_allocation2: trying to allocate memory for negative amount of objects, an exception thrown.
    // --------

    void bad_allocation2 () {
        A x;
        try {
        const pointer p = x.allocate(-15);
        x.destroy(p);
        x.deallocate(p, 0);
        }
        catch (std::bad_alloc& e)
        {
            CPPUNIT_ASSERT(true);
        }
        }

    // --------
    // bad_allocation3: trying to allocate memory for 0 object, nothing significant happened.
    // --------

    void bad_allocation3 () {
        A x;
        const pointer p = x.allocate(0);
        if (p == NULL) CPPUNIT_ASSERT(true);
        }

    // --------
    // max_allocation: trying to allocate all it can. (for double).
    // --------

    void max_allocation () {
        A x;
        try {
        const pointer p = x.allocate(11);
        x.destroy(p);
        x.deallocate(p, 0);
        CPPUNIT_ASSERT(true);
        }
        catch (std::bad_alloc& e)
        {
            CPPUNIT_ASSERT(false); // you won't get here.
        }
        }

    // --------
    // regular_allocation: trying to allocate some spaces.
    // --------

    void regular_allocation () {
        A x;
        try {
        const pointer p = x.allocate(5);
        x.destroy(p);
        x.deallocate(p, 0);
        CPPUNIT_ASSERT(true);
        }
        catch (std::bad_alloc& e)
        {
            CPPUNIT_ASSERT(false); // you won't get here.
        }
        }

    // --------
    // legal_valid: test the basic functionality of valid().
    // --------

    void legal_valid () {
        A x;
        pointer p = x.allocate(5);
        x.construct(p, 100);
        int* b_node = reinterpret_cast<int*>((int*)p - 1);
        CPPUNIT_ASSERT(x.valid() && *b_node == -5 * (int)sizeof(value_type)); 
        }

    // --------
    // illegal_valid1: change the value of the first node, valid should return false.
    // --------

    void illegal_valid1 () {
        A x;
        pointer p = x.allocate(5);
        x.construct(p, 100);
        int* b_node = reinterpret_cast<int*>((int*)p - 1);
        *b_node = 123;
        CPPUNIT_ASSERT(!x.valid()); 
        }

    // --------
    // illegal_valid2: change the value of the last node, valid should return false.
    // --------

    void illegal_valid2 () {
        A x;
        pointer p = x.allocate(5);
        x.construct(p, 100);
        int* b_node = reinterpret_cast<int*>((int*)p - 1);
        int* e_node = reinterpret_cast<int*>((char*)b_node + (std::abs(*b_node) + sizeof(int)));
        *e_node = 123;
        CPPUNIT_ASSERT(!x.valid()); 
        }


    // --------
    // deallocate_and_test_merge1: allocate one chunk of memory and then deallocate it, see if it merges.
    // --------

    void deallocate_and_test_merge1 () {
        A x;
        pointer p = x.allocate(5);
        x.construct(p, 100);
        int* b_node = reinterpret_cast<int*>((int*)p - 1);
        CPPUNIT_ASSERT(*b_node == -5 * (int)sizeof(value_type));
        x.destroy(p);
        x.deallocate(p, 0);
        CPPUNIT_ASSERT(*b_node == 92);
        }


    // --------
    // deallocate_and_test_merge2: allocate three chunks of memory, deallocate the first chunk and then the 
    //                                 second, see if it merges.
    // --------

    void deallocate_and_test_merge2 () {
        A x;
        pointer p1 = x.allocate(1);
        pointer p2 = x.allocate(2);
        pointer p3 = x.allocate(3);
        x.construct(p1, 1000000);
        x.construct(p2, 2000000);
        x.construct(p3, 3000000);
        x.destroy(p1);
        x.destroy(p2);
        x.deallocate(p1, 0);
        x.deallocate(p2, 0);
        int* b_node = reinterpret_cast<int*>((int*)p1 - 1);
        CPPUNIT_ASSERT(*b_node == 3 * (int)sizeof(value_type) + 2 * sizeof(int));
        x.destroy(p3);
        x.deallocate(p3, 0);
        CPPUNIT_ASSERT(*b_node == 92);
        }


    // --------
    // deallocate_and_test_merge3: allocate three chunks of memory, deallocate the first chunk and then the 
    //                                      third. And then deallocate the second chunk and see if it merges.
    // --------

    void deallocate_and_test_merge3 () {
        A x;
        pointer p1 = x.allocate(1);
        pointer p2 = x.allocate(2);
        pointer p3 = x.allocate(3);
        x.construct(p1, 1000000);
        x.construct(p2, 2000000);
        x.construct(p3, 3000000);
        x.destroy(p1);
        x.destroy(p3);
        x.deallocate(p1, 0);
        x.deallocate(p3, 0);
        int* b_node = reinterpret_cast<int*>((int*)p1 - 1);
        CPPUNIT_ASSERT(*b_node == (int)sizeof(value_type));
        int* b_node3 = reinterpret_cast<int*>((int*)p3 - 1);
        CPPUNIT_ASSERT(*b_node3 == 100 - ((int)sizeof(value_type) * 3 + (int)sizeof(int) * 6));
        x.destroy(p2);
        x.deallocate(p2, 0);
        CPPUNIT_ASSERT(*b_node == 92);
        }

//CPPUNIT_ASSERT(*b_node == 3 * (int)sizeof(value_type) + 2 * sizeof(int));



    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(bad_allocation1);
    CPPUNIT_TEST(bad_allocation2);
    CPPUNIT_TEST(bad_allocation3);
    CPPUNIT_TEST(max_allocation);
    CPPUNIT_TEST(regular_allocation);
    CPPUNIT_TEST(legal_valid);
    CPPUNIT_TEST(illegal_valid1);
    CPPUNIT_TEST(illegal_valid2);
    CPPUNIT_TEST(deallocate_and_test_merge1);
    CPPUNIT_TEST(deallocate_and_test_merge2);
    CPPUNIT_TEST(deallocate_and_test_merge3);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

  //  tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

 //   tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); 

    tr.run();

    cout << "Done." << endl;
    return 0;}
