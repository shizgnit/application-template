/*
================================================================================
  Copyright (c) 2023, Pandemos
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

#if defined __PLATFORM_EMSCRIPTEN

std::string implementation::emscripten::filesystem::seperator() {
    return {};
}

bool implementation::emscripten::filesystem::cp(const std::string&, const std::string&) {
    return false;
}

bool implementation::emscripten::filesystem::rm(const std::string&) {
    return false;
}

bool implementation::emscripten::filesystem::mv(const std::string&, const std::string&) {
    return false;
}

bool implementation::emscripten::filesystem::mkdir(const std::string&, unsigned int) {
    return false;
}

bool implementation::emscripten::filesystem::rmdir(const std::string&) {
    return false;
}

std::string implementation::emscripten::filesystem::pwd(const std::string&) {
    return {};
}


std::vector<unsigned long> implementation::emscripten::filesystem::stat(const std::string&) {
    return {};
}

std::vector<unsigned long> implementation::emscripten::filesystem::lstat(const std::string&) {
    return {};
}

bool implementation::emscripten::filesystem::exists(const std::string&) {
    return false;
}

std::string implementation::emscripten::filesystem::filetype(const std::string&) {
    return {};
}


std::pair<int, std::string> implementation::emscripten::filesystem::error() {
    return {0, {}};
}

std::vector<std::string> implementation::emscripten::filesystem::read_directory(const std::string&, bool) {
    return {};
}

bool implementation::emscripten::filesystem::is_directory(const std::string&) {
    return false;
}

std::string implementation::emscripten::filesystem::join(const std::vector<std::string>&) {
    return {};
}

std::string implementation::emscripten::filesystem::dirname(const std::string&) {
    return {};
}
std::string implementation::emscripten::filesystem::basename(const std::string&) {
    return {};
}

std::string implementation::emscripten::filesystem::home(const std::string&) {
    return {};
}

std::string implementation::emscripten::filesystem::appdata(const std::string&) {
    return {};
}

void implementation::emscripten::assets::init(void*) {
    // noop
}

std::vector<std::string> implementation::emscripten::assets::list(const std::string&, const std::string&) {
    return {};
}

std::istream& implementation::emscripten::assets::retrieve(const std::string&) {
    static std::istringstream dummy;
    return dummy;
}

void implementation::emscripten::assets::release() {
    // noop
}

std::string implementation::emscripten::assets::load(const std::string&, const std::string&, const std::string&) {
    return {};
}

#endif
