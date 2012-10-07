// ------------------------------
// projects/allocator/Allocator.h
// Copyright (C) 2011
// Glenn P. Downing
// ------------------------------

#ifndef Allocator_h
#define Allocator_h

// --------
// includes
// --------

#include <cassert>   // assert
#include <cstddef>   // size_t
#include <new>       // new
#include <stdexcept> // invalid_argument
#include <cstdlib>   // abs

// ---------
// Allocator
// ---------

template <typename T, std::size_t N>
class Allocator {
    public:
        // --------
        // typedefs
        // --------

        typedef T                 value_type;

        typedef std::size_t       size_type;
        typedef std::ptrdiff_t    difference_type;

        typedef value_type*       pointer;
        typedef const value_type* const_pointer;

        typedef value_type&       reference;
        typedef const value_type& const_reference;

    public:
        // -----------
        // operator ==
        // -----------

        friend bool operator == (const Allocator&, const Allocator&) {
            return true;}

        // -----------
        // operator !=
        // -----------

        friend bool operator != (const Allocator& lhs, const Allocator& rhs) {
            return !(lhs == rhs);}

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
        bool valid () const {    
            size_type index = 0;
            while (index < N)
            {
                const int* b = reinterpret_cast<const int *>(a + index);
                const int* e = reinterpret_cast<const int *>(a + (std::abs(*b) + sizeof(int) + index));
                std::cout << "index: " << index << std::endl; //debug............
                std::cout << "*b   : " << *b << std::endl; //debug............
                std::cout << "*e   : " << *e << std::endl << std::endl; //debug............
                if (*b != *e) return false;
                index += std::abs(*e) + 2 * sizeof(int);
            }
            return true;}

    public:
        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        Allocator () {
            int init_available_space = N - 2 * sizeof(int);
            int* p = reinterpret_cast<int*>(a);
            *p = init_available_space;
            p = reinterpret_cast<int*>(a + N - sizeof(int));
            *p = init_available_space;
            assert(valid());
        }

        // Default copy, destructor, and copy assignment
        // Allocator  (const Allocator<T>&);
        // ~Allocator ();
        // Allocator& operator = (const Allocator&);

        // --------
        // allocate
        // --------

        /**
         * O(1) in space
         * O(n) in time
         * <your documentation>
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
        pointer allocate (size_type n) {
            int required_size = n * sizeof(value_type);

            size_type index = 0;
            while (index < N)
            {
                int* b = reinterpret_cast<int *>(a + index);
                if (*b < 0 || *b < required_size)
                    index += std::abs(*b) + 2 * sizeof(int);
                else
                {
                    int available_space = *b;
                    int* e = reinterpret_cast<int *>(a + (std::abs(*b) + sizeof(int) + index));
                    *b = -(required_size);
                    int* e_allocated = reinterpret_cast<int *>(a + (std::abs(*b) + sizeof(int) + index));
                    *e_allocated = *b;
                    if (e != e_allocated) 
                    {
                        int* b_available = reinterpret_cast<int *>(a + (std::abs(*b) + 2 * sizeof(int) + index));
                        *b_available = available_space - 2 * sizeof(int) - required_size;
                        *e = *b_available;
                    }
                    assert(valid());
                    return reinterpret_cast<pointer>(b + 1);
                }
            }
            assert(valid());
            std::cout << "There is not enough space available. Please deallocate some data first.\n";
            return 0;}


        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void construct (pointer p, const_reference v) {
            // new (p) T(v);                            // uncomment!
            assert(valid());}

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type = 0) {
            // <your code>
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * <your documentation>
         */
        void destroy (pointer p) {
            // p->~T();            // uncomment!
            assert(valid());}};

#endif // Allocator_h
