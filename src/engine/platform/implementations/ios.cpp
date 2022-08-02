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

std::vector<std::string> implementation::ios::assets::list(const std::string& path) {
    return filesystem->read_directory(utilities::join("/", std::vector<std::string>({ base, path })));
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
        errors.push_back(asset + ", failed to retrieve asset");
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
    }
    return loader->load(this, type, resource, id);
}

#endif
