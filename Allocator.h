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
         * Iterates through a[N] and makes sure that all of its nodes are correctly
         * set and formatted.
         */
        bool valid () const {    
            size_type index = 0;
            while (index < N)
            {
                const int* b = reinterpret_cast<const int *>(a + index);
                const int* e = reinterpret_cast<const int *>(a + (std::abs(*b) + sizeof(int) + index));
                if (*b != *e) return false;
                index += std::abs(*e) + 2 * sizeof(int);
            }
            return true;}

        /**
         * Used by deallocate to merge free contiguous blocks of free memory to the right
         */
        int* try_merge_right (int *b, int *e)
        {
            int* b_next_chunk = e + 1;
            int* e_next_chunk = reinterpret_cast<int*>((char *)b_next_chunk + (std::abs(*b_next_chunk) + sizeof(int)));
            if (*b_next_chunk > 0)
            {
                int new_size = std::abs(*e) + *b_next_chunk + 2 * sizeof(int);
                *b = *e_next_chunk = new_size;
            }
            else
            {
                *b = *e = std::abs(*b);
            }
            return e_next_chunk;    
        } 

        /**
         * Used by deallocate to merge free contiguous blocks of free memory to the left
         */
        int* try_merge_left (int *b, int *e)
        {
            int* e_previous_chunk = b - 1;
            int* b_previous_chunk = reinterpret_cast<int*>((char *)e_previous_chunk - (std::abs(*e_previous_chunk) + sizeof(int)));
            if (*e_previous_chunk > 0)
            {
                int new_size = std::abs(*b) + *e_previous_chunk + 2 * sizeof(int);
                *e = *b_previous_chunk = new_size;
            }
            else
            {
                *b = *e = std::abs(*b);
            }
            return b_previous_chunk;
        } 

    public:

        void debug () const {    
            size_type index = 0;
            while (index < N)
            {
                const int* b = reinterpret_cast<const int *>(a + index);
                const int* e = reinterpret_cast<const int *>(a + (std::abs(*b) + sizeof(int) + index));
                std::cout << "index: " << index << std::endl; //debug............
                std::cout << "*b   : " << *b << std::endl; //debug............
                std::cout << "*e   : " << *e << std::endl << std::endl; //debug............
                if (*b != *e) std::cout << "FUCKED UP" << std::endl; //debug.............
                index += std::abs(*e) + 2 * sizeof(int);
            }
            std::cout << "\n\n\n";
        }


        // ------------
        // constructors
        // ------------

        /**
         * O(1) in space
         * O(1) in time
         * Sets up initial nodes in a[N] to total free space available in block
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
         * Finds first available block of memory with enough free space for n objects.
         * When appropriate block found, new nodes are set up at either end with new
         * free space and the old nodes are changed accordingly to the amount of space
         * left. Also makes sure there is enough room to allocate.
         * after allocation there must be enough space left for a valid block
         * the smallest allowable block is sizeof(T) + (2 * sizeof(int))
         * choose the first block that fits
         */
        pointer allocate (size_type n) {
	    assert (n >= 0);
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
		    assert (b != NULL);
                    return reinterpret_cast<pointer>(b + 1);
                }
            }
            throw std::bad_alloc();
            assert(valid());
            return 0;}


        // ---------
        // construct
        // ---------

        /**
         * O(1) in space
         * O(1) in time
         * Constructor for object.
         */
        void construct (pointer p, const_reference v) {
            new (p) T(v);                            // uncomment!
            assert(valid());}

        // ----------
        // deallocate
        // ----------

        /**
         * O(1) in space
         * O(1) in time
         * Goes to pointer p in a[N] and then checks to see if nodes next to block to be 
         * deallocated indicate free space.  If so, try to merge in that (or those)
         * direction(s).
         * after deallocation adjacent free blocks must be coalesced
         */
        void deallocate (pointer p, size_type = 0) {
	    assert (p != NULL);
            int* b = reinterpret_cast<int *>((int*)p - 1);
            int* e = reinterpret_cast<int *>((char *)b + (std::abs(*b) + sizeof(int)));
            char* very_end = (char *)e + sizeof(int);
            if ((char*)b == &a[0]) 
            {
                try_merge_right(b, e);
            }
            else if (very_end == &a[N])
            {
                try_merge_left(b, e);
            }
            else
            {
                b = try_merge_left(b, e);
                try_merge_right(b, e);
            }
            assert(valid());}

        // -------
        // destroy
        // -------

        /**
         * O(1) in space
         * O(1) in time
         * Destructor for object
         */
        void destroy (pointer p) {
            p->~T();            // uncomment!
            assert(valid());}};

#endif // Allocator_h
