template <class key_type, class value_type> 
class kv_pair {
    private:
        key_type key;
        value_type value;
    public:
        kv_pair(key_type newkey, value_type newvalue) {
            this->key = newkey;
            this->value = newvalue;
        }
        key_type get_key() const {
            return this->key;
        }
        value_type get_value() const {
            return this->value;
        }
        bool is_key(key_type check) const {
            return this->key == check;
        }
};  