#include "engine.hpp"

bool properties::flag(const label_t& label) {
    if (variables.find(label) == variables.end()) {
        return false;
    }
    auto variable = variables[label];
    if (std::holds_alternative<bool>(variable)) {
        return std::get<bool>(variable);
    }
    if (std::holds_alternative<int>(variable)) {
        return std::get<int>(variable) != 0;
    }
    if (std::holds_alternative<double>(variable)) {
        return std::get<double>(variable) > 0.0f;
    }
    if (std::holds_alternative<std::string>(variable)) {
        return std::get<std::string>(variable).empty();
    }
    if (std::holds_alternative<spatial::vector>(variable)) {
        return std::get<spatial::vector>(variable).hasValue();
    }
    return false;
}

bool properties::has(const label_t& label) {
    return variables.find(label) != variables.end();
}

value_t properties::get(const label_t& label) {
    return variables[label];
}

value_t properties::set(const label_t& label, value_t value) {
    variables[label] = value;
    return value;
}

size_t properties::events() {
    return _events.size();
}

void properties::event(const std::string &e) {
    _events.push_back(e);
}

std::string properties::event() {
    if (_events.size() == 0) {
        return "";
    }
    std::string status = _events.front();
    _events.pop_front();
    return status;
}
