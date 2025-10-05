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

void implementation::emscripten::assets::init(void* ref) {
    TRACE_SCOPE;

    _base = (char*)ref;
}

std::vector<std::string> implementation::emscripten::assets::list(const std::string& path, const std::string& type) {
    if (type.empty()) {
        return filesystem->read_directory(filesystem->join({ _base, path }));
    }
    std::vector<std::string> results;
    for (auto entry : filesystem->read_directory(filesystem->join({ _base, path }))) {
        if (filesystem->filetype(filesystem->join({ _base, path, entry })) == type) {
            results.push_back(entry);
        }
    }
    return results;
}

std::istream& implementation::emscripten::assets::retrieve(const std::string& path) {
    TRACE_SCOPE;

    auto file = new std::ifstream();
    if (file == NULL) {
        // TODO : care about this
    }

    std::vector<std::string> directories = { _base };
    for (auto path : utilities::tokenize(resolve(path), "/")) {
        directories.push_back(path);
    }
    auto asset = filesystem->join(directories);

    //for(auto entry : filesystem->read_directory("/")) {
    //    trace->debug() << "root: " << entry;
    //    for(auto subentry : filesystem->read_directory("/" + entry)) {
    //        trace->debug() << "  sub: " << subentry;
    //    }
    //}

    trace->debug() << "retrieving asset: " << asset;

    file->open(asset.c_str(), std::ios::in | std::ios::binary);
    if (file->is_open() == false) {
        trace->error() << "failed to retrieve asset: " << asset;
        auto error = strerror(errno);
        event(utilities::string() << asset << ", failed to retrieve asset");
    }

    // push onto the stack regardless of success or failure
    assets::source entry = { utilities::dirname(path), file };
    _stack.push_back(entry);

    return *file;
}

void implementation::emscripten::assets::release() {
    TRACE_SCOPE;

    if (_stack.size() == 0) {
        return;
    }
    std::ifstream *ref = (std::ifstream *)_stack.back().stream;
    if (ref != NULL) {
        ref->close();
        delete ref;
    }
    _stack.pop_back();
}

std::string implementation::emscripten::assets::load(const std::string& type, const std::string& resource, const std::string& id) {
    TRACE_SCOPE;

    if (_loader == NULL) {
        _loader = new implementation::universal::assets();
        _loader->copy(*this);
    }
    return _loader->load(this, type, resource, id);
}

#endif
