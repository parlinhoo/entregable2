#ifndef HASH_TABLE_CLOSED
#define HASH_TABLE_CLOSED

#include <optional>
#include <vector>
#include <iostream>

#include "KeyValuePair.h"
#include "MapADT.h"

template <class key_type, class value_type>
class HashTableClosed : public MapADT<key_type, value_type>{
    protected:
        std::vector<kv_pair<key_type, value_type>> table;
        std::vector<bool> had_value;        
        size_t vector_size;
        size_t non_empty_spaces;
        
        // Funcion hash que se le da la llave a buscar y los intentos que ha realizado antes
        virtual size_t hash(key_type key, int tries) = 0; 
    public:
        HashTableClosed(size_t initial_size) : vector_size(initial_size) {
            table.resize(initial_size);
            this->had_value.resize(initial_size);
            this->non_empty_spaces = 0;
        }

        std::optional<value_type> get(key_type key) override {
            int index, tries = 0;
            for (int i = 0; i < this->vector_size; i++) {
                index = this->hash(key, tries);
                tries++;
                if (this->table[index].is_key(key)) return this->table[index].get_value(); // Buscar si es el valor que buscamos, y si es asi devolverlo
                if (!this->had_value[index]) return std::nullopt; // Si la casilla no ha tenido nunca un valor, significa que no está el valor en la tabla
            }
            return std::nullopt; // Si ya recorrimos toda la tabla, significa que no está en ella
        }

        void put(key_type key, value_type value) override {
            int index, tries = 0;
            for (int i = 0; i < this->vector_size; i++) {
                index = this->hash(key, tries);
                tries++;
                if (this->table[index].is_null()) {
                    if (!this->had_value[index]) this->had_value[index] = 1; // Si esa casilla no ha tenido nunca un valor, la marcamos para que si tenga
                    kv_pair<key_type, value_type> pair(key, value);
                    this->table[index] = pair; // Creamos un par clave valor nuevo y lo agregamos a la tabla
                    this->non_empty_spaces++; // Hay un espacio no vacío más
                    return;
                }
            }
            std::cout << "El valor con clave " << key << " no fue insertado porque la tabla está llena.\n";
        }

        std::optional<value_type> remove(key_type key) override {
            int index, tries = 0;
            for (int i = 0; i < this->vector_size; i++) {
                index = this->hash(key, tries);
                tries++;
                if (this->table[index].is_key(key)) {
                    value_type value = this->table[index].get_value(); // Obtener valor
                    this->table[index] = kv_pair<key_type, value_type>(); // Agregar un par clave valor default a la tabla
                    this->non_empty_spaces--; // Hay un espacio no vacío menos 
                    return value;
                }
                if (!this->had_value[index]) return std::nullopt;
            }
            return std::nullopt;
        }

        int size() override {
            return this->non_empty_spaces;        
        }

        bool is_empty() override {
            return this->non_empty_spaces == 0;
        }

        void print_contents() {
            for (int i = 0; i < this->vector_size; i++) {
                if (!this->table[i].is_null()) {
                    std::cout << "[" << i << "]: " << this->table[i].get_value() << " (Key: " << this->table[i].get_key() << ")\n"; 
                }
            }
        }
};

#endif
