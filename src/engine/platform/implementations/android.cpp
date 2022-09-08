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

std::vector<std::string> implementation::android::assets::list(const std::string& path) {
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
                results.push_back(directory.substr(path.length() + 1, directory.length() - path.length() - 1));
            }
            stack.pop_back();
        }
        stack.push_back(line.substr(pos + 4, line.length() - 4));
    }

    auto handle = AAssetManager_openDir(assetManager, path.c_str());
    while (auto file = AAssetDir_getNextFileName(handle)) {
        results.push_back(file);
    }
    AAssetDir_close(handle);

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
    }
    return loader->load(this, type, resource, id);
}


#endif