#ifndef DoubleClosedHashTableH
#define DoubleClosedHashTableH

#include <bitset>
#include <cstddef>
#include <iostream>
#include <vector>

#include "HashTableClosed.h"

template <class key_type, class value_type>
class DoubleClosedHashTable : public HashTableClosed<key_type, value_type>{
    protected:
        virtual size_t second_hash(key_type key) = 0;
        virtual size_t first_hash(key_type key) = 0;

        size_t hash(key_type key, int tries) {
            std::cout << "Intento " << tries+1 << " con index " << (first_hash(key) + tries*second_hash(key)) % this->vector_size << "\n";
            return (first_hash(key) + tries*second_hash(key)) % this->vector_size;
        } 
    public:
        DoubleClosedHashTable(size_t initial_size) : HashTableClosed<key_type, value_type>(initial_size) {}
};

#endif
