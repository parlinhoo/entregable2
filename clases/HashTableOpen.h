#include <cstddef>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <list>
#include <vector>

#include "MapADT.h"
#include "KeyValuePair.h"

// Clase para la tabla hash con hashing abierto (encadenamiento)
template <class key_type, class value_type> // Template para crear una hash table con llave key_type y valor (o valores) value_type
class HashTableOpen : public MapADT<key_type, value_type>{
protected:
    // Guarda pares llave-valor
    std::vector<std::list<kv_pair<key_type, value_type>>> table;
    size_t vector_size;
    
    size_t non_empty_spaces;
    // Funcion hash abstracta que se tiene que sobreescribir
    virtual size_t hash(key_type key) = 0;

public:
    HashTableOpen(size_t initialSize) : vector_size(initialSize) {
        table.resize(vector_size);
        this->non_empty_spaces = 0;
    }
    
    std::optional<value_type> get(key_type key) {
        size_t index = hash(key);
        for (auto pair : table[index]) {
            if (pair.is_key(key)) {
                return pair.get_value();
            } 
        }
        return std::nullopt;
    }
    
    void put(key_type key, value_type value) {
        size_t index = hash(key);
        if (table[index].empty()) this->non_empty_spaces++;
        kv_pair<key_type, value_type> pair(key, value);
        table[index].push_back(pair);
    }

    std::optional<value_type> remove(key_type key) {
        size_t index = hash(key);
        auto iterator = table[index].begin();

        while (iterator != table[index].end()) {
            auto pair = *iterator;
            if (pair.is_key(key)) {
                if (table[index].size() == 1) this->non_empty_spaces--;
                value_type value = pair.get_value();
                table[index].erase(iterator);
                return value;
            }
            iterator++;
        }

        return std::nullopt;
    }

    int size() {
        return this->non_empty_spaces;
    }

    bool is_empty() {
        return this->non_empty_spaces == 0;
    }

    void print_contents() {
        for (int i = 0; i < this->vector_size; i++) {
            if (!table[i].empty()) {
                std::cout << "[" << i << "]: ";
                for (const auto pair : table[i]) {
                    std::cout << "(" << pair.get_key() << ", " << pair.get_value().userID << ") ";
                }
                std::cout << "\n";
            }
        }
    }
};
