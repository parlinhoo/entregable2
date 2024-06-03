#ifndef MAP_ADT
#define MAP_ADT

#include <optional>

template <class key_type, class value_type> class MapADT {
    virtual std::optional<value_type> get(key_type key) = 0;
    virtual void put(key_type key, value_type value) = 0;
    virtual std::optional<value_type> remove(key_type key) = 0;
    virtual int size() = 0;
    virtual bool is_empty() = 0;
};

#endif
