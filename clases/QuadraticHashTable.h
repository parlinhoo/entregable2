#ifndef QUADRATICHASHTABLE_H
#define QUADRATICHASHTABLE_H

#include "HashTableClosed.h"

template <class key_type, class value_type>
class QuadraticHashTable : public HashTableClosed<key_type, value_type> {
private:
    /*
    size_t hash(key_type key, int tries) {
        if constexpr (std::is_same_v<key_type, std::string>) {
            // Función de hash para cadenas de caracteres
            size_t hash_val = 0;
            for (char c : key) {
                hash_val = (hash_val * 127 + c) % this->vector_size;
            }
            return (hash_val + tries * tries) % this->vector_size;
        } else {
            // Función de hash para otros tipos de datos
            return (key + tries * tries) % this->vector_size;
        }
    }
    */

    void resize() {
        size_t new_size = this->vector_size * 2;
        std::vector<kv_pair<key_type, value_type>> new_table(new_size);

        // Rehash todos los elementos en la nueva tabla
        for (const auto& kv : this->table) {
            if (!kv.is_null()) {
                for (size_t i = 0; i < new_size; ++i) {
                    size_t idx = hash(kv.get_key(), i);
                    if (new_table[idx].is_null()) {
                        new_table[idx] = kv;
                        break;
                    }
                }
            }
        }

        // Actualizar la tabla y su tamaño
        this->table = new_table;
        this->vector_size = new_size;
    }

public:
    QuadraticHashTable(size_t initial_size) : HashTableClosed<key_type, value_type>(initial_size) {}
};

#endif // QUADRATICHASHTABLE_H
