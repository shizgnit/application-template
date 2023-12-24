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

#if defined __PLATFORM_IOS

#include <CoreFoundation/CoreFoundation.h>

#include <malloc/malloc.h>

void implementation::ios::assets::init(void* ref) {
    if(ref != NULL) {
        base = (char *)ref;
        return;
    }
    
    CFBundleRef bundle = CFBundleGetMainBundle();
    CFURLRef url = CFBundleCopyResourceURL(bundle, CFSTR("assets/content"), CFSTR("txt"), NULL);
    CFStringRef path = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);

    CFStringEncoding encoding = kCFStringEncodingUTF8;
    CFIndex length = CFStringGetLength(path);
    CFIndex size = CFStringGetMaximumSizeForEncoding(length, encoding);

    char *filename = (char *)malloc(size + 1);

    CFStringGetCString(path, filename, size, encoding);

    if(filesystem->exists(filename) == false) {
        throw("failed to locate asset bundle");
    }

    base = filesystem->dirname(filename);
    
    if(filename) {
        free(filename);
        filename = NULL;
    }
    if(path) {
        CFRelease(path);
        path = NULL;
    }
    if(url) {
        CFRelease(url);
        url = NULL;
    }
}

std::vector<std::string> implementation::ios::assets::list(const std::string& path, const std::string& type) {
    if (type.empty()) {
        return filesystem->read_directory(filesystem->join({ base, path }));
    }
    std::vector<std::string> results;
    for (auto entry : filesystem->read_directory(filesystem->join({ base, path }))) {
        if (filesystem->filetype(filesystem->join({ base, path, entry })) == type) {
            results.push_back(entry);
        }
    }
    return results;
}

std::istream& implementation::ios::assets::retrieve(const std::string& path) {
    auto file = new std::ifstream();
    if (file == NULL) {
        // TODO : care about this
    }

    std::vector<std::string> directories = { base };
    for (auto path : utilities::tokenize(resolve(path), "/")) {
        directories.push_back(path);
    }
    auto asset = utilities::join("/", directories);

    file->open(asset.c_str(), std::ios::in | std::ios::binary);
    if (file->is_open() == false) {
        event(utilities::string() << asset << ", failed to retrieve asset");
    }

    // push onto the stack regardless of success or failure
    assets::source entry = { utilities::dirname(path), file };
    stack.push_back(entry);

    return *file;
}

void implementation::ios::assets::release() {
    if (stack.size() == 0) {
        return;
    }
    std::ifstream *ref = (std::ifstream *)stack.back().stream;
    if (ref != NULL) {
        ref->close();
        delete ref;
    }
    stack.pop_back();
}

std::string implementation::ios::assets::load(const std::string& type, const std::string& resource, const std::string& id) {
    if (loader == NULL) {
        loader = new implementation::universal::assets();
        loader->copy(*this);
    }
    return loader->load(this, type, resource, id);
}

#endif
