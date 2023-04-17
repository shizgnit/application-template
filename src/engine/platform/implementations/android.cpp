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

#if defined __PLATFORM_ANDROID

struct membuf : std::streambuf
{
    membuf(char* ptr, off_t size)
    {
        this->setg(ptr, ptr, ptr+size);
    }

    pos_type seekoff(off_type off, std::ios_base::seekdir dir, std::ios_base::openmode which = std::ios_base::in) override
    {
        if (dir == std::ios_base::cur)
            gbump(off);
        else if (dir == std::ios_base::end)
            setg(eback(), egptr() + off, egptr());
        else if (dir == std::ios_base::beg)
            setg(eback(), eback() + off, egptr());
        return gptr() - eback();
    }

    pos_type seekpos(pos_type sp, std::ios_base::openmode which) override
    {
        return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
    }
};

void implementation::android::assets::init(void* ref) {
    assetManager = (AAssetManager*)ref;
}

std::vector<std::string> implementation::android::assets::list(const std::string& path, const std::string& type) {
    std::vector<std::string> results;

    std::vector<std::string> stack;
    std::vector<std::string> directories;

    // Compensation for AAssetDir not listing directories
    AAsset* asset = AAssetManager_open(assetManager, "content.txt", AASSET_MODE_BUFFER);
    if (!asset) {
        event("failed to retrieve asset list from the content.txt");
        return results;
    }
    auto contents = std::string((char*)AAsset_getBuffer(asset), AAsset_getLength(asset));
    for (auto line : utilities::tokenize(contents)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        auto pos = line.find_first_of("+\\");
        if (pos == std::string::npos) {
            continue;
        }
        if (line[pos] == '\\' && stack.size() == 0) {
            continue;
        }
        int depth = pos / 4;
        while (stack.size() && depth < stack.size()) {
            auto directory = filesystem->join(stack);
            if (directory.length() > path.length() && directory.substr(0, path.length()) == path) {
                if (type.empty() || type == "directory") {
                    results.push_back(directory.substr(path.length() + 1, directory.length() - path.length() - 1));
                }
            }
            stack.pop_back();
        }
        stack.push_back(line.substr(pos + 4, line.length() - 4));
    }

    if (type.empty() || type == "regular") {
        auto handle = AAssetManager_openDir(assetManager, path.c_str());
        while (auto file = AAssetDir_getNextFileName(handle)) {
            results.push_back(file);
        }
        AAssetDir_close(handle);
    }

    return results;
}

std::istream &implementation::android::assets::retrieve(const std::string& path) {
    auto ss = new std::stringstream;
    ss->str(std::string());
    ss->clear();

    std::string fullpath = resolve(path);

    const char* spath = fullpath.c_str();
    AAsset* asset = AAssetManager_open(assetManager, fullpath.c_str(), AASSET_MODE_BUFFER);
    if (asset) {
        ss->write((char*)AAsset_getBuffer(asset), AAsset_getLength(asset));
    }
    else {
        event(fullpath + ", failed to retrieve asset");
    }
    AAsset_close(asset);

    assets::source entry = { utilities::dirname(path), ss };
    stack.push_back(entry);

    return *ss;
}

void implementation::android::assets::release() {
    if (stack.size() == 0) {
        return;
    }
    std::stringstream* ref = (std::stringstream*)stack.back().stream;
    if (ref != NULL) {
        // No special resource release for stringstreams
        delete ref;
    }
    stack.pop_back();
}


std::string implementation::android::assets::load(const std::string& type, const std::string& resource, const std::string& id) {
    if (loader == NULL) {
        loader = new implementation::universal::assets();
        loader->copy(*this);
    }
    return loader->load(this, type, resource, id);
}


#endif