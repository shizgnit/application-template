#pragma once

namespace type {

    class shader : virtual public type::info {
    public:
        std::string text;

        shader& operator=(const shader& ref) {
            type::info::operator=(ref);
            text = ref.text;
            return *this;
        }
        
        friend type::shader& operator>>(type::shader& input, type::shader& instance) {
            instance = input;
            return instance;
        }

        std::string type() {
            return "type::shader";
        }

        bool empty() {
            return text.empty();
        }
    };

}
