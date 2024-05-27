template <class key_t, class value_t> class kv_pair {
    private:
        key_t key;
        value_t value;
    public:
        kv_pair(key_t newkey, value_t newvalue) {
            this->key = newkey;
            this->value = newvalue;
        }
        key_t getKey() {
            return this->key;
        }
        value_t getValue() {
            return this->value;
        }
        bool isKey(key_t check) {
            return this->key == check;
        }
};  
