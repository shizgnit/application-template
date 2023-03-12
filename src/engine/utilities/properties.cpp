/*
================================================================================
  Copyright (c) 2023, Dee E. Abbott
  All rights reserved.
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the organization nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================
*/

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
