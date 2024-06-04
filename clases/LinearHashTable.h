#ifndef LINEARHASHTABLE_H
#define LINEARHASHTABLE_H

#include "HashTableClosed.h"
#include "KeyValuePair.h"
#include <vector>
#include <optional>

template <class key_type, class value_type>
class LinearHashTable : public HashTableClosed<key_type, value_type> {
private:
    void resize() {
        size_t new_size = this->vector_size * 2; // Duplicar el tamaño de la tabla
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
    LinearHashTable(size_t initial_size) : HashTableClosed<key_type, value_type>(initial_size) {}
};

#endif // LINEARHASHTABLE_H
