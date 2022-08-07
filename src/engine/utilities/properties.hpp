typedef std::string label_t;
typedef std::variant<bool, double, int, std::string, spatial::vector> value_t;

class properties {
public:
    static properties& empty() {
        static properties instance;
        return instance;
    }

    typedef std::map<label_t, value_t> type_t;

    bool flag(const label_t& label);
    bool has(const label_t& label);
    value_t get(const label_t& label);
    value_t set(const label_t& label, value_t value);

    std::vector<label_t> keys() {
        std::vector<label_t> results;
        for (auto entries : variables) {
            results.push_back(entries.first);
        }
        return results;
    }

    void copy(const properties& ref) {
        variables = ref.variables;
    }

protected:
    type_t variables;
};