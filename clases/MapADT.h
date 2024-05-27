#include <optional>

template <class key_type, class value_type> class MapADT {
    virtual std::optional<value_type> get(key_type key);
    virtual void put(key_type key, value_type value);
    virtual std::optional<value_type> remove(key_type key);
    virtual int size();
    virtual bool isEmpty();
};
