#include <optional>

template <class key_type, class value_type> class MapADT {
    virtual std::optional<value_type> get(key_type key);

};
